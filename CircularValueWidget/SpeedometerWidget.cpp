#include "SpeedometerWidget.h"
#include <QPainter>
#include <QFontDatabase>
#include <QLocale>
#include <QPainterPath>
#include <QColor>
SpeedometerWidget::SpeedometerWidget(QWidget* parent) : CircularValueWidget(parent){
     Q_INIT_RESOURCE(res);
     int fontId = QFontDatabase::addApplicationFont(":/lcd"); 
     QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
     fontValue =  QFont(fontFamily, 48); 

}

SpeedometerWidget::~SpeedometerWidget(){

}

void SpeedometerWidget::setUnits(int type){
     update();
} 
void SpeedometerWidget::setUnitsName(int type, QString nameUints){
     update();
} 

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
     painter.setRenderHint(QPainter::Antialiasing); 
     // Рисуем круг
     int side = qMin(width(), height());
     painter.translate(width() / 2, height() / 2);

     qreal radius1 = side / 2; // Радиус эллипса
     // Находим точки для углов 225° и 125°
     QPointF point225 = getPointOnCircle(radius1,220);
     painter.setPen(QPen(Qt::red, 2));
     QPointF point125 = getPointOnCircle(radius1,140);
     
     // Создаём путь для рисования
     QPainterPath path;
     qreal angleStart = 310.0; // Начальный угол для дуги (вершина вверху)
     qreal angleSpan = 280.0;  // Угол разворота дуги (3/4 круга)

     // Добавляем дугу эллипса
     path.arcTo(-radius1, -radius1, side, side, angleStart, angleSpan);
     // Закрываем фигуру прямой линией (нижняя грань)
     path.lineTo(point125); // Линия от конца дуги до центра нижней грани

     // Заливка пути
     painter.setBrush(Qt::black); // Цвет заливки
     painter.setPen(Qt::transparent); // Без контура
     painter.drawPath(path); // Рисуем путь
     

     qreal scale_factor = 0.4;
     qreal lift_amount = -(side / 10);
     QTransform transformp;
     transformp.scale(scale_factor, scale_factor);
     transformp.translate(0, lift_amount);
     QPainterPath scaledPath = transformp.map(path);
     QColor colorSenter = QColor("#d9c1c3");
     // Создаем радиальный градиент
     QRadialGradient gradientBack(QPointF(0,0), side/2, QPointF(0,0)); // (центр, радиус, фокус)
     for (qreal pos = 0.0; pos <= 1.0; pos += 0.1) {
          // Вычисляем альфа-компонент для текущей позиции
          qreal k = 4.5;
          // Вычисляем альфа-компонент с использованием экспоненциальной функции
          qreal alpha = qExp(-k * pos); // Экспоненциальное затухание
          colorSenter.setAlphaF(alpha);
          // Добавляем остановку градиента
          gradientBack.setColorAt(pos, colorSenter);
      }

     // Создаем кисть с градиентом
     painter.setBrush(QBrush(gradientBack));
     painter.setPen(Qt::transparent);
     painter.drawPath(scaledPath);


     // Рассчитываем угол для значения m_value
     angleValue = 220 + (m_value - m_minValue) / (m_maxValue - m_minValue) * 280;
     int radius = side / 2 ; // Отступ от края круга
     if(m_value  > 0 ){         
          QLinearGradient gradient(-side / 2.1, -side / 2.1, side / 2.1, side / 2.1); 
          gradient.setColorAt(0.0, Qt::green); 
          gradient.setColorAt(1.0, Qt::red); 
          painter.setPen(QPen(QBrush(gradient), (side / 2.1) - (side / 2.25), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
          painter.setBrush(Qt::transparent);
          int startAngle = 220; // Начальный угол в градусах
          int endAngle = angleValue; // Конечный угол в градусах
          int stepAngle = 1; // Шаг угла (рисуем линию каждые 1 градус)
          
          QPainterPath pathLine;
          pathLine.moveTo( getPointOnCircle(side / 2.1,startAngle));
          for (int angle = startAngle; angle <= endAngle; ++angle) {
               // Вычисляем текущий угол в радианах
               pathLine.lineTo(getPointOnCircle(side / 2.1,angle));
          }
          painter.drawPath(pathLine);
     }

     // добавляем метки на шкале
     
     const int numTicks = 14;            // Количество меток на шкале
     const double angleStep = 280.0 / numTicks; // Шаг между метками

     QFont fontTics;
     fontTics.setPointSize(side/45);
     fontTics.setBold(true);
     painter.setFont(fontTics);

     for (int i = 0; i <= numTicks; ++i) {
          double angle = 220 + i * angleStep; // Текущий угол метки
          QPointF startPoint = getPointOnCircle(side / 2.1, angle); // Начальная точка метки
          QPointF endPoint = getPointOnCircle(side / 2.4, angle);   // Конечная точка метки
          painter.setPen(QPen(Qt::white, 4)); // Устанавливаем белый цвет для меток
          painter.drawLine(startPoint, endPoint); // Рисуем метку

          if (i % 1 == 0) { // Только для основных меток
               double value = m_minValue + (m_maxValue - m_minValue) * i / numTicks; // Рассчитываем значение
               QString text = QLocale().toString(value, 'f', 1);   
               QPointF textPoint = getPointOnCircle(side / 3.0,angle); // Позиция текста внутри круга
               QFontMetrics fm = painter.fontMetrics();
               QRect textRect = fm.boundingRect(text);
               int textWidth = fm.horizontalAdvance(text);
               int x , y;
               if(i == 0){
                    x = endPoint.x() + textWidth/3;
                    y = endPoint.y();
               }else if(i == numTicks){
                    x = endPoint.x() - textWidth*1.2;
                    y = endPoint.y();
               }else if( i >= (numTicks/2)-1 && i < numTicks/2){
                    x = endPoint.x() - textWidth/3;
                    y = endPoint.y() + fm.height() ;
               }else if(i == numTicks/2){
                    x = endPoint.x() - textWidth/2;
                    y = endPoint.y() + fm.height();
               }else if(i > numTicks/2 &&  i <= (numTicks/2)+1 ){
                    x = endPoint.x() - textWidth/2;
                    y = endPoint.y() + fm.height() ;
               }else{
                    if(angle <= 360){
                         x = endPoint.x() + textWidth/10;
                         y = endPoint.y() + fm.height()/3;
                    }else{
                         x = endPoint.x() - textWidth * 1.1;
                         y = endPoint.y() + fm.height()/3;
                    }
               }

               
               // Центрируем текст относительно позиции
               QPointF adjustedTextPoint(x,y);
                    
               // Рисуем текст
               painter.drawText(adjustedTextPoint, text);
          }

          painter.setPen(QPen(Qt::white, 2));
          //Отрисовка меток
          if(i < numTicks){
               //Центральная метка
               double angle = 220 + i * angleStep + angleStep*0.5; 
               QPointF startPoint = getPointOnCircle(side / 2.2, angle); 
               QPointF endPoint = getPointOnCircle(side / 2.4, angle);   
               painter.drawLine(startPoint, endPoint); 
               painter.setPen(QPen(Qt::white, 1));
               fontTics.setBold(false);
               //дробные метки
               for(int j = 0; j <= 10; ++j ){
                    double angle = 220 + i * angleStep +  j*angleStep*0.1;
                    QPointF startPoint = getPointOnCircle(side / 2.2, angle); 
                    QPointF endPoint = getPointOnCircle(side / 2.4, angle);  
                    painter.drawLine(startPoint, endPoint); 
               }
          }
     }

     
     fontValue.setPointSize(side/9);
     QString text = QLocale().toString(m_value, 'f', m_decimals);
     painter.setFont(fontValue);

     QFontMetrics fm(fontValue);
    
     int textWidth = fm.horizontalAdvance(text);
     int textHeight = fm.ascent();               
     int textHeightDelta = -(textHeight-textHeight/5);    
     int x = 0-textWidth/2; 
     int y = side/4;
     if(m_editMode) painter.setPen(Qt::yellow);
     else painter.setPen(Qt::white);
     painter.drawText(x, y, text);

     // Создаем исходный прямоугольник для текста
     QRect originalRect(x, y, textWidth, textHeightDelta);
     // Применяем текущее преобразование к исходному прямоугольнику
     QTransform transform = painter.transform();
     QPolygonF transformedRect = transform.mapToPolygon(originalRect);
     // Получаем ограничивающий прямоугольник
     QRectF boundingRect = transformedRect.boundingRect();
     // Преобразуем в QRect
     valueFrame = boundingRect.toAlignedRect();
     if(m_editMode){
          QString leftPart = text.left(m_cursorPosition);
          int cursorX = fm.horizontalAdvance(leftPart) - textWidth/2;
          int cursorY = originalRect.y();
          
          // Рисуем вертикальную линию для курсора
          painter.setPen(QPen(Qt::red, 5));
          painter.drawLine(cursorX, cursorY, cursorX, cursorY+textHeightDelta);
     }
     
     painter.setPen(Qt::white);
     QFont names =  QFont("Helvetica",side/25);
     names.setBold(true);
     painter.setFont(names);
     
     QFontMetrics fontunits(painter.font());
    
     int textWidthUnits = fontunits.horizontalAdvance(currentNameUints);
     int xUnits = 0-textWidthUnits/2; 
     int yUnits = side/3;
     painter.drawText(xUnits, yUnits, currentNameUints);

     int textWidthName= fontunits.horizontalAdvance(nameValue);
     int xName = 0-textWidthName/2; 
     int yName = -side/8;
     painter.drawText(xName, yName, nameValue);

     qreal diamEllipse = side/7;

     

     painter.rotate(angleValue);
     QPointF pointAngleValue = getPointOnCircle(radius1,angleValue);
     QPainterPath pathTriangle;

     pathTriangle.moveTo(0 + (diamEllipse / 2), 0); // Первая вершина (справа от центра)
     pathTriangle.lineTo(0 - (diamEllipse / 2), 0); // Вторая вершина (слева от центра)
     pathTriangle.lineTo(0, -(side / 2.5));          // Третья вершина (наверху)
     pathTriangle.closeSubpath();                  // Замыкаем треугольник


     QTransform transformTriangle = painter.transform();
     QPolygonF transformedRectTriangle = transform.mapToPolygon(originalRect);
     // Получаем ограничивающий прямоугольник
     QRectF boundingRectTriangle = transformedRect.boundingRect();

     valueViewFrame = boundingRect.toAlignedRect();

     QColor colorTriangle("#f62434");
     colorTriangle.setAlpha(128);
     painter.setBrush(colorTriangle); // Цвет заливки треугольника
     painter.setPen(Qt::transparent); // Без контура
     painter.drawPath(pathTriangle); // Рисуем треугольни

     colorTriangle.setAlpha(255);
     painter.setBrush(colorTriangle);
     painter.drawEllipse(0-diamEllipse/2,0-diamEllipse/2,diamEllipse,diamEllipse);     



}

QPointF SpeedometerWidget::getPointOnCircle(int radius, double angleDegrees) {
     double angleRadians = (angleDegrees - 90) * M_PI / 180.0; // Преобразуем угол в радианы
     return QPointF(radius * cos(angleRadians), radius * sin(angleRadians));
 }

