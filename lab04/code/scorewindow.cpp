#include "scorewindow.h"
#include "ui_scorewindow.h"
#include <QDebug>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

ScoreWindow::ScoreWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScoreWindow)
{
    ui->setupUi(this);
    database=QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName("localhost");
    database.setPort(3306);
    database.setUserName("root");
    database.setPassword("");
    database.setDatabaseName("test");
    connect(ui->pushButton,&QPushButton::clicked,this, &ScoreWindow::btn_query);
    connect(ui->btn_add,QAction::triggered,this,&ScoreWindow::btn_add_clicked);
    connect(ui->btn_import,QAction::triggered,this,&ScoreWindow::btn_import_clicked);
    connect(ui->btn_outport1,&QAction::triggered,this,&ScoreWindow::btn_import_t_courses);

}
ScoreWindow::~ScoreWindow()
{
    database.close();
    delete ui;
}

void ScoreWindow::btn_query()
{
//    重启数据库
    if(database.isOpen())
        database.close();

    if(!database.open()){
        qDebug() << "错误，无法连接到数据库！\n请检查数据库连接配置\n" << database.lastError();
        return;
    }
    else
    {
        QString str;
        if(ui->searchNumber->isChecked())
            str=QString("where sNumber ='%1'").arg(ui->searchEdit->text());
        else if(ui->searchName->isChecked())
            str=QString("where sName ='%1'").arg(ui->searchEdit->text());

        QSqlQuery queryStr;            //数据库操作类，QSqlQuery类提供了一种执行和操作SQL语句的方法。
        QString selected_Str;

        selected_Str= QString("select * from t_stud_info %1").arg(str);
        if(!queryStr.exec(selected_Str))
            qDebug()<<queryStr.lastError();
        else
        {
            while(queryStr.next())
            {                                                     //四个详细信息，用于界面显示
                stu_number=queryStr.value(0).toString();              //学号
                stu_name=queryStr.value(1).toString();                //姓名
                stu_profession=queryStr.value(2).toString();          //专业
                stu_grade=queryStr.value(3).toString();               //年级
            }
        }
        selected_Str = QString("select t_stud_info.sName as '姓名',t_courses.courseName as '课程名称',"
                                     "t_stud_course_info.scScores as '成绩',t_courses.courseCredit as '学分' "
                                     "from "
                                     "(t_stud_course_info inner join t_courses on t_stud_course_info.scCourseID = t_courses.courseID) "
                                     "inner join t_stud_info on t_stud_info.sNumber = t_stud_course_info.scNumber "
                                     "%1 "
                                     "order by t_stud_course_info.scNumber "
                                     ";").arg(str);       
        if(!queryStr.exec(selected_Str))
             qDebug()<<queryStr.lastError(); //返回有关此查询发生的最后一个错误（如果有）的错误信息
        else{
            qDebug()<<queryStr.next();       //检索结果中的下一条记录（如果可用），并将查询定位在检索到的记录上
            while(queryStr.next())
            { //四个容器列表
                sName<<queryStr.value(0).toString();                //姓名
                course_Name<<queryStr.value(1).toString();          //课程名称
                scScores<<queryStr.value(2).toString();             //成绩
                courseCredit<< queryStr.value(3).toString();        //学分
            }
        }      
    }
    updataWindow();
}

/*界面更新函数*/
//cellpadding规定单元边沿与其内容之间的空白。
//align规定表格相对周围元素的对齐方式。
//border规定表格边框的宽度。
//colspan合并单元格
void ScoreWindow::updataWindow()
{
    QString temp;
    temp.append("<table border='1' cellpandding='10' align='center' > <tr>  <td colspan='4' align='center' bgcolor='#9DCFEF' >");
    if(!sName.size()) return;
    temp.append(sName.at(0));
    temp.append("</td> </tr>");
    temp.append("<tr> <td width='80' >序号</td>  <td>课程名称</td>  <td>成绩</td>  <td>学分</td>  </tr>");
    for(int i=0;i<sName.size();i++)
    {
        temp.append("<tr>  <td>");
        temp.append(QString::number(i));
        temp.append("<td>  </td>");
        temp.append(course_Name.at(i));
        temp.append("<td>  </td>");
        temp.append(scScores.at(i));
        temp.append("<td>  </td>");
        temp.append(courseCredit.at(i));
        temp.append("</td>  </tr>");
    }
    temp.append("<tr bgcolor='#9DCEEF'>  <td colspan='4' align='center' >");
    temp.append("平均绩点：");
    QString avePoint=GPA(scScores,courseCredit);         //计算平均绩点
    temp.append(avePoint);
    temp.append("</td>  </tr> </table>");
    ui->textEdit->clear();
    ui->textEdit->append(temp);
    ui->edit_Number->setText(stu_number);
    ui->edit_Name->setText(stu_name);
    ui->edit_Profession->setText(stu_profession);
    ui->edit_Class->setText(stu_grade);
    sName.clear();
    course_Name.clear();
    scScores.clear();
    courseCredit.clear();
}

/*计算平均绩点*/
QString ScoreWindow::GPA(QStringList score,QStringList credit)
{
   QStringList leavelList;
   leavelList<<"优秀"<<"良好"<<"中等"<<"及格"<<"不及格"<<"缓考"<<"旷考";
   double stdPoint=0,sum=0;
   for(int i=0;i<score.size();i++)
   {
       QString toscore=score.at(i);
       QString stdcredit=credit.at(i);
       if (leavelList.contains(score.at(i)))
//如果列表包含字符串score.at(i)，则返回true;否则返回false
//indexOf返回列表中score.at(i)的第一个完全匹配的索引位置，从索引位置向前搜索。如果没有匹配项，则返回-1。
       {
           switch (leavelList.indexOf(score.at(i)))
           {
           case 0:                     // 优秀
               toscore= "95";
               break;
           case 1:                     // 良好
               toscore="85";
               break;
           case 2:                     // 中等
               toscore="75";
               break;
           case 3:                     // 及格
               toscore="65";
               break;
           case 4:                     // 不及格
               toscore="50";
               break;
           case 5:                     // 缓考
               toscore="0";
               break;
           case 6:                     // 旷考
               toscore="0";
               break;
           default:
               break;
           }
       }
       if(!toscore.toDouble())  //返回转换为double值的字符串。如果转换失败，则返回0.0。
           stdPoint+=0;
       else
           stdPoint+=((toscore.toDouble()/10-5)*stdcredit.toDouble());
       sum+=stdcredit.toDouble();
       qDebug()<<stdPoint<<sum;
       double avePoint=stdPoint/sum;
       QString s = QString::number(avePoint);//根据指定的数返回等于数字avePoint的字符串。
       return s;
   }
}




void ScoreWindow::btn_add_clicked()
{
    myaddDialog=new AddDialog(this);
    connect(myaddDialog,&AddDialog::addFinish,this,&ScoreWindow::close_addDialog); /*批量导入结束响应槽函数*/
    myaddDialog->show();
    myaddDialog->setRequestdb(database);
}

void ScoreWindow::close_addDialog()
{
    myaddDialog->close();
}


/**
 * @brief MainWindow::on_import_clicked
 * 批量导入槽函数，选择文件
 */
void ScoreWindow::btn_import_clicked()
{
        /*系统调用文件选择框*/
    QString path = QFileDialog::getOpenFileName(this, tr("批量导入"), ".", tr("csv Files(*.csv)"));
    if(path.length() == 0) {
        QMessageBox::information(NULL, tr("Info"), tr("You didn't select any files."));
        return;
    } else {
        QMessageBox::information(NULL, tr("Info"), tr("You selected ") + path);
    }

    /*读取文件*/
    QStringList  tempText;
    QFile file(path);
    qDebug()<<path;
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(NULL,tr("错误"),tr("打开文件失败！"));
        return;
    }
    QTextStream out(&file);
    while(!out.atEnd()){
        tempText.append(out.readLine().trimmed());
    }

    file.close();

    /*数据库操作*/
    if(tempText.isEmpty())
        return;
    if(database.isOpen())
    {
        database.close(); //重启db
    }
    if(!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
        return;
    }
    else{
        QString tempdata;
        QStringList singledata;
        QString  select_sql1;
        QString select_sql2;
        QStringList signalcourseID;
        QSqlQuery sql_query;       //数据库操作类

        tempdata=tempText.at(1);
        singledata=tempdata.split(',');
        qDebug()<<singledata;

        for(int i=0;i<4;i++){
            select_sql1 = QString("select courseID from t_courses where courseName='%1';").arg(singledata.at(5+i));


            if(!sql_query.exec(select_sql1))
            {
                qDebug()<<sql_query.lastError();
            }
            else
            {
                while(sql_query.next())
                {
                    signalcourseID<<sql_query.value(0).toString();

                }

            }
        }
        qDebug()<<signalcourseID;

        for(int i=1;i<tempText.size();i++)
        {
            tempdata=tempText.at(i);
            singledata=tempdata.split(',');
            qDebug()<<singledata;
            select_sql2 = QString("insert into t_stud_info value ('%1','%2','%3','%4','','');").arg(singledata.at(1),singledata.at(2),singledata.at(3),singledata.at(4));
            sql_query.exec(select_sql2);
            for(int j=0;j<4;j++){

                select_sql1 = QString("insert into t_stud_course_info value ('','%1','%2','%3','','');").arg(singledata.at(1),signalcourseID.at(j),singledata.at(5+j));
                sql_query.exec(select_sql1);
            }
        }
        QMessageBox::information(NULL, tr("Info"),tr("导入成功"));
   }

}


void ScoreWindow::outport_txt()
{
    QSqlQuery query1;

    database.open();
    query1.exec("select * from t_courses into outfile 'D:/Qt/Program/lab04/t_course.csv'");
    query1.exec("select * from t_stud_info into outfile 'D:/Qt/Program/lab04/qtMysql/t_course.csv'");
    query1.exec("select * from t_stud_course_info into outfile 'D:/Qt/Program/lab04/qtMysql/t_course.csv'");
    database.close();
//    query1.exec("select * from t_courses");
//    QStringList sa;
//    while(query1.next())
//        sa<<query1.value(0).toString()<<query1.value(1).toString()<<query1.value(2).toString()<<query1.value(3).toString();
//    qDebug()<<sa;
//    QSqlDatabase db_sqlite = QSqlDatabase::addDatabase("QSQLITE"); //添加数据库
//    db_sqlite.setDatabaseName("note.db"); //创建一个note.db的文件存储数据
//    db_sqlite.open(); //开启数据库
//    query = QSqlQuery(db_sqlite); //获得访问数据库的query
//    query.exec(XXX); //执行指令（XXX为SQL指令）
//    db_sqlite.close(); //关闭数据库
}




// mysql 数据库导出为 sqlite3 数据库
void ScoreWindow::btn_import_t_courses(){
    outport_txt();
    QStringList list1,list2,list3;
    list1.clear();
    list2.clear();
    list3.clear();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("mydb.db");
    QSqlQuery query;  //默认打开

    QFile file("D:/Qt/Program/lab04/t_course.csv"); //打开csv文件
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
         qDebug() << "Cannot open file for reading";
      //将csv_utf8转为数据库中的表District保存
//    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName("mydb.db");
    if(!db.open())
    {
        QMessageBox::critical(0 , "Can not open database",
                              "Unable to establish a database connection.",QMessageBox::Cancel);
        qDebug()<<"stop!";
    }
    query.exec("drop table if exists t_courses;");
    query.exec("create table t_courses (courseID varchar(25), course varchar(25),"
               "courseCredit varchar(25),courseType varchar(25))");
   QTextStream in(&file);  //QTextStream读取数据
   in.setCodec("UTF-8");    //使用UTF-8编码读取文件
   while(!in.atEnd())
   {
      QString fileLine = in.readLine();  //从第一行读取至下一行
      qDebug()<<fileLine;
      list1 = fileLine.split("\t", QString::SkipEmptyParts);
      qDebug()<<list1.at(0)<<list1.at(1)<<list1.at(2)<<list1.at(3);
      QString WYB=QString("insert into t_courses values('%1','%2','%3','%4');").arg(list1.at(0),list1.at(1),list1.at(2),list1.at(3));
      query.exec(WYB);
   }
   file.close();
}



//   QFile file1("D:/wyb1.csv"); //打开csv文件
//   if(!file1.open(QIODevice::ReadOnly | QIODevice::Text))
//        qDebug() << "Cannot open file1 for reading";

//   if(!db.open())
//   {
//       QMessageBox::critical(0 , "Can not open database",
//                             "Unable to establish a database connection.",QMessageBox::Cancel);
//       qDebug()<<"stop!";
//   }
//   query.exec("drop table if exists t_stud_info;");
//   query.exec("create table t_stud_info (courseID varchar(25), sName varchar(25),"
//              "sMajor varchar(25),sClass varchar(25)),sEnrolledYear varchar(25),sPhone varchar(25)");
//  QTextStream in1(&file1);  //QTextStream读取数据
//  in1.setCodec("UTF-8");    //使用UTF-8编码读取文件
//  while(!in1.atEnd())
//  {
//     QString fileLine = in1.readLine();  //从第一行读取至下一行
//     qDebug()<<fileLine;
//     list3 = fileLine.split("\t");
//     qDebug()<<list3.at(0)<<list3.at(1)<<list3.at(2)<<list3.at(3);
//     QString WYB=QString("insert into t_stud_info values('%1','%2','%3','%4','','');").arg(list3.at(0),list3.at(1),list3.at(2),list3.at(3));
//     query.exec(WYB);
//  }
//  file1.close();


//   query.clear();
//   db.close();
//}

//void ScoreWindow::btn_outport_clicked()
//{
//    QSqlQuery query2;
//    db.open();
//    query2.exec("create database new.db;");
//    QSqlDatabase db_sqlite = QSqlDatabase::addDatabase("QSQLITE","new.db");
//           db_sqlite.setDatabaseName("new.db");
//           if (!db_sqlite.open()) {
//                qDebug()<<"open database error!";
//           }else{
//               QSqlQuery query(db_sqlite);
//               query.exec("drop table if exists test0;");
//               QString attach = "attach database 'test0' as test";
//               query.exec(attach);
//               query.exec("create table test0 as select * from test.t_courses");
//               qDebug()<<0;
//               }
//}
