#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "layerwidget.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QString>
#include <QScrollBar>
#include <QPushButton>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    static_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout())->setSpacing(10);

    layersCount = 0;
    connect(ui->addLayerButton, SIGNAL(clicked()), this, SLOT(addLayerButtonClicked()));
    connect(ui->trainButton, SIGNAL(clicked()), this, SLOT(trainButtonClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* SLOTS */
void MainWindow::addLayerButtonClicked()
{
    layersCount++;
    QString name = "Layer " + QString::number(layersCount );
    LayerWidget *layer = new LayerWidget(name, this);
    layer->setObjectName(name);

    QVBoxLayout *editorLayout = static_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    editorLayout->addWidget(layer);
    connect(layer, SIGNAL(hovered()), this, SLOT(layerWidgetHovered()));
    connect(layer, SIGNAL(hoverEnded()), this, SLOT(layerWidgetHoverEnded()));
    connect(layer, SIGNAL(mousePressed()), this, SLOT(layerWidgetMousePressed()));
    connect(layer, SIGNAL(mouseReleased()), this, SLOT(layerWidgetMouseReleased()));

    QPushButton *layerMoveUpButton = layer->findChild<QPushButton*>("moveUpButton");
    connect(layerMoveUpButton , SIGNAL(clicked()), this, SLOT(layerWidgetMoveUpButtonClicked()));

    QPushButton *layerMoveDownButton = layer->findChild<QPushButton*>("moveDownButton");
    connect(layerMoveDownButton , SIGNAL(clicked()), this, SLOT(layerWidgetMoveDownButtonClicked()));

    QPushButton *layerRemoveButton = layer->findChild<QPushButton*>("removeButton");
    connect(layerRemoveButton, SIGNAL(clicked()), this, SLOT(layerWidgetRemoveButtonClicked()));

    // Enable/disable move up/down buttons of this layer and previous layer
    const int lastLayerIndex = editorLayout->count() - 1;
    if (lastLayerIndex > 0)
    {
        LayerWidget *previousLayer = static_cast<LayerWidget*>(editorLayout->itemAt(lastLayerIndex - 1)->widget());
        QPushButton *previousLayerDownButton = previousLayer->findChild<QPushButton*>("moveDownButton");
        QPushButton *previousLayerUpButton = previousLayer->findChild<QPushButton*>("moveUpButton");
        previousLayerDownButton->setEnabled(true);

        if (lastLayerIndex - 1 != 0)
        {
            previousLayerUpButton->setEnabled(true);
        }
        layerMoveDownButton->setEnabled(false); // Last layer (this) cannot go lower
    }
    else
    {
        // No previous layer
        layerMoveUpButton->setEnabled(false);
        layerMoveDownButton->setEnabled(false);
    }

}

void MainWindow::trainButtonClicked()
{

}

void MainWindow::layerWidgetHovered()
{
    //qDebug() << "Hovered " << sender()->objectName();
}

void MainWindow::layerWidgetHoverEnded()
{
    //qDebug() << "Hover ended" << sender()->objectName();
}

void MainWindow::layerWidgetMousePressed()
{
    //QApplication::setOverrideCursor(Qt::ClosedHandCursor);

    //LayerWidget *grabbedLayer = static_cast<LayerWidget*>(sender());
}

void MainWindow::layerWidgetMouseReleased()
{
    QApplication::restoreOverrideCursor();
}

void MainWindow::layerWidgetMoveUpButtonClicked()
{
    LayerWidget *layer = static_cast<LayerWidget*>(sender()->parent()->parent());
    QVBoxLayout *editorLayout = static_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());

    const int index = editorLayout->indexOf(layer);

    editorLayout->removeWidget(layer);
    editorLayout->insertWidget(index - 1, layer);

    const int newIndex = editorLayout->indexOf(layer);
    LayerWidget *switchedLayer = static_cast<LayerWidget*>(editorLayout->itemAt(newIndex + 1)->widget());

    // Enable move down button of this layer
    QPushButton *layerMoveDownButton = layer->findChild<QPushButton*>("moveDownButton");
    layerMoveDownButton->setEnabled(true);

    // Disable move up button of this layer if on top
    if (newIndex == 0) {
        QPushButton *layerMoveUpButton = layer->findChild<QPushButton*>("moveUpButton");
        layerMoveUpButton->setEnabled(false);
    }

    // Enable move up button of switched layer (switched layer could be on top)
    QPushButton *switchedLayerMoveUpButton = switchedLayer->findChild<QPushButton*>("moveUpButton");
    switchedLayerMoveUpButton->setEnabled(true);

    // Disable move down button of switched layer if on bottom
    if (newIndex + 1 == editorLayout->count() - 1)
    {
        QPushButton *switchedLayerMoveDownButton = switchedLayer->findChild<QPushButton*>("moveDownButton");
        switchedLayerMoveDownButton->setEnabled(false);
    }
}

void MainWindow::layerWidgetMoveDownButtonClicked()
{
    LayerWidget *layer = static_cast<LayerWidget*>(sender()->parent()->parent());
    QVBoxLayout *editorLayout = static_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());

    const int index = editorLayout->indexOf(layer);

    editorLayout->removeWidget(layer);
    editorLayout->insertWidget(index + 1, layer);

    const int newIndex = editorLayout->indexOf(layer);
    LayerWidget *switchedLayer = static_cast<LayerWidget*>(editorLayout->itemAt(newIndex - 1)->widget());

    // Enable move up button of this layer
    QPushButton *layerMoveUpButton = layer->findChild<QPushButton*>("moveUpButton");
    layerMoveUpButton->setEnabled(true);

    // Disable move down button of this layer if on bottom
    if (newIndex == editorLayout->count() - 1) {
        QPushButton *layerMoveDownButton = layer->findChild<QPushButton*>("moveDownButton");
        layerMoveDownButton->setEnabled(false);
    }

    // Enable move down button of switched layer (switcher layer could be on bottom)
    QPushButton *switchedLayerMoveDownButton = switchedLayer->findChild<QPushButton*>("moveDownButton");
    switchedLayerMoveDownButton->setEnabled(true);

    // Disable move up button of switched layer if on top
    if (newIndex - 1 == 0)
    {
        QPushButton *switchedLayerMoveUpButton = switchedLayer->findChild<QPushButton*>("moveUpButton");
        switchedLayerMoveUpButton->setEnabled(false);
    }
}

void MainWindow::layerWidgetRemoveButtonClicked()
{
    LayerWidget *layer = static_cast<LayerWidget*>(sender()->parent()->parent());
    QVBoxLayout *editorLayout = static_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());

    editorLayout->removeWidget(layer);
    layersCount--;
}
