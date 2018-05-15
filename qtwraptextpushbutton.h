#ifndef QTWRAPTEXTPUSHBUTTON_H
#define QTWRAPTEXTPUSHBUTTON_H

#include <QPushButton>
#include <QProxyStyle>

class QtPushButtonStyleProxy : public QProxyStyle
{
public:
    QtPushButtonStyleProxy()
        : QProxyStyle()
    {
    }

    virtual void drawItemText(QPainter *painter, const QRect &rect,
        int flags, const QPalette &pal, bool enabled,
        const QString &text, QPalette::ColorRole textRole) const
    {
        flags |= Qt::TextWordWrap;
        QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, text, textRole);
    }

private:
    Q_DISABLE_COPY(QtPushButtonStyleProxy)
};

class QtWrapTextPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit QtWrapTextPushButton(QWidget *parent = 0);
    virtual ~QtWrapTextPushButton(){}
};

#endif // QTWRAPTEXTPUSHBUTTON_H
