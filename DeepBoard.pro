#-------------------------------------------------
#
# Project created by QtCreator 2017-04-16T19:48:42
#
#-------------------------------------------------

CONFIG += c++14
QMAKE_CXXFLAGS += -Wno-unicode-whitespace

QT       += core gui

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
    layerwidget.cpp

HEADERS  += mainwindow.h \
    layerwidget.h \
    ../DeepEngine/DeepEngine/Functions/ActivationFunctions/ActivationFunctionInterface.h \
    ../DeepEngine/DeepEngine/Functions/ActivationFunctions/IdentityActivationFunction.h \
    ../DeepEngine/DeepEngine/Functions/ActivationFunctions/ReluActivationFunction.h \
    ../DeepEngine/DeepEngine/Functions/ActivationFunctions/SigmoidActivationFunction.h \
    ../DeepEngine/DeepEngine/Functions/ActivationFunctions/TanhActivationFunction.h \
    ../DeepEngine/DeepEngine/Functions/LossFunctions/LossFunctionInterface.h \
    ../DeepEngine/DeepEngine/Functions/LossFunctions/MSELossFunction.h \
    ../DeepEngine/DeepEngine/Layers/IdentityLayer.h \
    ../DeepEngine/DeepEngine/Layers/InputLayer.h \
    ../DeepEngine/DeepEngine/Layers/LayerInterface.h \
    ../DeepEngine/DeepEngine/Layers/ReluLayer.h \
    ../DeepEngine/DeepEngine/Layers/SigmoidLayer.h \
    ../DeepEngine/DeepEngine/Layers/TanhLayer.h \
    ../DeepEngine/DeepEngine/Matrix.h \
    ../DeepEngine/DeepEngine/Network.h \
    ../DeepEngine/DeepEngine/Vector.h

FORMS    += mainwindow.ui \
    layerwidget.ui
