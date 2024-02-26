#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.setWindowTitle("Simple_ToDo");
    w.show();
    return app.exec();
}
