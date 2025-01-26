#include <QKeyEvent>
#include <QRegularExpressionValidator>
#include <cmath>
#include "CoordinateEdit.h"
#include "NaviStrings.h"
#include <QLineEdit>

static const char GEOGRAPHICAL_NORD_MASK[] = "99° 99.999' \\N;_";
static const char GEOGRAPHICAL_SOUTH_MASK[] =  "99° 99.999' S;_";
static const char GEOGRAPHICAL_EAST_MASK[] =  "999° 99.999' E;_";
static const char GEOGRAPHICAL_WEST_MASK[] =  "999° 99.999' W;_";

static const char DELTA_GEOGRAPHICAL_NORD_MASK[] =  "+99° 99.999';_";
static const char DELTA_GEOGRAPHICAL_SOUTH_MASK[] =  "-99° 99.999';_";
static const char DELTA_GEOGRAPHICAL_EAST_MASK[] =  "+999° 99.999';_";
static const char DELTA_GEOGRAPHICAL_WEST_MASK[] = "-999° 99.999';_";

static const char GEODESIC_NORD_MASK[] = "99° 99' 99.99\" \\N;_";
static const char GEODESIC_SOUTH_MASK[] = "99° 99' 99.99\" S;_";
static const char GEODESIC_WEST_MASK[] = "999° 99' 99.99\" W;_";
static const char GEODESIC_EAST_MASK[] = "999° 99' 99.99\" E;_";

static const char GEOGRAPHICAL_HEIGHT_POSITIVE_MASK[] =  "+99° 99.9';_";
static const char GEOGRAPHICAL_HEIGHT_NEGATIVE_MASK[] =  "-99° 99.9';_";
static const char GEOGRAPHICAL_AZIMUTH_MASK[] =  "999° 99.999';_";

AbstractCoordinateEdit::AbstractCoordinateEdit(double min, double max, QWidget* parent):
    QAbstractSpinBox(parent),
    CoordinateEdit(min, max)
{
    setContextMenuPolicy(Qt::NoContextMenu);
    setButtonSymbols(NoButtons);
    lineEdit()->setFocusPolicy(Qt::WheelFocus);
}

void AbstractCoordinateEdit::stepBy(int steps)
{
    switch(section(lineEdit()->cursorPosition())) {
        case Section::degree:
            addDegree(steps);
            break;
        case Section::minute:
            addMinute(steps);
            break;
        case Section::sub_minute:
            addMinute(0.001 * steps);
            break;
        case Section::second:
            addSecond(steps);
            break;
        case Section::sub_second:
            addSecond(0.001 * steps);
            break;
        case Section::hemisphere:
            setHemisphere(steps > 0 ? Hemisphere::positive : Hemisphere::negative);
            break;
        default:
            break;
    }
}

QAbstractSpinBox::StepEnabled AbstractCoordinateEdit::stepEnabled() const
{
    QAbstractSpinBox::StepEnabled result = QAbstractSpinBox::StepNone;
    if(value() < maximum()) {
        result |= QAbstractSpinBox::StepUpEnabled;
    }
    if(value() > minimum()) {
        result |= QAbstractSpinBox::StepDownEnabled;
    }
    return result;
}

void AbstractCoordinateEdit::keyPressEvent(QKeyEvent* event)
{
    switch(event->key()) {
        case Qt::Key_Plus:
        case Qt::Key_E:
        case Qt::Key_N:
            setHemisphere(Hemisphere::positive);
            return;
        case Qt::Key_Minus:
        case Qt::Key_W:
        case Qt::Key_S:
            setHemisphere(Hemisphere::negative);
            return;
    }

    if(event->key() >= Qt::Key_Home && event->key() <= Qt::Key_PageDown) {
        QAbstractSpinBox::keyPressEvent(event);
        return;
    }

    if(event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) {
        auto old = value();
        auto old_pos = lineEdit()->cursorPosition();

        QAbstractSpinBox::keyPressEvent(event);
        double value;
        if(parse(value)) {
            setValue(value);
        } else {
            setValue(old);
            lineEdit()->setCursorPosition(old_pos);
        }
    }
}

void AbstractCoordinateEdit::valueChanged()
{
    auto pos = lineEdit()->cursorPosition();
    lineEdit()->setText(assambly());
    lineEdit()->setCursorPosition(pos);
    emit valueChanged(value());
}

//------------------------------------------------------------------------------
LatitudeEdit::LatitudeEdit(QWidget* parent):
    AbstractCoordinateEdit(-90., 90., parent)
{
    setValue(0);
}

QString LatitudeEdit::assambly() const
{
    return geoLat(value());
}

void LatitudeEdit::hemisphereChanged(CoordinateEdit::Hemisphere hs)
{
    auto pos = lineEdit()->cursorPosition();
    switch(hs) {
        case Hemisphere::positive:
            lineEdit()->setInputMask(GEOGRAPHICAL_NORD_MASK);
            break;
        case Hemisphere::negative:
            lineEdit()->setInputMask(GEOGRAPHICAL_SOUTH_MASK);
            break;
        default:
            break;
    }
    lineEdit()->setCursorPosition(pos);
}

bool LatitudeEdit::parse(double& result) const
{
    auto str = lineEdit()->text();

    auto list = str.split(" ");
    if(list.size() < 3) {
        return false;
    }

    auto l = QLocale::c();

    bool ok;
    auto deg = l.toDouble(list[0].left(2), & ok);
    if(!ok) {
        return false;
    }
    auto min = l.toDouble(list[1].left(6), & ok);
    if(!ok || min >= 60.) {
        return false;
    }

    result = deg + min / 60.;
    if(list[2] == "S") {
        result = -result;
    }
    return true;
}

AbstractCoordinateEdit::Section LatitudeEdit::section(int pos) const
{
    if(pos < 2) {
        return Section::degree;
    }
    if(pos < 6) {
        return Section::minute;
    }
    if(pos < 10) {
        return Section::sub_minute;
    }
    return Section::hemisphere;
}

//------------------------------------------------------------------------------
LongitudeEdit::LongitudeEdit(QWidget* parent):
    AbstractCoordinateEdit(-180., 180., parent)
{
    setValue(0);
}

QString LongitudeEdit::assambly() const
{
    return geoLon(value());
}

void LongitudeEdit::hemisphereChanged(CoordinateEdit::Hemisphere hs)
{
    auto pos = lineEdit()->cursorPosition();
    switch(hs) {
        case Hemisphere::positive:
            lineEdit()->setInputMask(GEOGRAPHICAL_EAST_MASK);
            break;
        case Hemisphere::negative:
            lineEdit()->setInputMask(GEOGRAPHICAL_WEST_MASK);
            break;
        default:
            break;
    }
    lineEdit()->setCursorPosition(pos);

}

bool LongitudeEdit::parse(double& result) const
{
    auto str = lineEdit()->text();

    auto list = str.split(" ");
    if(list.size() < 3) {
        return false;
    }

    auto l = QLocale::c();

    bool ok;
    auto deg = l.toDouble(list[0].left(3), & ok);
    if(!ok) {
        return false;
    }
    auto min = l.toDouble(list[1].left(6), & ok);
    if(!ok || min >= 60.) {
        return false;
    }

    result = deg + min / 60.;
    if(list[2] == "W") {
        result = -result;
    }
    return true;
}

AbstractCoordinateEdit::Section LongitudeEdit::section(int pos) const
{
    if(pos < 3) {
        return Section::degree;
    }
    if(pos < 7) {
        return Section::minute;
    }
    if(pos < 11) {
        return Section::sub_minute;
    }
    return Section::hemisphere;
}

//------------------------------------------------------------------------------
DeltaLatitudeEdit::DeltaLatitudeEdit(QWidget* parent):
    AbstractCoordinateEdit(-90., 90., parent)
{
    setValue(0);
}

QString DeltaLatitudeEdit::assambly() const
{
    return geoDeltaLat(value());
}

void DeltaLatitudeEdit::hemisphereChanged(CoordinateEdit::Hemisphere hs)
{
    auto pos = lineEdit()->cursorPosition();
    switch(hs) {
        case Hemisphere::positive:
            lineEdit()->setInputMask(DELTA_GEOGRAPHICAL_NORD_MASK);
            break;
        case Hemisphere::negative:
            lineEdit()->setInputMask(DELTA_GEOGRAPHICAL_SOUTH_MASK);
            break;
        default:
            break;
    }
    lineEdit()->setCursorPosition(pos);
}

bool DeltaLatitudeEdit::parse(double& result) const
{
    auto str = lineEdit()->text();

    auto list = str.split(" ");
    if(list.size() < 2) {
        return false;
    }

    auto l = QLocale::c();

    bool ok;
    auto deg = l.toDouble(list[0].left(3), & ok);
    if(!ok) {
        return false;
    }
    auto min = l.toDouble(list[0][0] + list[1].left(6), & ok);
    if(!ok || min >= 60.) {
        return false;
    }
    result = deg + min / 60.;
    return true;
}

AbstractCoordinateEdit::Section DeltaLatitudeEdit::section(int pos) const
{
    if(pos < 1) {
        return Section::hemisphere;
    }
    if(pos < 3) {
        return Section::degree;
    }
    if(pos < 7) {
        return Section::minute;
    }
    return Section::sub_minute;
}

//------------------------------------------------------------------------------
DeltaLongitudeEdit::DeltaLongitudeEdit(QWidget* parent):
    AbstractCoordinateEdit(-180., 180., parent)
{
    setValue(0);
}

void DeltaLongitudeEdit::hemisphereChanged(CoordinateEdit::Hemisphere hs)
{
    auto pos = lineEdit()->cursorPosition();
    switch(hs) {
        case Hemisphere::positive:
            lineEdit()->setInputMask(DELTA_GEOGRAPHICAL_EAST_MASK);
            break;
        case Hemisphere::negative:
            lineEdit()->setInputMask(DELTA_GEOGRAPHICAL_WEST_MASK);
            break;
        default:
            break;
    }
    lineEdit()->setCursorPosition(pos);
}

bool DeltaLongitudeEdit::parse(double& result) const
{
    auto str = lineEdit()->text();

    auto list = str.split(" ");
    if(list.size() < 2) {
        return false;
    }

    auto l = QLocale::c();

    bool ok;
    auto deg = l.toDouble(list[0].left(4), & ok);
    if(!ok) {
        return false;
    }

    auto min = l.toDouble(list[0][0] + list[1].left(6), & ok);
    if(!ok || min >= 60.) {
        return false;
    }
    result = deg + min / 60.;
    return true;
}

AbstractCoordinateEdit::Section DeltaLongitudeEdit::section(int pos) const
{
    if(pos < 1) {
        return Section::hemisphere;
    }
    if(pos < 4) {
        return Section::degree;
    }
    if(pos < 8) {
        return Section::minute;
    }
    return Section::sub_minute;
}

QString DeltaLongitudeEdit::assambly() const
{
    return geoDeltaLon(value());
}

//------------------------------------------------------------------------------
AltitudeEdit::AltitudeEdit(QWidget *parent):
    AbstractCoordinateEdit(-90., 90., parent)
{
    setValue(0);
}

void AltitudeEdit::hemisphereChanged(Hemisphere hs)
{
    auto pos = lineEdit()->cursorPosition();
    switch(hs) {
        case Hemisphere::positive:
            lineEdit()->setInputMask(GEOGRAPHICAL_HEIGHT_POSITIVE_MASK);
            break;
        case Hemisphere::negative:
            lineEdit()->setInputMask(GEOGRAPHICAL_HEIGHT_NEGATIVE_MASK);
            break;
        default:
            break;
    }
    lineEdit()->setCursorPosition(pos);
}

bool AltitudeEdit::parse(double &result) const
{
    auto str = lineEdit()->text();

    auto list = str.split(" ");
    qDebug() << list;
    if(list.size() < 2) {
        return false;
    }

    auto l = QLocale::c();

    bool ok;
    auto deg = l.toDouble(list[0].left(3), &ok);
    if(!ok) {
        return false;
    }
    auto min = l.toDouble(list[0][0] + list[1].left(4), &ok);
    if(!ok || min >= 60.) {
        return false;
    }
    result = deg + min / 60.;
    return true;
}

AbstractCoordinateEdit::Section AltitudeEdit::section(int pos) const
{
    if(pos < 1) {
        return Section::hemisphere;
    }
    if(pos < 3) {
        return Section::degree;
    }
    if(pos < 7) {
        return Section::minute;
    }
    return Section::sub_minute;
}

QString AltitudeEdit::assambly() const
{
    return geoAltitude(value());
}

//------------------------------------------------------------------------------
AzimuthEdit::AzimuthEdit(QWidget *parent):
    AbstractCoordinateEdit(0., 360., parent)
{
    setValue(0);
}

void AzimuthEdit::hemisphereChanged(Hemisphere hs)
{
    auto pos = lineEdit()->cursorPosition();
    lineEdit()->setInputMask(GEOGRAPHICAL_AZIMUTH_MASK);
    lineEdit()->setCursorPosition(pos);
}

bool AzimuthEdit::parse(double &result) const
{
    auto str = lineEdit()->text();

    auto list = str.split(" ");
    if(list.size() < 2) {
        return false;
    }

    auto l = QLocale::c();

    bool ok;
    auto deg = l.toDouble(list[0].left(3), &ok);
    if(!ok) {
        return false;
    }

    auto min = l.toDouble(list[1].left(6), &ok);
    if(!ok || min >= 60.) {
        return false;
    }
    result = deg + min / 60.;
    return true;
}

AbstractCoordinateEdit::Section AzimuthEdit::section(int pos) const
{
    if(pos < 3) {
        return Section::degree;
    }
    if(pos < 7) {
        return Section::minute;
    }
    return Section::sub_minute;
}

QString AzimuthEdit::assambly() const
{
    return geoAzimuth(value());
}