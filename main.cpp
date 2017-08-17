#include "mainwindow.h"
#include "Unity.h"

#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    // Uncomment to see Modbus Data Logs
    //QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = true"));

    QApplication a(argc, argv);

    Vreo::Init();
    MainWindow w;
    w.show();

    return a.exec();
}
