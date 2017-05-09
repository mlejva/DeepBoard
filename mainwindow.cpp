#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "layerwidget.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QString>
#include <QScrollBar>
#include <QPushButton>
#include <algorithm>

#include "../DeepEngine/DeepEngine/Network.h"
#include "../DeepEngine/DeepEngine/Functions/LossFunctions/MSELossFunction.h"

typedef Functions::MSELossFunction<double> MSEDouble;
typedef Functions::MSELossFunction<float> MSEFloat;

/* Constructors & Destructor */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    layersCount = 0;
    static_cast<QVBoxLayout*>(ui->editorScrollAreaWidgetContents->layout())->setSpacing(10);

    connect(ui->trainInputButton, SIGNAL(clicked()), this, SLOT(trainInputButtonClicked()));
    connect(ui->trainExpectedOutputButton, SIGNAL(clicked()), this, SLOT(trainExpectedOutputButtonClicked()));
    connect(ui->testInputButton, SIGNAL(clicked()), this, SLOT(testInputButtonClicked()));
    connect(ui->testExpectedOutputButton, SIGNAL(clicked()), this, SLOT(testExpectedOutputButtonClicked()));
    connect(ui->saveOutputButton, SIGNAL(clicked()), this, SLOT(saveOutputButtonClicked()));    

    connect(ui->addLayerButton, SIGNAL(clicked()), this, SLOT(addLayerButtonClicked()));
    connect(ui->runButton, SIGNAL(clicked()), this, SLOT(runButtonClicked()));
    connect(ui->clearConsoleButton, SIGNAL(clicked()), this, SLOT(clearConsoleButtonClicked()));    
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* Public functions */
void MainWindow::printToConsole(const QString &text, const QColor &c)
{    
    ui->consoleTextEdit->setTextColor(c);
    ui->consoleTextEdit->append(">> " + text);
}

/* Private functions */
QString MainWindow::openFileDialog(const QString &message, const QString &path, const QString &filter)
{
    QString fileName = QFileDialog::getOpenFileName(this, message, path, filter);
    return fileName;
}

void MainWindow::chartLoss(const std::vector<double> &lossData)
{
    // Delete chart from a previous run
    QLayoutItem *child;
    while ((child = ui->chartTab->layout()->takeAt(0)) != 0)
    {
      delete child;
    }

    QLineSeries *series = new QLineSeries();
    for (auto i = 0; i < lossData.size(); ++i)
    {
        auto& loss = lossData.at(i);
        series->append(i, loss);
    }

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Network's loss function progress");

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->chartTab->layout()->addWidget(chartView);
}

/* Slots */
void MainWindow::trainInputButtonClicked()
{
    QString selectedFile = openFileDialog("Select Train Input", QDir::currentPath(), "Text files (*.txt)");
    ui->trainInputButton->setText(selectedFile);
}

void MainWindow::trainExpectedOutputButtonClicked()
{
    QString selectedFile = openFileDialog("Select Train Expected Output", QDir::currentPath(), "Text files (*.txt)");
    ui->trainExpectedOutputButton->setText(selectedFile);
}

void MainWindow::testInputButtonClicked()
{
    QString selectedFile = openFileDialog("Select Test Input", QDir::currentPath(), "Text files (*.txt)");
    ui->testInputButton->setText(selectedFile);
}

void MainWindow::testExpectedOutputButtonClicked()
{
    QString selectedFile = openFileDialog("Select Test Expected Output", QDir::currentPath(), "Text files (*.txt)");
    ui->testExpectedOutputButton->setText(selectedFile);
}

void MainWindow::saveOutputButtonClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Where To Save Output"),
                                                 QDir::currentPath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    ui->saveOutputButton->setText(dir);
}

void MainWindow::addLayerButtonClicked()
{
    layersCount++;
    ui->runButton->setEnabled(true);

    QString name = "Layer " + QString::number(layersCount);
    LayerWidget *layer = new LayerWidget(name, layersCount - 1, this);
    layer->setObjectName(name);

    QVBoxLayout *editorLayout = static_cast<QVBoxLayout*>(ui->editorScrollAreaWidgetContents->layout());
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

void MainWindow::runButtonClicked()
{
    const QString &selectedDataTypeText = ui->dataTypeComboBox->currentText();
    const QString &selectedLossFunctionText = ui->lossFunctionComboBox->currentText();

    QList<LayerWidget*> allLayers = ui->editorScrollAreaWidgetContents->findChildren<LayerWidget*>();

    // Sort layers with their position property as the key
    std::sort(allLayers.begin(), allLayers.end(), [](auto &lhs, auto &rhs) {
        return lhs->getPosition() < rhs->getPosition();
    });    

    if (selectedDataTypeText == "Float") {
        if (selectedLossFunctionText == "Mean Squared Error") {
            auto network = MainWindow::setupNetwork<float, MSEFloat>(allLayers);

            const char &delimiter = ui->delimiterLineEdit->text().toStdString()[0];
            const Matrix<float> trainInput(ui->trainInputButton->text().toStdString(), delimiter);
            const Matrix<float> trainExpectedOutput(ui->trainExpectedOutputButton->text().toStdString(), delimiter);            

            if (ui->testAfterEpochCheckBox->isChecked())
            {
                const Matrix<float> testInput(ui->testInputButton->text().toStdString(), delimiter);
                const Matrix<float> testExpectedOutput(ui->testExpectedOutputButton->text().toStdString(), delimiter);

                const bool saveNetworkTestOutput = ui->saveOutputCheckBox->isChecked();

                QString outputPath = ui->saveOutputButton->text();
                const QString &outputFileName = ui->outputFileNameLineEdit->text();
                outputPath.append("/").append(outputFileName).append(".txt");

                MainWindow::runNetwork<float, MSEFloat>(network,
                                                        trainInput,
                                                        trainExpectedOutput,
                                                        testInput,
                                                        testExpectedOutput,
                                                        ui->batchSizeSpinBox->value(),
                                                        ui->epochCountSpinBox->value(),
                                                        saveNetworkTestOutput,
                                                        outputPath);
            }
            else
            {
                MainWindow::runNetwork<float, MSEFloat>(network,
                                                        trainInput,
                                                        trainExpectedOutput,
                                                        ui->batchSizeSpinBox->value(),
                                                        ui->epochCountSpinBox->value());
            }

        }
    }
    else if (selectedDataTypeText == "Double") {
        if (selectedLossFunctionText == "Mean Squared Error") {
            auto network = MainWindow::setupNetwork<double, MSEDouble>(allLayers);

            const char &delimiter = ui->delimiterLineEdit->text().toStdString()[0];
            const Matrix<double> trainInput(ui->trainInputButton->text().toStdString(), delimiter);
            const Matrix<double> trainExpectedOutput(ui->trainExpectedOutputButton->text().toStdString(), delimiter);

            if (ui->testAfterEpochCheckBox->isChecked())
            {
                const Matrix<double> testInput(ui->testInputButton->text().toStdString(), delimiter);
                const Matrix<double> testExpectedOutput(ui->testExpectedOutputButton->text().toStdString(), delimiter);

                const bool saveNetworkTestOutput = ui->saveOutputCheckBox->isChecked();

                QString outputPath = ui->saveOutputButton->text();
                const QString &outputFileName = ui->outputFileNameLineEdit->text();
                outputPath.append("/").append(outputFileName).append(".txt");

                MainWindow::runNetwork<double, MSEDouble>(network,
                                                         trainInput,
                                                         trainExpectedOutput,
                                                         testInput,
                                                         testExpectedOutput,
                                                         ui->batchSizeSpinBox->value(),
                                                         ui->epochCountSpinBox->value(),
                                                         saveNetworkTestOutput,
                                                         outputPath);
            }
            else
            {
                MainWindow::runNetwork<double, MSEDouble>(network,
                                                          trainInput,
                                                          trainExpectedOutput,
                                                          ui->batchSizeSpinBox->value(),
                                                          ui->epochCountSpinBox->value());
            }

        }
    }
}

void MainWindow::clearConsoleButtonClicked()
{
    ui->consoleTextEdit->clear();
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
    QVBoxLayout *editorLayout = static_cast<QVBoxLayout*>(ui->editorScrollAreaWidgetContents->layout());

    const int index = editorLayout->indexOf(layer);

    editorLayout->removeWidget(layer);
    editorLayout->insertWidget(index - 1, layer);

    const int newIndex = editorLayout->indexOf(layer);
    layer->setPosition(newIndex);

    LayerWidget *switchedLayer = static_cast<LayerWidget*>(editorLayout->itemAt(newIndex + 1)->widget());
    switchedLayer->setPosition(newIndex + 1);

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
    QVBoxLayout *editorLayout = static_cast<QVBoxLayout*>(ui->editorScrollAreaWidgetContents->layout());

    const int index = editorLayout->indexOf(layer);

    editorLayout->removeWidget(layer);
    editorLayout->insertWidget(index + 1, layer);

    const int newIndex = editorLayout->indexOf(layer);
    layer->setPosition(newIndex);

    LayerWidget *switchedLayer = static_cast<LayerWidget*>(editorLayout->itemAt(newIndex - 1)->widget());
    switchedLayer->setPosition(newIndex - 1);

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
    QVBoxLayout *editorLayout = static_cast<QVBoxLayout*>(ui->editorScrollAreaWidgetContents->layout());

    const int countBeforeRemove = editorLayout->count();
    const int removeLayerIndex = editorLayout->indexOf(layer);
    editorLayout->removeWidget(layer);
    delete layer;
    layersCount--;

    if (editorLayout->count() > 0)
    {
        // Enable/disable moving button of sorrounding layers

        // a) Removed layer wasn't first or wasn't last, i.e. was in middle
        // -> Sorrounding layers should still have enabled/disabled moving buttons as before

        // b) Removed layer was first
        if (removeLayerIndex == 0)
        {
            const int layerAfterIndex = removeLayerIndex;

            // Disable move up button of layer after
            LayerWidget *layerAfter = static_cast<LayerWidget*>(editorLayout->itemAt(layerAfterIndex)->widget());

            QPushButton *moveUpButton = layerAfter->findChild<QPushButton*>("moveUpButton");
            moveUpButton->setEnabled(false);
        }
        // c) Removed layer was last
        if (removeLayerIndex == countBeforeRemove - 1)
        {
            const int layerAboveIndex = removeLayerIndex - 1;

            // Disable move down button of layer above
            LayerWidget *layerAbove = static_cast<LayerWidget*>(editorLayout->itemAt(layerAboveIndex)->widget());

            QPushButton *moveDownButton = layerAbove->findChild<QPushButton*>("moveDownButton");
            moveDownButton->setEnabled(false);            
        }        
    }

    if (editorLayout->count() == 0) // No layers
    {
        ui->runButton->setEnabled(false);
    }
}
