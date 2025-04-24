#pragma once 
#include "CircularValueWidget.h"


class CompassWidget : public CircularValueWidget{
     Q_OBJECT

public:   
     CompassWidget(QWidget *parent = nullptr);
     ~CompassWidget();

protected:
     void resizeEvent(QResizeEvent *event) override;
     void paintEvent(QPaintEvent *event) override;
     virtual void calcValuePos(QPoint pos , bool move = false) override;
private:
     void drawBackground(QPainter &painter, qreal radius, qreal weightBorder);
     void drawSideMarks(QPainter &painter, qreal radius, qreal weightBorder);
     void drawDirectionLabels(QPainter &painter, qreal radius, qreal weightBorder);
     void drawCompassArrow(QPainter &painter, qreal radius, qreal weightBorder);
     void drawValueText(QPainter &painter, qreal radius, qreal weightBorder);
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