#include "SpeedometerWidget.h"
#include <QPainter>
#include <QFontDatabase>
#include <QLocale>

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
     
     painter.drawEllipse(QRect(-side/2, -side/2, side, side));
     
     
     painter.setPen(QPen(Qt::white, 2));
     int radius = side / 2 - 10; // Отступ от края круга
     // Функция для вычисления точки на окружности

     // Находим точки для углов 225° и 125°
     QPointF point225 = getPointOnCircle(radius,220);
     painter.setPen(QPen(Qt::red, 2));
     QPointF point125 = getPointOnCircle(radius,140);

     // Рисуем линии от центра до этих точек
     // painter.drawLine(QPointF(0, 0), point225); // Линия для угла 225°
     // painter.drawLine(QPointF(0, 0), point125); // Линия для угла 125°
     painter.setPen(QPen(Qt::green, 3)); // Зеленый цвет для дуги
     int startAngle = 310 * 16;          // Начальный угол (225°, умноженный на 16)
     int spanAngle = 280 * 16;          // Длина дуги (-100°, так как движемся по часовой стрелке)

     // Прямоугольник для ограничения области дуги
     QRect arcRect(-side / 2.2, -side / 2.2, 2 * (side / 2.2), 2 * (side / 2.2));

     painter.drawArc(arcRect, startAngle, spanAngle); // Рисуем дугу

   // Опционально: добавляем метки на шкале
    painter.setPen(QPen(Qt::white, 2)); // Устанавливаем белый цвет для меток
    const int numTicks = 14;            // Количество меток на шкале
    const double angleStep = 280.0 / numTicks; // Шаг между метками

     for (int i = 0; i <= numTicks; ++i) {
          double angle = 220 + i * angleStep; // Текущий угол метки
          QPointF startPoint = getPointOnCircle(side / 2.2, angle); // Начальная точка метки
          QPointF endPoint = getPointOnCircle(side / 2.4, angle);   // Конечная точка метки
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
                    x = endPoint.x()+ textWidth/3;
                    y = endPoint.y();
               }else if(i == 1){
                    x = endPoint.x() + textWidth/3;
                    y = endPoint.y()  + fm.height()/3 ;
               }else if( i >= (numTicks/2)-2 && i < numTicks/2){
                    x = endPoint.x() - textWidth/3;
                    y = endPoint.y() + fm.height() ;
               }else if(i == numTicks/2){
                    x = endPoint.x() - textWidth/2;
                    y = endPoint.y() + fm.height();
               }else{
                    if(angle <= 360){
                         x = endPoint.x() + textWidth/10;
                         y = endPoint.y() + fm.height()/3;
                    }else{
                         x = endPoint.x() ;
                         y = textPoint.y() + textRect.height() / 2;
                    }
               }

               qDebug() << x << text;
               // Центрируем текст относительно позиции
               QPointF adjustedTextPoint(x,y);
                    
               // Рисуем текст
               painter.drawText(adjustedTextPoint, text);
           }
     }

     
    fontValue.setPointSize(side/9);
    QString text = QLocale().toString(150.0, 'f', m_decimals);
    painter.setFont(fontValue);

    QFontMetrics fm(fontValue);
    int textWidth = fm.horizontalAdvance(text);
    int textHeight = fm.height();               

     int x = -textWidth / 2; 
     int y = fm.ascent();

    painter.drawText(x, y, text);
}

QPointF SpeedometerWidget::getPointOnCircle(int radius, double angleDegrees) {
     double angleRadians = (angleDegrees - 90) * M_PI / 180.0; // Преобразуем угол в радианы
     return QPointF(radius * cos(angleRadians), radius * sin(angleRadians));
 }