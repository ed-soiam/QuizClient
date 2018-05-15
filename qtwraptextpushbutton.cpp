#include "qtwraptextpushbutton.h"

QtWrapTextPushButton::QtWrapTextPushButton(QWidget *parent) : QPushButton(parent)
{
    setStyle(new QtPushButtonStyleProxy());
}
