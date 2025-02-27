#include "SpeedometerWidget.h"
#include <QPainter>
#include <QFontDatabase>
#include <QLocale>
#include <QPainterPath>

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
     // widgetFrame = rect();
     // widgetFrame.setSize(QSize(widgetFrame.width(),widgetFrame.height()));
     Q_UNUSED(event);

     // Устанавливаем фиксированный размер виджета как квадрат
     int side = qMin(width(), height());
     resize(side, side);
}

void SpeedometerWidget::paintEvent(QPaintEvent *event)  {
     Q_UNUSED(event);

     QPainter painter(this);
     painter.setRenderHint(QPainter::Antialiasing); 

     painter.setBrush(Qt::black);
     painter.setPen(Qt::transparent);

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
     
     int radius = side / 2 - 10; // Отступ от края круга

     painter.setPen(QPen(Qt::green, 3)); // Зеленый цвет для дуги
     int startAngle = 310 * 16;          // Начальный угол (225°, умноженный на 16)
     int spanAngle = 280 * 16;          // Длина дуги (-100°, так как движемся по часовой стрелке)

     // Прямоугольник для ограничения области дуги
     QRect arcRect(-side / 2.2, -side / 2.2, 2 * (side / 2.2), 2 * (side / 2.2));

     painter.drawArc(arcRect, startAngle, spanAngle); // Рисуем дугу

     // Опционально: добавляем метки на шкале
     
     const int numTicks = 14;            // Количество меток на шкале
     const double angleStep = 280.0 / numTicks; // Шаг между метками

     QFont fontTics;
     fontTics.setPointSize(side/56);
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
     int textHeight = fm.height();               

     int x = 0-textWidth/1.7; 
     int y = side/4;

     // valueFrame = QRect(x, y+textHeight, textWidth, textHeight);
     painter.drawText(x, y, text);

     // Создаем исходный прямоугольник для текста
     QRect originalRect(x, y-textHeight, textWidth, textHeight);

     // Применяем текущее преобразование к исходному прямоугольнику
     QTransform transform = painter.transform();
     QPolygonF transformedRect = transform.mapToPolygon(originalRect);

     // Получаем ограничивающий прямоугольник
     QRectF boundingRect = transformedRect.boundingRect();

     // Преобразуем в QRect
     valueFrame = boundingRect.toAlignedRect();
}

QPointF SpeedometerWidget::getPointOnCircle(int radius, double angleDegrees) {
     double angleRadians = (angleDegrees - 90) * M_PI / 180.0; // Преобразуем угол в радианы
     return QPointF(radius * cos(angleRadians), radius * sin(angleRadians));
 }

