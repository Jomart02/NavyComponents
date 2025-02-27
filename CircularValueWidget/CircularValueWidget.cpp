#include "CircularValueWidget.h"
#include <qmath.h> 

CircularValueWidget::CircularValueWidget(QWidget* parent) : QWidget(parent){

}

CircularValueWidget::~CircularValueWidget(){

}

void CircularValueWidget::setMaximum(double max) {
     if (max < m_minValue) { 
         m_minValue = max;
     }
     m_maxValue = max;
}

double CircularValueWidget::getMaximum() const{
     return m_maxValue;
}

void CircularValueWidget::setMinimum(double min) {
     if (min > m_maxValue) { 
         m_maxValue = min; 
     }
     m_minValue = min;

     setValue(m_value); 
}
double CircularValueWidget::getMinimum() const {
     return m_minValue;
}
void CircularValueWidget::setValue(double value){
     m_value = qBound(m_minValue, value, m_maxValue);
     update();
}
double CircularValueWidget::getValue(){
     return m_value;
}

void CircularValueWidget::setReadOnly(bool readOnly){
     m_readOnly = readOnly;
}
bool CircularValueWidget::getReadOnly(){
     return m_readOnly;
}
void CircularValueWidget::setDecimals(int dec){
     m_decimals = dec;
}
int CircularValueWidget::getDecimals(){
     return m_decimals;
}
