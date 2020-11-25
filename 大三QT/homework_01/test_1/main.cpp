#include <QCoreApplication>
#include <QDebug>
#include <QList>

#define MAX(x,y) ((x)>(y) ? (x):(y))    //两个数中最大值
#define MIN(x,y) ((x)<(y) ? (x):(y))    //两个数中最小值
#define Lex_16(x)   ((x) & 65535)   //32位数中低16位
#define Hex_16(x)   (((x) >> 16))   //32位数中高16位
#define Lex_8(x)    (((x) & 255))   //16位数中低8位
#define Hex_8(x)    (((x) >> 8))    //16位数中高8位

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    quint32  Hex32,Hex32_new;   //已知32位数，重组后32位数
    quint16  H16,L16;           //32位数拆分为的高16位和低16位
    quint16  MAX1,MIN1;         //高16位数的较大值，低16位数较小值
    quint8   HHI,HLO,LHI,LLO;   //32位数拆分为高八位，次高八位，次低八位，低八位变量
    QList<quint8> values;
    Hex32=0x45678932;

    //32位数拆分
    H16=Hex_16(Hex32);  //32位数中高16位
    L16=Lex_16(Hex32);  //32位数中低16位
    HHI=Hex_8(H16);     //高16位中高8位
    HLO=Lex_8(H16);     //高16位中次高8位
    LHI=Hex_8(L16);     //低16位中次低8位
    LLO=Lex_8(L16);     //低16位中低8位
    values<<HHI<<HLO<<LHI<<LLO;     //32位数每8位存到values

    //数值比较
    MAX1=MAX(HHI,HLO);  //比较高16中位高八位和次高8位中最大值
    MIN1=MIN(LHI,LLO);  //比较低16位中低八位和次低8位中最小值

    //数值重新拼接
    Hex32_new=(((quint32)(HLO))<<24)+(((quint32)(HHI))<<16)+(((quint32)(LLO))<<8)+LHI;


    //10进制转换16进制
    QString str0 = QString("0x%1").arg(Hex32,8,16);     //Hex32转换为16进制
    QString str1 = QString("0x%1").arg(MAX1,2,16);     //MAX1转换为16进制
    QString str2 = QString("0x%1").arg(MIN1,2,16);    //MIN1转换为16进制
    QString str3 = QString("0x%1").arg(Hex32_new,8,16);     //Hex32_new转换为16进制

    qDebug()<<"原始值"<< str0 << Hex32;
    qDebug()<<"最高八位和次高八位最大值"<< str1 << MAX1;     //分别输出MAX1的16进制和10进制所表示数
    qDebug()<<"最低八位和次低八位最小值"<< str2<< MIN1;     //分别输出MIN1的16进制和10进制所表示数
    qDebug()<<"重新组合的32位数"<< str3 << Hex32_new;      //分别输出重新组合后32位数的16进制和10进制所表示数
    qDebug()<<"排序前"<<values;
    std::sort(values.begin(),values.end(),std::greater<quint8>());  //values列表里数值从大到小排序
    qDebug()<<"排序后"<<values;                           //输出提取出的八位数排序

    return a.exec();
}
