#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDialog(QWidget *parent = 0);
    ~AddDialog();
     void setRequestdb(QSqlDatabase);
     void updatadialog();
     void addgradeInfo();
     void inputFinish();
private:
    Ui::AddDialog *ui;
    void getnewInfo();
    QSqlDatabase newdb;
    QString  add_stuNumber;
    QString  add_stuName;
    QString  add_stuProfession;
    QString  add_stuGrade;
    QStringList Courses;
signals:
    void addFinish();
};

#endif // ADDDIALOG_H
