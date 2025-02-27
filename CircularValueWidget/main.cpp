#include <QApplication>
#include <QWidget>
#include "CircularValueWidget.h"
#include "SpeedometerWidget.h"
#include <QObject>
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    SpeedometerWidget *w = new SpeedometerWidget();
    w->show();
    return app.exec();

}