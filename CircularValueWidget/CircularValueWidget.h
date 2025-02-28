#pragma once 
#include <QWidget>

class CircularValueWidget : public QWidget{
     Q_OBJECT
     // Q_PROPERTY(int desimals READ getBackground WRITE setDecimals)
     Q_PROPERTY(bool readOnly READ getReadOnly WRITE setReadOnly)
     Q_PROPERTY(double maxValue READ getMaximum WRITE setMaximum)
     Q_PROPERTY(double minValue READ getMinimum WRITE setMinimum)
     Q_PROPERTY(double value READ getMinimum WRITE setMinimum)
public:
     CircularValueWidget(QWidget *parent = nullptr);
     ~CircularValueWidget();

     void setMaximum(double max);
     double getMaximum() const;
     void setMinimum(double min);
     double getMinimum() const;
     void setReadOnly(bool readOnly);
     bool getReadOnly();
     void setDecimals(int dec);
     int getDecimals();
     void setValue(double value);
     double getValue();

     virtual void setUnits(int type) = 0; 
     virtual void setUnitsName(int type, QString nameUints) = 0; 
     
protected:
     void toggleEditMode(bool enable);
     void mousePressEvent(QMouseEvent *event) override;
     void wheelEvent(QWheelEvent *event) override ;
     void keyPressEvent(QKeyEvent *event) override ;
     void mouseDoubleClickEvent(QMouseEvent *event) override;
     void updateCursorPosition(const QString &newText);
     bool isValidInput(const QString &currentText, const QString &input) const ;
     void  mouseMoveEvent(QMouseEvent *event) override;
     void mouseReleaseEvent(QMouseEvent *event) override;
     virtual void calcValuePos(QPoint pos) = 0;
protected:
     double m_maxValue = 1500.0;
     double m_minValue = 0.0;
     bool m_readOnly = false;
     int curentUnits = 0;
     QString currentNameUints = "knots";
     QString nameValue = "BMW";
     int m_decimals = 1;
     double m_value = 0.0;
     double m_step = 1.0;
     QRectF valueFrame;
     QRectF valueViewFrame;
     bool m_editMode = false; // Режим редактирования
     int m_cursorPosition = 0;
     QString newText;

     bool pressed=true;

};
