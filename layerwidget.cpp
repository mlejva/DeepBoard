#include "layerwidget.h"
#include "ui_layerwidget.h"

#include <QDebug>
#include <QString>
#include <QEvent>

LayerWidget::LayerWidget(QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayerWidget)
{
    ui->setupUi(this);

    ui->nameLineEdit->setText(name);
    this->setMouseTracking(true);
}

LayerWidget::~LayerWidget()
{
    delete ui;
}

/* Overrides */
void LayerWidget::enterEvent(QEvent *e)
{
    Q_EMIT hovered();

    QWidget::enterEvent(e);
}

void LayerWidget::leaveEvent(QEvent *e)
{
    Q_EMIT hoverEnded();

    QWidget::leaveEvent(e);
}

void LayerWidget::mousePressEvent(QMouseEvent *e)
{
    Q_EMIT mousePressed();

    QWidget::mousePressEvent(e);
}

void LayerWidget::mouseReleaseEvent(QMouseEvent *e)
{
    Q_EMIT mouseReleased();

    QWidget::mouseReleaseEvent(e);
}
