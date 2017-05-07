#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "layerwidget.h"

#include <QMainWindow>
#include <QDebug>
#include <QComboBox>
#include <QSpinBox>
#include <QString>
#include <QFileDialog>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>


#include "../DeepEngine/DeepEngine/Matrix.h"
#include "../DeepEngine/DeepEngine/Network.h"
#include "../DeepEngine/DeepEngine/Layers/TanhLayer.h"
#include "../DeepEngine/DeepEngine/Layers/ReluLayer.h"
#include "../DeepEngine/DeepEngine/Layers/SigmoidLayer.h"
#include "../DeepEngine/DeepEngine/Layers/IdentityLayer.h"

using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

/* Constructors & Destructor */
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

/* Private properties */
private:
    Ui::MainWindow *ui;
    int layersCount;    
    enum Color { black, red };

/* Public functions */
public:
    void printToConsole(const QString &text, const QColor &c = Qt::black);
    void chartLoss(const std::vector<double> &lossData);

/* Template private functions */
private:        
    template <typename T, typename LossFunctionType>
    void runNetwork(Network<T, LossFunctionType> &n,
                         const Matrix<T> &trainInput,
                         const Matrix<T> &trainExpectedOutput,
                         const Matrix<T> &testInput,
                         const Matrix<T> &testExpectedOutput,
                         const int &batchSize,
                         const int &epochCount,
                         const bool &saveTestOutput,
                         const QString &outputPath)
    {
        auto trainInputSize = trainInput.GetRowsCount();
        auto batchCount = trainInputSize / batchSize;
        if (trainInputSize % batchSize != 0)
            batchCount++; // Last batch will be less than batchSize

        std::vector<double> lossData;
        Matrix<T> networkOutput;
        for (auto epoch = 0; epoch < epochCount; ++epoch)
        {
            auto output = QStringLiteral("Epoch %1 (out of %2)...").arg(epoch + 1).arg(epochCount + 1);
            MainWindow::printToConsole(output);

            // Go through the whole train data
            try
            {
                trainOnDataset(n, batchCount, batchSize, trainInput, trainExpectedOutput, lossData);
            }
            catch (const std::exception &ex) {
                QString message = "Exception: ";
                message.append(ex.what());
                MainWindow::printToConsole(message, Qt::red);
            }

            // Run evaluation on test dataset
            const auto& testTargets = n.Evaluate(testInput, testExpectedOutput);
            const auto& testLoss = std::get<0>(testTargets);
            if (epoch == epochCount - 1)
                networkOutput = static_cast<Matrix<T>>(std::get<1>(testTargets));

            output = QStringLiteral("\tTest loss: %1").arg(testLoss);
            MainWindow::printToConsole(output);
        }

        MainWindow::chartLoss(lossData);

        if (saveTestOutput)
            networkOutput.SaveToFile(outputPath.toStdString());
    }

    template <typename T, typename LossFunctionType>
    void runNetwork(Network<T, LossFunctionType> &n,
                  const Matrix<T> &trainInput,
                  const Matrix<T> &trainExpectedOutput,
                  const int &batchSize,
                  const int &epochCount)
    {
        auto trainInputSize = trainInput.GetRowsCount();
        auto batchCount = trainInputSize / batchSize;
        if (trainInputSize % batchSize != 0)
            batchCount++; // Last batch will be less than batchSize

        std::vector<double> lossData;
        for (auto epoch = 0; epoch < epochCount; ++epoch)
        {
            auto output = QStringLiteral("Epoch %1 (out of %2)...").arg(epoch).arg(epochCount);
            MainWindow::printToConsole(output);

            // Go through the whole train data
            try
            {
                trainOnDataset(n, batchCount, batchSize, trainInput, trainExpectedOutput, lossData);
            }
            catch (const std::exception &ex) {
                QString message = "Exception: ";
                message.append(ex.what());
                MainWindow::printToConsole(message, Qt::red);
            }
        }

        MainWindow::chartLoss(lossData);
    }

    template <typename T, typename LossFunctionType>
    void trainOnDataset(Network<T, LossFunctionType> &n,
                        const size_t &batchCount,
                        const size_t &batchSize,
                        const Matrix<T> &trainInput,
                        const Matrix<T> &trainExpectedOutput,
                        std::vector<double> &lossAggregator)
    {
        for (size_t batchIndex = 0; batchIndex < batchCount; ++batchIndex)
        {
            auto output = QStringLiteral("\tBatch %1 (out of %2)").arg(batchIndex).arg(batchCount);
            MainWindow::printToConsole(output);

            const auto& batchInput = trainInput.GetRowsFromIndex(batchIndex * batchSize, batchSize);
            const auto& batchExpectedOutput = trainExpectedOutput.GetRowsFromIndex(batchIndex * batchSize, batchSize);

            const auto& targets = n.Train(batchInput, batchExpectedOutput);
            auto& loss = std::get<0>(targets);
            lossAggregator.push_back(loss);

            output = QStringLiteral("\tTrain loss: %1").arg(loss);
            MainWindow::printToConsole(output);
        }
    }

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

/* Private functions */
private:
    QString openFileDialog(const QString &message, const QString &path, const QString &filter);

/* Slots */
public slots:
    void trainInputButtonClicked();
    void trainExpectedOutputButtonClicked();
    void testInputButtonClicked();
    void testExpectedOutputButtonClicked();
    void saveOutputButtonClicked();

    void addLayerButtonClicked();
    void runButtonClicked();
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
