#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 设置标签页名称
    ui->Answer->setTabText(0,"词法分析");
    ui->Answer->setTabText(1,"语法分析");
    // 设置中英文字体 华文细宋 11 consolas 10
    ChineseFont.setFamily("STZhongsong");
    ChineseFont.setPointSize(11);
    EnglishFont.setFamily("consolas");
    EnglishFont.setPointSize(10);
}

MainWindow::~MainWindow() {
    delete ui;
}

// 清空答案区域，除了按钮区域
void MainWindow::clearAnswer(QGridLayout* Answer) {
    QLayoutItem *child;
    while ((child = Answer->takeAt(1)) != nullptr) {
        delete child->widget();
        delete child;
    }
    // 清空变量
    if(Answer == ui->gridLayout_WordAnal){
        mTransChars.clear();
        mNodes.clear();
    }
    else if(Answer == ui->gridLayout_GramAnal){
        mGrammars.clear();
        mAnalyTable.clear();
    }
}

void MainWindow::on_btnSelectFile_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "打开文本文件", "../test_data", "");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly)){
            ui->inputText->setText(file.readAll());
            file.close();
        } else
            QMessageBox::information(this, "Failed to Read File!", "Maybe the file not exist!");
    } else
        QMessageBox::information(this, "Failed to Open File!", "File name is Empty!");
}

void MainWindow::on_btnSaveFile_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "把文本保存至文件", "../test_data", "");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        QByteArray text = ui->inputText->toPlainText().toUtf8();
        if (file.open(QIODevice::WriteOnly)) {
            file.write(text, text.length());
            file.close();
            QMessageBox::information(this, "File Saved!", "File has been saved successfully!");
        } else {
            QMessageBox::information(this, "Failed to Save File!", "Failed to open file for writing!");
        }
    } else {
        QMessageBox::information(this, "Failed to Save File!", "File name is Empty!");
    }
}

QString MainWindow::getStateStr() {
    switch (currState) {
        case nothing:
            return "Nothing";
        case nfa:
            return "NFA";
        case dfa:
            return "DFA";
        case sdfa:
            return "SDFA";
        case GENprogram:
            return "genProgram";
        case RUNprogram:
            return "runProgram";
        case GrammarSimplify:
            return "文法化简";
        case removeLeftRecursive:
            return "消除左递归";
        case removeLeftCommonFactor:
            return "消除左公因子";
        case FirstFollow:
            return "FirstFollow";
        case LLtable:
            return "LLanal";
        case GrammarTree:
            return "GrammarTree";
    }
    return "";
}
