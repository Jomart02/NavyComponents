
#include "SpeedometerWidgetPlugin.h"
#include "SpeedometerWidget.h"

SpeedometerWidgetPlugin::SpeedometerWidgetPlugin(QObject *parent)
    : QObject(parent), initialized(false)
{
}

bool SpeedometerWidgetPlugin::isContainer() const
{
    return false; // Этот виджет не является контейнером
}

bool SpeedometerWidgetPlugin::isInitialized() const
{
    return initialized;
}

QIcon SpeedometerWidgetPlugin::icon() const
{
    return QIcon(); // Можно добавить иконку для виджета
}

QString SpeedometerWidgetPlugin::domXml() const
{
    return "<widget class=\"SpeedometerWidget\" name=\"speedometerWidget\">\n</widget>\n";
}

QString SpeedometerWidgetPlugin::group() const
{
    return "Custom Widgets"; // Группа, где будет отображаться виджет
}

QString SpeedometerWidgetPlugin::includeFile() const
{
    return  QString("SpeedometerWidget.h"); // Заголовочный файл для виджета
}

QString SpeedometerWidgetPlugin::name() const
{
    return QString("SpeedometerWidget"); // Имя виджета
}

QString SpeedometerWidgetPlugin::toolTip() const
{
    return QString(); // Описание для всплывающей подсказки
}

QString SpeedometerWidgetPlugin::whatsThis() const
{
    return QString(); // Подробное описание
}

QWidget *SpeedometerWidgetPlugin::createWidget(QWidget *parent)
{
    return new SpeedometerWidget(parent); // Создаем экземпляр виджета
}

void SpeedometerWidgetPlugin::initialize(QDesignerFormEditorInterface *core)
{
    if (initialized)
        return;

    initialized = true;
}