#include <QCoreApplication>
#include <QDebug>
#include <QVector>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    QList<QString> name,student_ID,lesson1,lesson2,headline;

    QVector<QString> name,student_ID,lesson1,lesson2;
    QVector<QString> temp,headline;
    QVector<QVector<QString>> total;

    headline<<"按姓名排序后名单"<<"按课程1排序后名单"<<"按课程2排序后名单";
    student_ID<< "1403130209" << "1403140101" << "1403140102" << "1403140103" ;  //学号列表
    name<<"鲁智深"<<"林冲"<<"宋江"<<"武松";              //名字
    lesson1<<"80"<<"82"<<"76"<<"88";                   //课程1成绩
    lesson2<<"72"<<"76"<<"85"<<"80";                   //课程2成绩
    total<<student_ID<<name<<lesson1<<lesson2;

    //输出原始成绩单
    int i;
    qDebug()<<"原始成绩单";
    qDebug()<<"学号"<<"\t\t"<<"姓名"<<"\t\t"<<"课程1"<<'\t'<<"课程2";
    for(i=0;i<student_ID.size();i++)
    {
        qDebug()<<student_ID.at(i)<<"\t"
                <<name.at(i)<<"\t"
                <<lesson1.at(i)<<"\t"
                <<lesson2.at(i)<<"\t";
    }

    //输出三种排序方式的列表
    int k;
    for (i=0;i<headline.size();i++)
    {
        qDebug()<<headline.at(i);
        qDebug()<<"学号"<<"\t\t"<<"姓名"<<"\t\t"<<"课程1"<<'\t'<<"课程2";
        temp=total.at(i+1);
        std::sort(temp.begin(),temp.end(),std::greater<QString>());     //根据不同要求排序

        for(int j=0;j<temp.size();j++)
        {
           for (k=0;k<temp.size();k++)
               if(temp.at(j)==total.at(i+1).at(k))
                   break;                               //排序后的值与原始对应值匹配
                   qDebug()<<total.at(0).at(k)<<'\t'
                           <<total.at(1).at(k)<<'\t'
                           <<total.at(2).at(k)<<'\t'
                           <<total.at(3).at(k)<<'\t';
        }
    }

    return a.exec();
}
