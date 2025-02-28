#include "CircularValueWidget.h"
#include <qmath.h> 
#include <QWheelEvent>
#include <QRegularExpression>
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
void CircularValueWidget::mousePressEvent(QMouseEvent *event){
     QPoint cursorPos = event->pos();
     pressed = true;

     calcValuePos(cursorPos);

     QWidget::mousePressEvent(event);
}
void CircularValueWidget::mouseMoveEvent(QMouseEvent *event){
     // QPoint cursorPos = event->pos();
     // if(pressed){
     //      if (valueViewFrame.contains(cursorPos)){
     //           calcValuePos(cursorPos);
     //      }
     // }
     QWidget::mouseMoveEvent(event);
}

void CircularValueWidget::mouseReleaseEvent(QMouseEvent *event){
     pressed = false;
     QWidget::mouseReleaseEvent(event);
}

void CircularValueWidget::wheelEvent(QWheelEvent *event) {
     QPoint cursorPos = event->position().toPoint();
     if (valueFrame.contains(cursorPos)) {
          int delta = event->angleDelta().y();
          if (delta > 0) {
               setValue(m_value + m_step);
          } else if (delta < 0) {
               setValue(m_value -m_step);
          }
     } else {
          QWidget::wheelEvent(event);
     }
}
void CircularValueWidget::toggleEditMode(bool enable) {
     if(m_editMode && m_editMode) m_editMode = false;
     else m_editMode = enable;
     update(); // Перерисовываем виджет
 }
 void CircularValueWidget::mouseDoubleClickEvent(QMouseEvent *event)  {
     // Проверяем, был ли двойной клик по области текста
     if (valueFrame.contains(event->position().toPoint())) {
         toggleEditMode(true); // Включаем режим редактирования
     } else {
         QWidget::mouseDoubleClickEvent(event);
     }
 }

 void CircularValueWidget::keyPressEvent(QKeyEvent *event) {

     switch (event->key()) {
          case Qt::Key_Up: // Перемещение курсора влево
               setValue(m_value + m_step);
               return;
          break;
          case Qt::Key_Down: // Перемещение курсора влево
               setValue(m_value - m_step);
               return;
          break;
     }


     if (!m_editMode) {
          QWidget::keyPressEvent(event); // Если не в режиме редактирования, передаем событие дальше
          return;
      }
     // QString currentText = QString::number(m_value, 'f', m_decimals); // Текущее значение как строка
     

     switch (event->key()) {
          case Qt::Key_Left: // Перемещение курсора влево
               m_cursorPosition = qMax(0, m_cursorPosition - 1);
          break;
          case Qt::Key_Right: // Перемещение курсора вправо
               m_cursorPosition = qMin(newText.size(), m_cursorPosition + 1);
          break;
          case Qt::Key_Backspace: // Удаление символа слева от курсора
               if (m_cursorPosition > 0) {
                    newText.remove(m_cursorPosition - 1, 1);
                    m_cursorPosition--;
               }
          break;
          case Qt::Key_Delete: // Удаление символа справа от курсора
               if (m_cursorPosition < newText.size()) {
                    newText.remove(m_cursorPosition, 1);
               }
          break;

          case Qt::Key_Enter:
          case Qt::Key_Return: // Подтверждение ввода
               toggleEditMode(false);
          return;
          case Qt::Key_Escape: // Отмена редактирования
               toggleEditMode(false);
          return;

          default:
               if (event->text().size() == 1 && isValidInput(newText, event->text())) {
                    // Вставляем символ в текущую позицию курсора
                    QString temp = newText;
                    temp.insert(m_cursorPosition, event->text());
                    bool ok = false;
                    double newValue = temp.toDouble(&ok);
                    if (ok && newValue<m_maxValue) {
                         newText.insert(m_cursorPosition, event->text());
                         m_cursorPosition++;
                    }
               } else {
                    return; // Игнорируем недопустимые символы
               }
          break;
     }


     bool ok = false;
     double newValue = newText.toDouble(&ok);
     
     if (ok) {
         setValue(newValue);
     } else {
         return; // Игнорируем некорректные значения
     }
     // currentText = QString::number(m_value, 'f', m_decimals);
     // updateCursorPosition(currentText); // Корректируем позицию курсора
     // update(); // Перерисовываем виджет
 }


 bool CircularValueWidget::isValidInput(const QString &currentText, const QString &input) const {
     // Разрешаем только цифры и одну точку
     if (!QRegularExpression("[0-9.]").match(input).hasMatch()) {
         return false;
     }
     // Проверяем, что точка добавляется только один раз
     if (input == "." && currentText.contains(".")) {
         return false;
     }

     return true;
 }

 void CircularValueWidget::updateCursorPosition(const QString &newText) {
     // Корректируем позицию курсора после изменения текста
     m_cursorPosition = qMin(m_cursorPosition, newText.size());
     update();
 }