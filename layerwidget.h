#ifndef LAYERWIDGET_H
#define LAYERWIDGET_H

#include <QWidget>
#include <QString>
#include <QEvent>

namespace Ui {
class LayerWidget;
}

class LayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LayerWidget(const QString &name, const int &position, QWidget *parent = 0);
    ~LayerWidget();

private:    
    Ui::LayerWidget *ui;
    int position_;

/* Setters & Getters */
public:
    void setPosition(const int &newPosition) { position_ = newPosition; }
    int getPosition() const { return position_; };   

/* Overrides */
protected:
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

public: Q_SIGNALS:
    void hovered();
    void hoverEnded();
    void mousePressed();
    void mouseReleased();
};

#endif // LAYERWIDGET_H
