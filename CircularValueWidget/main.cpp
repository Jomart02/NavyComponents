#include <QApplication>
#include <QWidget>
#include "CircularValueWidget.h"
#include "CompassWidget.h"
#include <QObject>
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    CompassWidget *w = new CompassWidget();
    w->show();
    return app.exec();

}