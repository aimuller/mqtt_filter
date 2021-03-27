#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("MQTT过滤控制系统");
    w.move(QApplication::desktop()->screenGeometry().center()- w.rect().center());
    w.resize(1000, 600);
    w.show();
    
    return a.exec();
}
