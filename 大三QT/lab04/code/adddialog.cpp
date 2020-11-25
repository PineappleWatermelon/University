#include "adddialog.h"
#include "ui_adddialog.h"
#include<QMessageBox>
#include<qDebug>
AddDialog::AddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog)
{
    ui->setupUi(this);
    /*增加学生记录*/  /*增加课程成绩*/    /*确认提交*/
    connect(ui->pushAdd,&QPushButton::clicked,this,&AddDialog::getnewInfo);
    connect(ui->gradeAdd,&QPushButton::clicked,this,&AddDialog::addgradeInfo);
    connect(ui->pushSure,&QPushButton::clicked,this,&AddDialog::inputFinish);
}

AddDialog::~AddDialog()
{
    delete ui;
}
/**
 * @brief addDialog::setRequestdb
 * 私有变量传递函数
 * @param getdb
 */
void AddDialog::setRequestdb(QSqlDatabase getdb)
{
    newdb=getdb ;
}

/**
 * @brief AddDialog::getnewInfo
 * 获取新增加的学生信息
 */

void AddDialog::getnewInfo()
{
    if (newdb.isOpen())
        newdb.close();

    if(!newdb.open())
    {
        qDebug() << "无法连接数据库" << newdb.lastError();
        return;
    }
    else
    {
        QSqlQuery sql_query;       //数据库操作类
        QString  select_sql = QString("select * from t_courses");
        if(!sql_query.exec(select_sql))
            qDebug()<<sql_query.lastError();
        else
        {
            while(sql_query.next())
//并且必须先导航到有效记录，然后才能检索数据值（例如，使用next（））。
            {
                Courses<<sql_query.value(1).toString();//value(i)返回当前记录中字段索引的值。
            }
        }
        /*得到学生信息*/
        QString str;
        QString data1=ui->lineEdit_Number->text();
        QString data2=ui->lineEdit_Name->text();
        if(!data1.isEmpty())
            str=QString("where sNumber ='%1'").arg(data1);
        else if(!data2.isEmpty())
            str=QString("where sName ='%1'").arg(data2);
        select_sql = QString("select * from t_stud_info %1").arg(str);

        if(!sql_query.exec(select_sql))
        {
            qDebug()<<sql_query.lastError();
        }
        else
        {
            while(sql_query.next())
            {
                add_stuNumber=sql_query.value(0).toString();
                add_stuName=sql_query.value(1).toString();
                add_stuProfession=sql_query.value(2).toString();
                add_stuGrade=sql_query.value(3).toString();
            }
            updatadialog();
        }
    }

}

/**
 * @brief AddDialog::updatadialog
 * 将学生信息在文本框中显示
 */
void AddDialog::updatadialog()
{
    ui->lineEdit_profess->setEnabled(true);
    ui->lineEdit_class->setEnabled(true);
    ui->lineEdit_grade->setEnabled(true);
    ui->textEdit_grade->setEnabled(true);
    ui->comboBox_course->addItems(Courses);
    ui->lineEdit_Name->setText(add_stuName);
    ui->lineEdit_Number->setText(add_stuNumber);
    ui->lineEdit_profess->setText(add_stuProfession);
    ui->lineEdit_class->setText(add_stuGrade);
}
/**
 * @brief AddDialog::AddgradeInfo
 * 添加成绩
 */
void AddDialog::addgradeInfo()
{
    ui->textEdit_grade->append(ui->comboBox_course->currentText());
    ui->textEdit_grade->append(ui->lineEdit_grade->text());
}
/**
 * @brief AddDialog::inputFinish
 * 确认提交后的处理函数
 */
void AddDialog::inputFinish()
{
    QString courseGrade=ui->textEdit_grade->toPlainText();//以纯文本形式返回文本编辑的文本。
//    qDebug()<<courseGrade;
    if(courseGrade.isEmpty())
    {
        QMessageBox::critical(this,tr("错误"),tr("请添加成绩！"));
        return;
    }
    QStringList data=courseGrade.split("\n");
//    qDebug()<<data;
    QSqlQuery sql_query;       //数据库操作类
    QString  select_sql;
    QString courseID;
    qDebug()<<data.size();
    if (newdb.open())
        newdb.close();

    if(!newdb.open())
    {
        qDebug() << "Error: Failed to connect database." << newdb.lastError();
        return;
    }
    else
    {
        for(int i=0;i<data.size();i=i+2)
        {
            select_sql = QString("select courseID from t_courses where courseName='%1';").arg(data.at(i));
            if(!sql_query.exec(select_sql))
            {
                qDebug()<<sql_query.lastError();
            }
            else
            {
                while(sql_query.next())
                    courseID=courseID=sql_query.value(0).toString();
            }

            select_sql = QString("insert into t_stud_course_info values('','%1','%2','%3','','');").arg(add_stuNumber,courseID,data.at(i+1));
            if(!sql_query.exec(select_sql))
                qDebug()<<sql_query.lastError();
            else
                qDebug()<<"插入成功";
        }
        QMessageBox::information(NULL, tr("Info"),tr("插入成功"));//打开包含给定标题和文本的信息消息框
        emit addFinish();    //发送提交完成信号，关闭dialog框
    }
}

