#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int layersCount;

public slots:
    void addLayerButtonClicked();
    void trainButtonClicked();

    void layerWidgetHovered();
    void layerWidgetHoverEnded();
    void layerWidgetMousePressed();
    void layerWidgetMouseReleased();
    void layerWidgetMoveUpButtonClicked();
    void layerWidgetMoveDownButtonClicked();
    void layerWidgetRemoveButtonClicked();
};

#endif // MAINWINDOW_H
