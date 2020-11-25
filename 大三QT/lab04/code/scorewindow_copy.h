#ifndef SCOREWINDOW_H
#define SCOREWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include "adddialog.h"
namespace Ui {
class ScoreWindow;
}

class ScoreWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScoreWindow(QWidget *parent = 0);
    ~ScoreWindow();
    void btn_query();           //搜素函数
    void updataWindow();
    QString GPA(QStringList score,QStringList credit);
    void btn_import_clicked();
    void btn_add_clicked();
    void close_addDialog();
    void btn_import_t_courses();

    void outport_to_txt();
private:
    Ui::ScoreWindow *ui;

    QSqlDatabase db;
    QString number,name,profession,grade; //界面显示
    QStringList sName,course_Name,scScores,courseCredit;
    QString path;
    AddDialog *myaddDialog;

};

#endif // SCOREWINDOW_H
