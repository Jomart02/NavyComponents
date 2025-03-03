#pragma once 
#include <QWidget>

class CircularValueWidget : public QWidget{
     Q_OBJECT
     Q_PROPERTY(bool readOnly READ getReadOnly WRITE setReadOnly)
     Q_PROPERTY(double maxValue READ getMaximum WRITE setMaximum)
     Q_PROPERTY(double minValue READ getMinimum WRITE setMinimum)
     Q_PROPERTY(double value READ getMinimum WRITE setMinimum)
     Q_PROPERTY(QString nameValue READ getNameValue WRITE setNameValue)
     Q_PROPERTY(QString nameUints READ getNameUints WRITE setNameUints)
     Q_PROPERTY(double step READ getStep WRITE setStep)
     Q_PROPERTY(double decimals READ getDecimals WRITE setDecimals)
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
     QString getNameValue();
     void setNameValue(QString name);
     QString getNameUints();
     void setNameUints(QString name);
     double getStep();
     void setStep(double step);
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
     virtual void calcValuePos(QPoint pos, bool move = false) = 0;
protected:
     double m_maxValue = 1500.0;
     double m_minValue = 0.0;
     bool m_readOnly = false;
     QString m_nameUints = "";
     QString m_nameValue = "";
     int m_decimals = 1;
     double m_value = 0.0;
     double m_step = 1.0;
     QRectF valueFrame;
     QRectF valueViewFrame;
     bool m_editMode = false; // Режим редактирования
     int m_cursorPosition = 0;
     bool pressed=true;
     bool isIntersectValueZone = false;
};
