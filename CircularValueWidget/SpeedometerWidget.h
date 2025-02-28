#pragma once 
#include "CircularValueWidget.h"

class SpeedometerWidget : public CircularValueWidget{
     Q_OBJECT
public:   
     SpeedometerWidget(QWidget *parent = nullptr);
     ~SpeedometerWidget();

     virtual void setUnits(int type) override; 
     virtual void setUnitsName(int type, QString nameUints) override; 
protected:
     void resizeEvent(QResizeEvent *event) override;
     void paintEvent(QPaintEvent *event) override;
     virtual void calcValuePos(QPoint pos) override;
private:
     QPointF getPointOnCircle(int radius, double angleDegrees);
private:
     QRectF widgetFrame;
     QFont fontValue;
     qreal angleValue;
};

