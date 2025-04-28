
#include "CompassWidgetPlugin.h"
#include "CompassWidget.h"

CompassWidgetPlugin::CompassWidgetPlugin(QObject *parent)
    : QObject(parent), initialized(false)
{
}

bool CompassWidgetPlugin::isContainer() const
{
    return false; // Этот виджет не является контейнером
}

bool CompassWidgetPlugin::isInitialized() const
{
    return initialized;
}

QIcon CompassWidgetPlugin::icon() const
{
    return QIcon(); // Можно добавить иконку для виджета
}

QString CompassWidgetPlugin::domXml() const
{
    return "<widget class=\"CompassWidget\" name=\"compassWidget\">\n</widget>\n";
}

QString CompassWidgetPlugin::group() const
{
    return "Custom Widgets"; // Группа, где будет отображаться виджет
}

QString CompassWidgetPlugin::includeFile() const
{
    return  QString("CompassWidget.h"); // Заголовочный файл для виджета
}

QString CompassWidgetPlugin::name() const
{
    return QString("CompassWidget"); // Имя виджета
}

QString CompassWidgetPlugin::toolTip() const
{
    return QString(); // Описание для всплывающей подсказки
}

QString CompassWidgetPlugin::whatsThis() const
{
    return QString(); // Подробное описание
}

QWidget *CompassWidgetPlugin::createWidget(QWidget *parent)
{
    return new CompassWidget(parent); // Создаем экземпляр виджета
}

void CompassWidgetPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if (initialized)
        return;

    initialized = true;
}