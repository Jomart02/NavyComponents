#include "SpeedometerWidget.h"
#include <QPainter>
#include <QFontDatabase>
#include <QLocale>
#include <QPainterPath>
#include <QColor>
SpeedometerWidget::SpeedometerWidget(QWidget* parent) : CircularValueWidget(parent){
    Q_INIT_RESOURCE(resCircularWidgets);
    int fontId = QFontDatabase::addApplicationFont(":/lcd"); 
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    fontValue =  QFont(fontFamily, 48); 

}

SpeedometerWidget::~SpeedometerWidget(){

} 
QColor SpeedometerWidget::getBackground(){
    return m_background;
}
void SpeedometerWidget::setBackground(QColor color){
    m_background = color;
    update();
}
QColor SpeedometerWidget::getTextNameColor(){
    return m_textNameColor;
}
void SpeedometerWidget::setTextNameColor(QColor color){
    m_textNameColor = color;
    update();
}
QColor SpeedometerWidget::getTextValueColor(){
     return m_textValueColor;
}
void SpeedometerWidget::setTextValueColor(QColor color){
    m_textValueColor = color;
    update();
}
QColor SpeedometerWidget::getUintsColor(){
    return m_textUintsColor;
}
void SpeedometerWidget::setUintsColor(QColor color){
    m_textUintsColor = color;
    update();
}
QColor SpeedometerWidget::getMarksColor(){
    return m_marksColor;
}
void SpeedometerWidget::setMarksColor(QColor color){
    m_marksColor = color;
    update();
}


//========================================================================================================\\

void SpeedometerWidget::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);
    // Устанавливаем фиксированный размер виджета как квадрат
    int side = qMin(width(), height());
    resize(side, side);
}


void SpeedometerWidget::calcValuePos(QPoint pos, bool move ){
    QPoint center(width() / 2, height() / 2); // Центр виджета
    int x_local = pos.x() - center.x(); // Перемещаем начало координат в центр
    int y_local = center.y() - pos.y(); 

    // Вычисляем расстояние до центра
    double dist = std::sqrt(x_local * x_local + y_local * y_local);

    // Вычисляем угол
    double angle_radians = std::atan2(y_local, x_local);
    double angle_degrees = fmod((90 - angle_radians * (180 / M_PI)), 360.0); // Используем fmod
    if (angle_degrees < 0) {
        angle_degrees += 360.0; // Коррекция для диапазона [0, 360]
    }
    int side = qMin(width(), height());
    bool is_valueEdit = (dist > side/ 2.4 && dist < side/ 2.1);
    if(move && isIntersectValueZone) is_valueEdit = true;
    if((angle_degrees >= 220.0 && angle_degrees <= 360.0) && is_valueEdit){
        double value = m_minValue + (angle_degrees - 220) * ((m_maxValue - m_minValue) / 280.0);
        setValue(value);
        isIntersectValueZone = true;
    }else if((angle_degrees >= 0 && angle_degrees <= 140.0) && is_valueEdit){
        angle_degrees += 360.0;
        double value = m_minValue + (angle_degrees - 220) * ((m_maxValue - m_minValue) / 280.0);
        setValue(value);
        isIntersectValueZone = true;
    }
}

void SpeedometerWidget::paintEvent(QPaintEvent *event)  {
   
    Q_UNUSED(event);

    QPainter painter(this);
    preparePainter(painter);

    int side = qMin(width(), height());

    drawBackground(painter, side);
    drawValueIndicator(painter, side);
    drawScaleMarks(painter, side);
    drawCurrentValue(painter, side);
    drawTextLabels(painter, side);
    drawPointer(painter, side);

}

void SpeedometerWidget::drawBackground(QPainter &painter, int side) {
    qreal radius1 = side / 2;
    QPointF point220 = getPointOnCircle(radius1, 220);
    QPointF point140 = getPointOnCircle(radius1, 140);

    QPainterPath path;
    qreal angleStart = 310.0;
    qreal angleSpan = 280.0;

    path.arcTo(-radius1, -radius1, side, side, angleStart, angleSpan);
    path.lineTo(point140);

    painter.setBrush(m_background);
    painter.setPen(Qt::transparent);
    painter.drawPath(path);

    qreal scale_factor = 0.4;
    qreal lift_amount = -(side / 10);
    QTransform transform;
    transform.scale(scale_factor, scale_factor);
    transform.translate(0, lift_amount);
    QPainterPath scaledPath = transform.map(path);

    QRadialGradient gradientBack(QPointF(0, 0), side / 2, QPointF(0, 0));
    QColor colorSenter("#d9c1c3");
    for (qreal pos = 0.0; pos <= 1.0; pos += 0.1) {
        qreal k = 4.5;
        qreal alpha = qExp(-k * pos);
        colorSenter.setAlphaF(alpha);
        gradientBack.setColorAt(pos, colorSenter);
    }

    painter.setBrush(QBrush(gradientBack));
    painter.setPen(Qt::transparent);
    painter.drawPath(scaledPath);
}

void SpeedometerWidget::drawValueIndicator(QPainter &painter, int side) {
    qreal angleValue = 220 + (m_value - m_minValue) / (m_maxValue - m_minValue) * 280;
    if (m_value > 0) {
        QLinearGradient gradient(-side / 2.1, -side / 2.1, side / 2.1, side / 2.1);
        gradient.setColorAt(0.0, Qt::green);
        gradient.setColorAt(1.0, Qt::red);

        painter.setPen(QPen(QBrush(gradient), (side / 2.1) - (side / 2.25), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(Qt::transparent);

        int startAngle = 220;
        int endAngle = angleValue;
        int stepAngle = 1;

        QPainterPath pathLine;
        pathLine.moveTo(getPointOnCircle(side / 2.1, startAngle));
        for (int angle = startAngle; angle <= endAngle; ++angle) {
            pathLine.lineTo(getPointOnCircle(side / 2.1, angle));
        }
        painter.drawPath(pathLine);
    }
}

void SpeedometerWidget::drawScaleMarks(QPainter &painter, int side) {
    const int numTicks = 14;
    const double angleStep = 280.0 / numTicks;

    QFont fontTics;
    fontTics.setPointSize(side / 45);
    fontTics.setBold(true);
    painter.setFont(fontTics);

    for (int i = 0; i <= numTicks; ++i) {
        double angle = 220 + i * angleStep;
        QPointF startPoint = getPointOnCircle(side / 2.1, angle);
        QPointF endPoint = getPointOnCircle(side / 2.4, angle);
        painter.setPen(QPen(m_marksColor, 4));
        painter.drawLine(startPoint, endPoint);

        if (i % 1 == 0) {
            double value = m_minValue + (m_maxValue - m_minValue) * i / numTicks;
            QString text = QLocale().toString(value, 'f', 1);
            QPointF textPoint = getPointOnCircle(side / 3.0, angle);

            QFontMetrics fm = painter.fontMetrics();
            int textWidth = fm.horizontalAdvance(text);
            int x, y;
            if (i == 0) {
                x = endPoint.x() + textWidth / 3;
                y = endPoint.y();
            } else if (i == numTicks) {
                x = endPoint.x() - textWidth * 1.2;
                y = endPoint.y();
            } else if (i >= (numTicks / 2) - 1 && i < numTicks / 2) {
                x = endPoint.x() - textWidth / 3;
                y = endPoint.y() + fm.height();
            } else if (i == numTicks / 2) {
                x = endPoint.x() - textWidth / 2;
                y = endPoint.y() + fm.height();
            } else if (i > numTicks / 2 && i <= (numTicks / 2) + 1) {
                x = endPoint.x() - textWidth / 2;
                y = endPoint.y() + fm.height();
            } else {
                if (angle <= 360) {
                    x = endPoint.x() + textWidth / 10;
                    y = endPoint.y() + fm.height() / 3;
                } else {
                    x = endPoint.x() - textWidth * 1.1;
                    y = endPoint.y() + fm.height() / 3;
                }
            }

            QPointF adjustedTextPoint(x, y);
            painter.drawText(adjustedTextPoint, text);
        }

        painter.setPen(QPen(m_marksColor, 2));
        if (i < numTicks) {
            double angle = 220 + i * angleStep + angleStep * 0.5;
            QPointF startPoint = getPointOnCircle(side / 2.2, angle);
            QPointF endPoint = getPointOnCircle(side / 2.4, angle);
            painter.drawLine(startPoint, endPoint);

            painter.setPen(QPen(m_marksColor, 1));
            fontTics.setBold(false);
            for (int j = 0; j <= 10; ++j) {
                double angle = 220 + i * angleStep + j * angleStep * 0.1;
                QPointF startPoint = getPointOnCircle(side / 2.2, angle);
                QPointF endPoint = getPointOnCircle(side / 2.4, angle);
                painter.drawLine(startPoint, endPoint);
            }
        }
    }
 }

 void SpeedometerWidget::drawCurrentValue(QPainter &painter, int side) {

    fontValue.setPointSize(side / 9);
    QString text = QLocale().toString(m_value, 'f', m_decimals);
    painter.setFont(fontValue);

    QFontMetrics fm(fontValue);
    int textWidth = fm.horizontalAdvance(text);
    int textHeight = fm.ascent();
    int textHeightDelta = -(textHeight - textHeight / 5);
    int x = 0 - textWidth / 2;
    int y = side / 4;

    if (m_editMode)
        painter.setPen(Qt::yellow);
    else
        painter.setPen(m_textValueColor);

    painter.drawText(x, y, text);

    QRect originalRect(x, y, textWidth, textHeightDelta);
    QTransform transform = painter.transform();
    QPolygonF transformedRect = transform.mapToPolygon(originalRect);
    QRectF boundingRect = transformedRect.boundingRect();
    valueFrame = boundingRect.toAlignedRect();

    if (m_editMode) {
        QString leftPart = text.left(m_cursorPosition);
        int cursorX = fm.horizontalAdvance(leftPart) - textWidth / 2;
        int cursorY = originalRect.y();
        painter.setPen(QPen(Qt::red, 5));
        painter.drawLine(cursorX, cursorY, cursorX, cursorY + textHeightDelta);
    }
}

 void SpeedometerWidget::drawTextLabels(QPainter &painter, int side) {
    QFont names("Helvetica", side / 25);
    names.setBold(true);
    painter.setFont(names);
    painter.setPen(QPen(m_textUintsColor));
    QFontMetrics fontunits(painter.font());
    int textWidthUnits = fontunits.horizontalAdvance(m_nameUints);
    int xUnits = 0 - textWidthUnits / 2;
    int yUnits = side / 3;
    painter.drawText(xUnits, yUnits, m_nameUints);
    
    painter.setPen(QPen(m_textNameColor));
    int textWidthName = fontunits.horizontalAdvance(m_nameValue);
    int xName = 0 - textWidthName / 2;
    int yName = -side / 8;
    painter.drawText(xName, yName, m_nameValue);
}

 void SpeedometerWidget::drawPointer(QPainter &painter, int side) {
    qreal radius1 = side / 2;
    qreal angleValue = 220 + (m_value - m_minValue) / (m_maxValue - m_minValue) * 280;
    painter.rotate(angleValue);

    QPointF pointAngleValue = getPointOnCircle(radius1, angleValue);
    QPainterPath pathTriangle;

    qreal diamEllipse = side / 7;
    pathTriangle.moveTo(0 + (diamEllipse / 2), 0);
    pathTriangle.lineTo(0 - (diamEllipse / 2), 0);
    pathTriangle.lineTo(0, -(side / 2.5));
    pathTriangle.closeSubpath();

    QColor colorTriangle("#f62434");
    colorTriangle.setAlpha(128);
    painter.setBrush(colorTriangle);
    painter.setPen(Qt::transparent);
    painter.drawPath(pathTriangle);

    colorTriangle.setAlpha(255);
    painter.setBrush(colorTriangle);
    painter.drawEllipse(0 - diamEllipse / 2, 0 - diamEllipse / 2, diamEllipse, diamEllipse);
}

QPointF SpeedometerWidget::getPointOnCircle(int radius, double angleDegrees) {
    double angleRadians = (angleDegrees - 90) * M_PI / 180.0; // Преобразуем угол в радианы
    return QPointF(radius * cos(angleRadians), radius * sin(angleRadians));
}

