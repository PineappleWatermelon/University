#include "dataworker.h"
#include <QtNetwork>
#include <QXmlStreamReader>
#include<QDebug>
/**
 * @brief dataWorker::dataWorker 构造函数
 * @param parent
 *
 * 默认构造函数，创建网络访问管理类QNetworkAccessManager对象<br/>
 * 并连接相应的信号与槽。
 *
 */
dataWorker::dataWorker(QObject *parent) :
    QObject(parent),splitter("#-#-#"),dataPath("data")
{
    initNetwork();
    display=true;        //默认显示气温
}

/**
 * @brief dataWorker::dataWorker
 * @param date 需要获取数据的年和月份
 * @param parent
 *
 * 重载构造函数，带一个参量 \a date,
 */
dataWorker::dataWorker(QString date, QObject *parent) :
    QObject(parent),splitter("#-#-#"),dataPath("data")
{
    _requestDate= date.replace("-","");
    initNetwork();
}

/**
 * @brief 设置请求年月的setter函数
 * @param newDate 新的年月
 *
 * 该函数简单的将传入年月做一个处理，删除"-"，<br/>
 * 如传入“2016-10”，则转换为“201610”的格式，以符合HTTP请求链接的格式。
 *
 */
void dataWorker::setRequestDate(QString newDate)
{
    _requestDate = newDate.replace("-","");
}

void dataWorker::setRequestCity(QString newCity)
{
    _requestCity = newCity;
}


/**
 * @brief getter函数，获得当前的年月
 * @return 请求年月
 */
QString dataWorker::requestDate()
{
    return _requestDate;
}

QString dataWorker::requestCity()
{
    return _requestCity;
}

/**
 * @brief 开始发送请求
 *
 * 该函数首先检查是否存在本地文件，如果有则读取本地数据文件；<br/>
 * 否则，发起http请求，从网络获取数据。
 */
void dataWorker::doRequest()
{
    // 导入数据，首先检查是否已经存在数据文件
     QString fName;
    if(display)        //便于区分,使用不同的文件名
        fName = QString("%1/weather_%2-%3.txt").arg(dataPath).arg(_requestCity).arg(_requestDate);
    else {
        fName = QString("%1/aqi_%2-%3.txt").arg(dataPath).arg(_requestCity).arg(_requestDate);
    }
    QStringList dataList;
    QFile f(fName);
    if(f.open(QIODevice::ReadOnly|QIODevice::Text)){  // 成功打开数据文件，则由文件中读取
        qDebug().noquote()<<QString("数据由文件%1导入...").arg(fName);
        QTextStream stream (&f);
        while(!stream.atEnd())
             dataList<<stream.readLine();
        // 数据导入完成，开始解析
        parseData(dataList.join(splitter).simplified());
    }else{
        // 如果无数据文件，则从网络获取
        qDebug().noquote()<<QString("数据由网络获取...");
        httpGet(requestUrl());
    }
}

void dataWorker::setdisplay(bool tempOrAQI)
{
    this->display=tempOrAQI;        //设置查找temp or AQI
}

/**
 * @brief 构造实际请求链接
 * @return 数据页面地址
 *
 * 该函数将请求年月插入模板中，获得实际数据页面的链接地址。
 */
QString dataWorker::requestUrl()
{    
    QString r;
    if(display)    //查询温度
        r = QString("https://lishi.tianqi.com/%1/%2.html").arg(_requestCity).arg(_requestDate);
    else                 //查询AQI
        r = QString("http://www.tianqihoubao.com/aqi/%1-%2.html").arg(_requestCity).arg(_requestDate);

    qDebug()<<r;
    return r;
}

/**
 * @brief 初始化网络请求管理类QNetworkAccessManager类
 *
 *  该函数创建一个QNetworkAccessManager类的指针对象，并连接相应的信号和槽。
 */
void dataWorker::initNetwork()
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,this, &dataWorker::httpsFinished);
}

/**
 * @brief 解析HTML
 * @param sourceText HTML文本
 *
 * 该函数使用QXmlStreamReader对HTML文本进行解析。<br/>
 * 注意： HTML必须是符合XML规范的文本（标签对完整）<br/>
 *
 * QXmlStreamReader类提供了一个针对良好格式化的基于流方式的XML快速解析器，<br/>
 * 适合于实现一次性解析任务（“一次性解析”任务，可以理解为只需读取文档一次，<br/>
 * 从头到尾一次性遍历处理XML文档，期间不会有反复的情况）。<br/>
 * <br/>
 *
 */
void dataWorker::parseHTML(const QString sourceText)
{
    // 使用QXmlStreamReader解析Html文档
    QXmlStreamReader reader(sourceText.simplified().trimmed());

    QStringList strData;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if(display){
                if (reader.name() == "ul"){         // 查找Html标签：ul
                    strData<<reader.readElementText(QXmlStreamReader::IncludeChildElements).trimmed();
                }
            }
            else{
                if (reader.name() == "tr"){         // 查找Html标签：tr
                    strData<<reader.readElementText(QXmlStreamReader::IncludeChildElements).trimmed();
                }
            }
        }
    }
    if (reader.hasError()) {
        qDebug()<< "  读取错误： " << reader.errorString();
    }else{
        if(!strData.isEmpty()){
            parseData(strData.join(splitter));
            exportDataToFile(strData.join(splitter));
        }
    }

}

/**
 * @brief 解析数据
 * @param sourceText 需要解析的文本
 *
 * 该函数处理parseHTML函数提取的文本，对其按照数据格式进行分割。 <br/>
 * 本程序只需使用前三列数据，在解析时每行文本只需使用前三个数据即可。<br/>
 * 数据解析完成后，发送dataParseFinished信号通知界面进行数据更新。<br/>
 */
void dataWorker::parseData(const QString sourceText)
{
    QStringList dataList = sourceText.split(splitter);

    dataDate.clear();
    dataHigh.clear();
    dataLow.clear();

    dataList.removeFirst();                  // 第一条数据是表头，删除
    for (QString s : dataList){
        QStringList dataList = s.split(" ",QString::SkipEmptyParts);
        QDateTime momentInTime = QDateTime::fromString(dataList.at(0),"yyyy-MM-dd");
        dataDate.append(momentInTime);
        if(!display){               //读取 AQI 的 3 列 AQI 和 5 列 PM2.5
            dataHigh.append(dataList.at(2).toDouble());
            dataLow.append(dataList.at(4).toDouble());
        }
        else
            if (display){         //读取temp 的 2列 最高气温 和 3 列的最低气温
                dataHigh.append(dataList.at(1).toDouble());
                dataLow.append(dataList.at(2).toDouble());
            }
    }
    emit dataParseFinished(dataDate,dataHigh,dataLow);   //转到 绘图槽
}

/**
 * @brief 将数据输出为文件
 * @param dataText 需要输出的数据文本
 *
 * 该类使用QFile和QTextStream类实现文本的保存。
 */
void dataWorker::exportDataToFile(const QString dataText)
{
    QStringList data = dataText.split(splitter);
    QDir dir;
    if( ! dir.exists(dataPath) )
        qDebug()<<dir.mkdir(dataPath);
//示例文件名: weather_nanjing-201809.txt  , 同时更新之前的读取文档程序   
    QString fName;
    if(display)                 // 温度 文件
        fName = QString("%1/weather_%2-%3.txt").arg(dataPath).arg(_requestCity).arg(_requestDate);
    else {                              //AQI 文件
        fName = QString("%1/aqi_%2-%3.txt").arg(dataPath).arg(_requestCity).arg(_requestDate);
    }
    QFile f(fName);
    if(f.open(QIODevice::WriteOnly|QIODevice::Text)){
        QTextStream stream (&f);
        for( QString d : data)
            stream << d <<"\n";
    }else{
        qDebug()<<"打开文件错误";
    }

}

/**
 * @brief 执行http请求（GET方法）
 * @param url 请求目标网址
 */
void dataWorker::httpGet(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    manager->get(request);
}

/**
 * @brief 网络请求完成finished信号的响应槽函数
 * @param reply web服务器响应
 *
 */
void dataWorker::httpsFinished(QNetworkReply *reply)
{
    int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString error="";
    if (reply->error()) {
        // 通知GUI，http访问出错
        error = QString("访问出错：代码 %1,原因 %2").arg(v).arg(reply->errorString());
        emit httpRequestError(error);
        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }    
    if(v != 200){        
        error = QString("访问出错：代码 %1,原因 %2").arg(v).arg(reply->errorString());
        emit httpRequestError(error);
        reply->deleteLater();
        reply = Q_NULLPTR;
        return;
    }

    qDebug()<<"开始读取返回的数据：";
    QString html = QString::fromLocal8Bit(reply->readAll());

    // 对QNetworkReply对象，在使用完成后必须由程序员进行释放
    // 释放时，不能直接使用delete函数，而使用deleteLater函数
    reply->deleteLater();
    reply = Q_NULLPTR;

    qDebug()<<"返回数据长度："<<html.size();

    // qDebug最大显示27605个
    // qDebug()<<html.left(27605);

    // 先做一个简单处理，将包含内容的完整<div>..</div>标签内的文本内容，
    // 并滤除其中的空白字符"\r\n\t"
    if(display)
    {               //匹配 温度 网页的 html 文本
        int begin = html.indexOf("<div class=\"tqtongji2\">");
        int end = html.indexOf("<div class=\"lishicity03\">");
        html = html.mid(begin,end-begin);
        html = html.left(html.indexOf("<div style=\"clear:both\">"));
    }
    else
        if(!display){               //匹配 AQI 网页的 html 文本
            int begin = html.indexOf("<table width=\"620px\" border=\"0\" class=\"b\" cellpadding=\"1\" cellspacing=\"1\">");
            int end = html.indexOf("<div id=\"chartdiv\" align=\"center\">");
            html = html.mid(begin,end-begin);
            html = html.left(html.indexOf("</div>"));
        }
    html = html.simplified().trimmed();
    if (! html.isEmpty()){
        qDebug()<<"开始解析html";
        parseHTML(html);  // 开始解析HTML
    }else{
        qDebug()<<"HTML内容为空："<<html;
        emit dataParseError(QString("HTML内容为空"));
    }
}

