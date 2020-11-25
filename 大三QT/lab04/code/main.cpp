#include "scorewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScoreWindow w;
    w.setWindowTitle("成绩管理");
    w.setMaximumSize(850,700);
    w.show();

    return a.exec();
}
