QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GramAnal.cpp \
    GramAnal1_SimGram.cpp \
    GramAnal2_LeftRecursive.cpp \
    GramAnal3_ComFactor.cpp \
    GramAnal4_FirstFollow.cpp \
    GramAnal5_LL1.cpp \
    Util.cpp \
    WordAnal.cpp \
    WordAnal1_postfix.cpp \
    WordAnal2_nfa.cpp \
    WordAnal3_dfa.cpp \
    WordAnal4_sdfa.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow_ques1.cpp \
    mainwindow_ques2.cpp

HEADERS += \
    BaseXFA.h \
    GramAnal.h \
    Util.h \
    WordAnal.h \
    mainwindow.h

FORMS += \
    mainwindow.ui
