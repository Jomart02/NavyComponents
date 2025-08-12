#include "CompassWidget.h"
#include <QPainter>
#include <QFontDatabase>
#include <QLocale>
#include <QPainterPath>
#include <QColor>



CompassWidget::CompassWidget(QWidget* parent) : CircularValueWidget(parent){
     Q_INIT_RESOURCE(resCircularWidgets);
     int fontId = QFontDatabase::addApplicationFont(":/lcd"); 
     QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
     fontValue =  QFont(fontFamily, 48); 
     setMaximum(360.0);
     setWrapping(true);
}
CompassWidget::~CompassWidget(){

}
QColor CompassWidget::getBackground(){
    return m_background;
}
void CompassWidget::setBackground(QColor color){
    m_background = color;
    update();
}
QColor CompassWidget::getTextNameColor(){
    return m_textNameColor;
}
void CompassWidget::setTextNameColor(QColor color){
    m_textNameColor = color;
    update();
}
QColor CompassWidget::getTextValueColor(){
     return m_textValueColor;
}
void CompassWidget::setTextValueColor(QColor color){
    m_textValueColor = color;
    update();
}
QColor CompassWidget::getUintsColor(){
    return m_textUintsColor;
}
void CompassWidget::setUintsColor(QColor color){
    m_textUintsColor = color;
    update();
}
QColor CompassWidget::getMarksColor(){
    return m_marksColor;
}
void CompassWidget::setMarksColor(QColor color){
    m_marksColor = color;
    update();
}



void CompassWidget::resizeEvent(QResizeEvent *event) {
     Q_UNUSED(event);
     // Устанавливаем фиксированный размер виджета как квадрат
     int side = qMin(width(), height());
     resize(side, side);
}
void CompassWidget::paintEvent(QPaintEvent *event) {
     Q_UNUSED(event);

     QPainter painter(this);
     preparePainter(painter);

     int side = qMin(width(), height());
     qreal radius = side / 2;
     qreal weightBorder = radius/80;

     drawBackground(painter, radius,weightBorder);
     drawSideMarks(painter, radius,weightBorder);
     drawDirectionLabels(painter, radius, weightBorder);
     drawCompassArrow(painter, radius, weightBorder);
     drawValueText(painter, radius, weightBorder);
}

void CompassWidget::calcValuePos(QPoint pos , bool move) {
     QPoint center(width() / 2, height() / 2); // Центр виджета
     int x_local = pos.x() - center.x(); // Перемещаем начало координат в центр
     int y_local = center.y() - pos.y(); 

     // Вычисляем расстояние до центра
     double dist = std::sqrt(x_local * x_local + y_local * y_local);

     // Вычисляем угол
     double angle_radians = std::atan2(y_local, x_local);
     double angle_degrees = fmod((90 - angle_radians * (180 / M_PI)), 360.0); 
     if (angle_degrees < 0) {
          angle_degrees += 360.0; // Коррекция для диапазона [0, 360]
     }
     int side = qMin(width(), height());
     bool is_valueEdit = (dist > side/ 2.4);
     if(is_valueEdit) setValue(angle_degrees);
}


void CompassWidget::drawBackground(QPainter &painter, qreal radius, qreal weightBorder){
   
    //  (фон компаса)
     painter.setBrush(m_background);
     painter.setPen(Qt::transparent);
     painter.drawEllipse(QPointF(0, 0), radius, radius);

     //  белая обводка круга
     painter.setBrush(Qt::NoBrush);
     painter.setPen(QPen(Qt::white, weightBorder));
     painter.drawEllipse(QPointF(0, 0), radius-weightBorder/2, radius-weightBorder/2);
}


void CompassWidget::drawSideMarks(QPainter &painter, qreal radius, qreal weightBorder){
    
     qreal radius1 = radius-weightBorder/2;//Учисываем круг
     qreal markLength = radius/40; // Длина рисок (в пикселях)

     painter.setPen(QPen(Qt::white, 2));

     // Рисуем риски каждые 5 градусов
     for (int angle = 0; angle < 360; angle += 5) {
          painter.save();
          painter.rotate(angle);
          painter.drawLine(QPointF(radius1, 0), QPointF(radius1 - markLength, 0));
          painter.restore();
     }
}

void CompassWidget::drawDirectionLabels(QPainter &painter, qreal radius, qreal weightBorder) {
     qreal radius1 = radius - weightBorder / 2; // Радиус обводки
     qreal markLength = radius / 40; // Длина рисок
     qreal textRadius = radius1 - markLength - radius / 8; // Отступ для основных направлений
     qreal angleTextRadius = radius1 - markLength - radius / 12; // Меньший отступ для углов

     //  шрифт для основных направлений
     QFont mainFont = painter.font();
     mainFont.setPixelSize(radius / 8); 
     mainFont.setBold(true);
     painter.setFont(mainFont);
     painter.setPen(Qt::white);

     QMap<int, QString> directions = {
          {0, "N"}, {45, "NE"}, {90, "E"}, {135, "SE"},
          {180, "S"}, {225, "SW"}, {270, "W"}, {315, "NW"}
     };

     for (auto it = directions.constBegin(); it != directions.constEnd(); ++it) {
          int angle = it.key();
          QString label = it.value();

          // чтобы север был сверху
          qreal correctedAngle = angle - 90;
          qreal radians = qDegreesToRadians(static_cast<qreal>(correctedAngle));

          //  координаты для текста
          qreal x = textRadius * cos(radians);
          qreal y = textRadius * sin(radians);

          // размер текста для центрирования
          QFontMetrics fm(painter.font());
          QRectF textRect = fm.boundingRect(label);
          qreal textWidth = textRect.width();
          qreal textHeight = textRect.height();

          x -= textWidth / 2;
          y += textHeight / 4; 

          painter.drawText(QPointF(x, y), label);
     }


     QFont angleFont = fontValue;
     angleFont.setPixelSize(radius / 12); 
     angleFont.setBold(true);
     painter.setFont(angleFont);

     //  промежуточные углы каждые 15 градусов, исключая основные направления
     for (int angle = 0; angle < 360; angle += 15) {
               
          if (directions.contains(angle)) {
               continue;
          }

          qreal correctedAngle = angle - 90;
          qreal radians = qDegreesToRadians(static_cast<qreal>(correctedAngle));

          QString label = QString::number(angle);

          qreal x = angleTextRadius * cos(radians);
          qreal y = angleTextRadius * sin(radians);

          QFontMetrics fm(painter.font());
          QRectF textRect = fm.boundingRect(label);
          qreal textWidth = textRect.width();
          qreal textHeight = textRect.height();

          x -= textWidth / 2;
          y += textHeight / 4; 

          painter.drawText(QPointF(x, y), label);
     }
}

void CompassWidget::drawCompassArrow(QPainter &painter, qreal radius, qreal weightBorder){

     qreal centerCircleRadius = radius / 4; // Радиус центрального круга
     qreal arrowLength = radius * 0.75; // Длина стрелки (до края компаса)
     qreal arrowWidth = radius / 40; 
     
     painter.setBrush(Qt::white);
     painter.setPen(Qt::transparent);
     painter.drawEllipse(QPointF(0, 0), centerCircleRadius, centerCircleRadius);

     painter.save();
     painter.rotate(m_value); // Поворот по текущему углу (0° = север)
     QPainterPath arrowPath;
     arrowPath.moveTo(0 + centerCircleRadius,0); // Начало у края круга
     arrowPath.lineTo(0 - (centerCircleRadius ), 0);
     arrowPath.lineTo(0 ,-arrowLength);
     arrowPath.closeSubpath();
     painter.setBrush(Qt::white);
     painter.setPen(Qt::transparent);
     painter.drawPath(arrowPath);
     painter.restore();
 
 
     painter.save();
     painter.rotate(m_value+180); // Противоположное направление
     QPainterPath arrowPath2;
     arrowPath2.moveTo(0 + centerCircleRadius-weightBorder,0); 
     arrowPath2.lineTo(0 - (centerCircleRadius-weightBorder ), 0);
     arrowPath2.lineTo(0 ,-(arrowLength-weightBorder*2));
     arrowPath2.closeSubpath();
     painter.setBrush(Qt::NoBrush);
     painter.setPen(QPen(Qt::white, weightBorder));
     painter.drawPath(arrowPath2);
     painter.restore();
}

void CompassWidget::drawValueText(QPainter &painter, qreal radius, qreal weightBorder) {
     Q_UNUSED(weightBorder);
 
     qreal centerCircleRadius = radius / 4; 
     qreal backgroundRadius = centerCircleRadius * 0.8; // Радиус области фона
 
     painter.setBrush(m_background);
     painter.setPen(Qt::transparent);
     painter.drawEllipse(QPointF(0, 0), backgroundRadius, backgroundRadius);
 
   
     QString angleText = QLocale().toString(m_value, 'f', m_decimals);
     QString degreeSymbol = "°";
 
     
     QFont font = fontValue;
     painter.setFont(font);
 
    
     qreal maxFontSize = backgroundRadius; 
     qreal fontSize = maxFontSize;
     QFontMetrics fm(font);
     qreal totalWidth = 0;
     qreal textHeight = 0;
     qreal maxWidth = 2 * backgroundRadius * 0.9; // 90% диаметра для запаса
     qreal maxHeight = 2 * backgroundRadius * 0.9;
 
     do {
         font.setPointSizeF(fontSize);
         painter.setFont(font);
         fm = QFontMetrics(font);
         totalWidth = fm.horizontalAdvance(angleText) + fm.horizontalAdvance(degreeSymbol) + radius / 50; // Отступ между текстом и °
         textHeight = fm.ascent();
         if (totalWidth <= maxWidth && textHeight <= maxHeight) {
             break;
         }
         fontSize -= 0.5; 
     } while (fontSize > 1);
 
    
     painter.setPen(m_editMode ? Qt::yellow : m_textValueColor);
 
     
     qreal angleWidth = fm.horizontalAdvance(angleText);
     qreal degreeWidth = fm.horizontalAdvance(degreeSymbol);
     qreal totalWidthWithSpace = angleWidth + degreeWidth + radius / 50; 
     qreal x = -totalWidthWithSpace / 2; 
     qreal y = textHeight / 2; 
 
    
     painter.drawText(QPointF(x, y), angleText);
 
     
     painter.drawText(QPointF(x + angleWidth + radius / 50, y), degreeSymbol);
 
  
     qreal textHeightDelta = -(textHeight - textHeight / 5); 
     QRect textFrame(x, y + textHeightDelta, totalWidthWithSpace, -textHeightDelta);
     QTransform transform = painter.transform();
     QPolygonF transformedRect = transform.mapToPolygon(textFrame);
     QRectF boundingRect = transformedRect.boundingRect();
     valueFrame = boundingRect.toAlignedRect();
 
     if (m_editMode) {
         QString leftPart = angleText.left(m_cursorPosition);
         qreal cursorX = fm.horizontalAdvance(leftPart) - totalWidthWithSpace / 2;
         qreal cursorY = y + textHeightDelta;
         painter.setPen(QPen(Qt::red, 5));
         painter.drawLine(QPointF(cursorX, cursorY), QPointF(cursorX, cursorY - textHeightDelta));
     }
 }