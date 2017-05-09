#-------------------------------------------------
#
# Project created by QtCreator 2017-04-16T19:48:42
#
#-------------------------------------------------

CONFIG += c++14
QMAKE_CXXFLAGS += -Wno-unicode-whitespace

QT += core gui
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DeepBoard
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    layerwidget.cpp \
    ./DeepEngine-0.2.0/Functions/ActivationFunctions/ActivationFunctionInterface.h \
    ./DeepEngine-0.2.0/Functions/ActivationFunctions/IdentityActivationFunction.h \
    ./DeepEngine-0.2.0/Functions/ActivationFunctions/ReluActivationFunction.h \
    ./DeepEngine-0.2.0/Functions/ActivationFunctions/SigmoidActivationFunction.h \
    ./DeepEngine-0.2.0/Functions/ActivationFunctions/TanhActivationFunction.h \
    ./DeepEngine-0.2.0/Functions/LossFunctions/LossFunctionInterface.h \
    ./DeepEngine-0.2.0/Functions/LossFunctions/MSELossFunction.h \
    ./DeepEngine-0.2.0/Layers/IdentityLayer.h \
    ./DeepEngine-0.2.0/Layers/InputLayer.h \
    ./DeepEngine-0.2.0/Layers/LayerInterface.h \
    ./DeepEngine-0.2.0/Layers/ReluLayer.h \
    ./DeepEngine-0.2.0/Layers/SigmoidLayer.h \
    ./DeepEngine-0.2.0/Layers/TanhLayer.h \
    ./DeepEngine-0.2.0/Matrix.h \
    ./DeepEngine-0.2.0/Network.h \
    ./DeepEngine-0.2.0/Vector.h


HEADERS  += mainwindow.h \
    layerwidget.h

FORMS    += mainwindow.ui \
    layerwidget.ui
