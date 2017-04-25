#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "layerwidget.h"

#include <QMainWindow>
#include <QDebug>
#include <QComboBox>
#include <QSpinBox>

#include "../DeepEngine/DeepEngine/Matrix.h"
#include "../DeepEngine/DeepEngine/Network.h"
#include "../DeepEngine/DeepEngine/Layers/TanhLayer.h"
#include "../DeepEngine/DeepEngine/Layers/ReluLayer.h"
#include "../DeepEngine/DeepEngine/Layers/SigmoidLayer.h"
#include "../DeepEngine/DeepEngine/Layers/IdentityLayer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

/* Consttructors & Destructor */
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

/* Public functions */
public:
    void printToConsole(const QString &text);

/* Private properties */
private:
    Ui::MainWindow *ui;
    int layersCount;
    enum NetworkDataType { FloatType, DoubleType };
    enum NetworkLossFunctionType { MSEType };

    template <typename T, typename LossFunctionType>
    Network<T, LossFunctionType> setupNetwork(const QList<LayerWidget*> &layersInEditor)
    {
        Network<T, LossFunctionType> n;

        for (const auto& layer : layersInEditor)
        {
            const int outputSize = layer->findChild<QSpinBox*>("outputSizeSpinBox")->value();
            const QString &activationFunctionString = layer->findChild<QComboBox*>("activationFunctionComboBox")->currentText();

            if (activationFunctionString == "Identity")
            {
                n.template AddLayer<Layers::IdentityLayer<T>>(outputSize);
            }
            else if (activationFunctionString == "Tanh")
            {
                n.template AddLayer<Layers::TanhLayer<T>>(outputSize);
            }
            else if (activationFunctionString == "ReLU")
            {
                n.template AddLayer<Layers::ReluLayer<T>>(outputSize);
            }
            else if (activationFunctionString == "Sigmoid")
            {
                n.template AddLayer<Layers::SigmoidLayer<T>>(outputSize);
            }
        }

        return n;
    }

/* Slots */
public slots:
    void addLayerButtonClicked();
    void trainButtonClicked();
    void clearConsoleButtonClicked();

    void layerWidgetHovered();
    void layerWidgetHoverEnded();
    void layerWidgetMousePressed();
    void layerWidgetMouseReleased();
    void layerWidgetMoveUpButtonClicked();
    void layerWidgetMoveDownButtonClicked();
    void layerWidgetRemoveButtonClicked();
};

#endif // MAINWINDOW_H
