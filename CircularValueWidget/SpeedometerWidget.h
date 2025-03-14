#pragma once 
#include "CircularValueWidget.h"

class SpeedometerWidget : public CircularValueWidget{
     Q_OBJECT
     Q_PROPERTY(QColor background READ getBackground WRITE setBackground DESIGNABLE true)
     Q_PROPERTY(QColor textNameColor READ getTextNameColor WRITE setTextNameColor DESIGNABLE true)
     Q_PROPERTY(QColor textValueColor READ getTextValueColor WRITE setTextValueColor DESIGNABLE true)
     Q_PROPERTY(QColor textUintsColor READ getUintsColor WRITE setUintsColor DESIGNABLE true)
     Q_PROPERTY(QColor marksColor READ getMarksColor WRITE setMarksColor DESIGNABLE true)
public:   
     SpeedometerWidget(QWidget *parent = nullptr);
     ~SpeedometerWidget();
     QColor getBackground();
     void setBackground(QColor color);
     QColor getTextNameColor();
     void setTextNameColor(QColor color);
     QColor getTextValueColor();
     void setTextValueColor(QColor color);
     QColor getUintsColor();
     void setUintsColor(QColor color);
     QColor getMarksColor();
     void setMarksColor(QColor color);


protected:
     void resizeEvent(QResizeEvent *event) override;
     void paintEvent(QPaintEvent *event) override;
     virtual void calcValuePos(QPoint pos , bool move = false) override;
private:
     QPointF getPointOnCircle(int radius, double angleDegrees);
     void preparePainter(QPainter &painter);
     void drawBackground(QPainter &painter, int side) ;
     void drawValueIndicator(QPainter &painter, int side);
     void drawScaleMarks(QPainter &painter, int side);
     void drawCurrentValue(QPainter &painter, int side);
     void drawTextLabels(QPainter &painter, int side);
     void drawPointer(QPainter &painter, int side);
private:
     QRectF widgetFrame;
     QFont fontValue;
     qreal angleValue;
     QColor m_background = Qt::black;
     QColor m_textNameColor = Qt::white;
     QColor m_textValueColor = Qt::white;
     QColor m_textUintsColor = Qt::white;
     QColor m_marksColor = Qt::white;
     
};

