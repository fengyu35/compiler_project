#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::on_btnNfa_clicked() {
    currState = nfa;
    resetQues01_TableLayout();
    vector<NFAState> nfas = mQues01.getNFAstates();
    if(nfas.size() < 1){
        QMessageBox::information(this,"解析文本错误","得到的 NFA 数组为空");
        return;
    }
    for(auto & nfa:nfas)
        mNodes.push_back(&nfa);
    setTable();// 设置表格数据
}

void MainWindow::on_btnDfa_clicked() {
    currState = dfa;
    resetQues01_TableLayout();
    vector<DFAState> dfas = mQues01.getDFAstates();
    if(dfas.empty()){
        QMessageBox::information(this,"解析文本错误","得到的 DFA 数组为空");
        return;
    }
    for(auto & dfa:dfas)
        mNodes.push_back(&dfa);
    setTable();// 设置表格数据
}

void MainWindow::on_btnSdfa_clicked() {
    currState = sdfa;
    resetQues01_TableLayout();
    vector<SDFAState> sdfas = mQues01.getSDFAstates();
    if(sdfas.empty()){
        QMessageBox::information(this,"解析文本错误","得到的 SDFA 数组为空");
        return;
    }
    for(auto & sdfa:sdfas)
        mNodes.push_back(&sdfa);
    setTable();// 设置表格数据
}

void MainWindow::on_btnWordAnal_clicked(){
    currState = nothing;
    resetQues01_SrcCodeLayout();
}

// 重新设置 WordAnal 的表格布局
void MainWindow::resetQues01_TableLayout() {
    clearAnswer(ui->gridLayout_WordAnal);
    // 生成 状态转换
    mQues01.parseExpressions(ui->inputText->toPlainText(),currState);
    mTransChars = mQues01.getTransChar();
    // 新增 label 生成转换图按钮 和 表格视图
    mTitle = new QLabel(QString("%1状态转换表：初态(绿)/终态(红)/初终态(黄)").arg(getStateStr()));
    mBtnGraph = new QPushButton(QString("生成%1转换图").arg(getStateStr()));
    mAnsTable = new QTableWidget();
    // 设置字体
    mTitle->setFont(ChineseFont);
    mBtnGraph->setFont(ChineseFont);

    // 绑定信号和槽函数，展示图片
    connect(mBtnGraph, SIGNAL(clicked()), this,SLOT(showGraph()));

    // 设置布局
    ui->gridLayout_WordAnal->addWidget(ui->GroupWord, 0, 0, 1, 7);
    ui->gridLayout_WordAnal->addWidget(mTitle, 1, 0);
    ui->gridLayout_WordAnal->addWidget(mBtnGraph, 1, 3, 1, 4);
    ui->gridLayout_WordAnal->addWidget(mAnsTable, 2, 0, 1, 7);

    // 设置行和列的伸展因子
    ui->gridLayout_WordAnal->setRowStretch(0, 0);
    ui->gridLayout_WordAnal->setRowStretch(1, 0);
    ui->gridLayout_WordAnal->setRowStretch(2, 1);
}

// 重新设置 WordAnal 的表格布局
void MainWindow::resetQues01_SrcCodeLayout() {
    clearAnswer(ui->gridLayout_WordAnal);
    // 新增标签、文本编辑器和按钮
    mTitle = new QLabel("源程序");
    mLabelTestCode = new QLabel("输入测试代码");
    mLabelEncoding = new QLabel("单词编码输出");
    mBtnGenProgram = new QPushButton("生成源程序");
    mBtnRunProgram = new QPushButton("运行源程序");
    mBtnOpenTestCode = new QPushButton("打开测试代码");
    mBtnSaveEncoding = new QPushButton("保存单词编码");
    mProgram = new QTextEdit();
    mTestCode = new QTextEdit();
    mEncoding = new QTextEdit();
    // 设置字体
    mTitle->setFont(ChineseFont);
    mLabelTestCode->setFont(ChineseFont);
    mLabelEncoding->setFont(ChineseFont);
    mBtnGenProgram->setFont(ChineseFont);
    mBtnRunProgram->setFont(ChineseFont);
    mBtnOpenTestCode->setFont(ChineseFont);
    mBtnSaveEncoding->setFont(ChineseFont);
    mProgram->setFont(EnglishFont);
    mTestCode->setFont(EnglishFont);
    mEncoding->setFont(EnglishFont);

    // 绑定信号和槽函数
    connect(mBtnGenProgram, SIGNAL(clicked()), this, SLOT(getProgram()));
    connect(mBtnRunProgram, SIGNAL(clicked()), this, SLOT(runProgram()));
    connect(mBtnOpenTestCode, SIGNAL(clicked()), this, SLOT(openEncoding()));
    connect(mBtnSaveEncoding, SIGNAL(clicked()), this, SLOT(saveEncoding()));
    // 设置按钮不可点击
    mBtnRunProgram->setEnabled(false);
    mBtnSaveEncoding->setEnabled(false);

    // 设置布局
    ui->gridLayout_WordAnal->addWidget(ui->GroupWord, 0, 0, 1, 7);

    ui->gridLayout_WordAnal->addWidget(mTitle, 1, 0);
    ui->gridLayout_WordAnal->addWidget(mBtnGenProgram, 1, 1);
    ui->gridLayout_WordAnal->addWidget(mLabelTestCode, 1, 2);
    ui->gridLayout_WordAnal->addWidget(mBtnOpenTestCode, 1, 3);
    ui->gridLayout_WordAnal->addWidget(mBtnRunProgram, 1, 4);
    ui->gridLayout_WordAnal->addWidget(mLabelEncoding, 1, 5);
    ui->gridLayout_WordAnal->addWidget(mBtnSaveEncoding, 1, 6);

    ui->gridLayout_WordAnal->addWidget(mProgram, 2, 0, 1, 2);
    ui->gridLayout_WordAnal->addWidget(mTestCode, 2, 2, 1, 3);
    ui->gridLayout_WordAnal->addWidget(mEncoding, 2, 5, 1, 2);

    // 设置行和列的伸展因子
    ui->gridLayout_WordAnal->setRowStretch(0, 0);
    ui->gridLayout_WordAnal->setRowStretch(1, 0);
    ui->gridLayout_WordAnal->setRowStretch(2, 1);

    ui->gridLayout_WordAnal->setColumnStretch(0, 0);
    ui->gridLayout_WordAnal->setColumnStretch(1, 1);
    ui->gridLayout_WordAnal->setColumnStretch(2, 0);
    ui->gridLayout_WordAnal->setColumnStretch(3, 0);
    ui->gridLayout_WordAnal->setColumnStretch(4, 0);
    ui->gridLayout_WordAnal->setColumnStretch(5, 0);
    ui->gridLayout_WordAnal->setColumnStretch(6, 1);
}

// 显示 WordAnal 状态转换表 的数据
void MainWindow::setTable() {
    QStringList header;
    // 设置列标签
    header << "初态/终态"<< "状态编号" << "变量名";
    for (auto & ch : mTransChars)
        if(ch[0] == '\\')
            header << ch.mid(1);
        else
            header << ch;
    mAnsTable->setColumnCount(mTransChars.size() + 3);     // 设定列的数量
    mAnsTable->setHorizontalHeaderLabels(header);    // 设定列名
    mAnsTable->verticalHeader()->setVisible(false);  // 隐藏行号
    mAnsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 单元格内容不可编辑

    //逐行更新item
    for (auto & tmp : mNodes) {
        size_t i = tmp->getStateID();
        mAnsTable->insertRow(mAnsTable->rowCount());

        //第一列 -初态(绿) / +终态(红) / +-初终态(黄)
        if (tmp->getIsStart() && tmp->getIsEnd()) {
            mAnsTable->setItem(i, 0, new QTableWidgetItem(QString("+-")));
            mAnsTable->item(i, 0)->setBackground(
                QBrush(QColor(255, 255, 0)));  //黄色
        } else if (tmp->getIsStart()) {
            mAnsTable->setItem(i, 0, new QTableWidgetItem(QString("-")));
            mAnsTable->item(i, 0)->setBackground(
                QBrush(QColor(0, 255, 0)));  //绿色
        } else if (tmp->getIsEnd()) {
            mAnsTable->setItem(i, 0, new QTableWidgetItem(QString("+")));
            mAnsTable->item(i, 0)->setBackground(
                QBrush(QColor(255, 0, 0)));  //红色
        }

        // 第二列 node 编号
        QString itemStr = QString::number(i);
        mAnsTable->setItem(i, 1, new QTableWidgetItem(itemStr));

        // 第三列 node 内容
        if(tmp->getVarName()!="")
            mAnsTable->setItem(i, 2, new QTableWidgetItem(tmp->getVarName()));

        //第四列 及之后
        int col = 3;
        for (auto & ch:mTransChars) {
            set<size_t> res = tmp->getTrans(ch);
            if (res.size())
                mAnsTable->setItem(i, col, new QTableWidgetItem(setTOstr(res)));
            col++;
        }  // end for
    }   // end for table cols
    mAnsTable->resizeColumnsToContents();// 根据内容来确定列宽度
}

void MainWindow::showGraph() {
    QDir tmpDir(QDir::currentPath() + "/tmp");
    if (!tmpDir.exists())
        tmpDir.mkpath(".");

    // 指定 DOT 暂存文件的路径和名称
    QString fileName = QDir::currentPath() + "/tmp/" + getStateStr();

    // 将 DOT 代码写入暂存文件中
    QFile dotFile(fileName + ".dot");
    if (dotFile.open(QIODevice::WriteOnly)) {
        dotFile.write(genDotFile());
        dotFile.close();
        // 调用 Graphviz 命令生成图像文件
        QString cmd = QString("dot -Gcharset=utf8 -Tsvg -o \"%1.svg\" \"%2.dot\" ").arg(fileName).arg(fileName);
        QProcess::execute(cmd);
        // 打开默认设备，显示生成的图像
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName + ".svg"));
    }
}

void MainWindow::getProgram() {
    resetQues01_SrcCodeLayout();
//    如果规则文本为空，提示输入文本内容
    if(ui->inputText->toPlainText().isEmpty()){
        QMessageBox::information(this,"解析文本错误","请输入文本内容");
        return;
    }
    // 生成状态转换
    mQues01.parseExpressions(ui->inputText->toPlainText(), currState);
    if(mQues01.getSDFAstates().empty())
        QMessageBox::information(this,"解析文本错误","得到的 SDFA 数组为空");

    QDir tmpDir(QDir::currentPath() + "/tmp");
    if (!tmpDir.exists())
        tmpDir.mkpath(".");

    QFile file(QDir::currentPath() + "/tmp/WordAnal_Program.cpp");
    QString program;
//    写入文件
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream text(&file);
        mQues01.genProgram(text);// 写入文件
        file.close();
    } else
        QMessageBox::information(this, "Failed to Save File!", "Failed to open file for writing!");

//    读取文件
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream text(&file);
        program = text.readAll();// 读取文件
        file.close();
    } else
        QMessageBox::information(this, "Failed to Save File!", "Failed to open file for writing!");
    mProgram->setText("");
    if(program != ""){
        mProgram->setText(program);
        mBtnRunProgram->setEnabled(true);
    }
    else
        QMessageBox::information(this,"解析文本错误","得到的 SDFA 数组为空");
}

void MainWindow::runProgram() {
    // 准备输入的测试文件
    QString inFileName = "Word.in";
    QString outFileName = "Word.out";
    QString inFilePath = QDir::currentPath() + "/tmp/" + inFileName;
    QString outFilePath = QDir::currentPath() + "/tmp/" + outFileName;
    QFile file(inFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream text(&file);
        text << mTestCode->toPlainText(); // 写入文件
        file.close();
    } else {
        QMessageBox::information(this, "Failed to Open Input File!", "Failed to open file for writing!");
        return;
    }

    // 编译程序
    QString programPath = QDir::currentPath() + "/tmp/WordAnal_Program.cpp";
    QString execPath = QDir::currentPath() + "/tmp/WordAnal_Program.exe";
    QProcess process1;
    process1.setWorkingDirectory(QDir::currentPath() + "/tmp");
    process1.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process1.start("g++", QStringList() << "-o" << execPath << programPath);
    if (!process1.waitForFinished()) {
        QMessageBox::information(this, "Compile Error!", "Failed to compile the program: " + process1.readAllStandardError());
        return;
    }

    // 运行程序
    QProcess process2;
    process2.setWorkingDirectory(QDir::currentPath() + "/tmp");
    process2.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
    process2.start(execPath, QStringList() << inFilePath << outFilePath);
    if (!process2.waitForFinished()) {
        QMessageBox::information(this, "Run Error!", "Failed to run the program: " + process2.errorString());
        return;
    }

    // 读入文件并显示文本
    QFile file2(outFilePath);
    if (file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream text(&file2);
        mEncoding->setText(text.readAll());
        file2.close();
        mBtnSaveEncoding->setEnabled(true);
    } else {
        QMessageBox::information(this, "Failed to Open Output File!", "Failed to open file for reading!");
        return;
    }
}

void MainWindow::saveEncoding() {
   QString fileName = QFileDialog::getSaveFileName(this, "单词编码保存至文件", "../test_data", "");
   if (!fileName.isEmpty()) {
       QFile file(fileName);
       QByteArray text = mEncoding->toPlainText().toUtf8();
       if (file.open(QIODevice::WriteOnly)) {
           file.write(text, text.length());
           file.close();
           QMessageBox::information(this, "File Saved!", "File has been saved successfully!");
       } else
           QMessageBox::information(this, "Failed to Save File!", "Failed to open file for writing!");
   } else
       QMessageBox::information(this, "Failed to Save File!", "File name is Empty!");
}

void MainWindow::openEncoding() {
    QString fileName = QFileDialog::getOpenFileName(this, "打开测试代码文件", "../test_data", "");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly)){
            mTestCode->setText(file.readAll());
            file.close();
        } else
            QMessageBox::information(this, "Failed to Read File!", "Maybe the file not exist!");
    } else
        QMessageBox::information(this, "Failed to Open File!", "File name is Empty!");
}

// 由于需要使用UTF8编码，所以返回二进制格式 QByteArray
QByteArray MainWindow::genDotFile() {
    QByteArray output;
    // 文件头部
    output += "digraph " + getStateStr() + " {\n";
    output += "\t rankdir=LR;\n ";
    output += "\t node [fontname=\"Consolas\", shape = ellipse];\n";
    output += "\t edge [fontname=\"Consolas\"];\n";

    // 添加节点
    for (int i = 0; i < mAnsTable->rowCount(); i++) {
        output += QString("\ts%1").arg(i) + " [style=filled, ";
        QTableWidgetItem *item = mAnsTable->item(i, 0);
        if(item != nullptr && !item->text().isEmpty()){
            if (item->text() == "+"){
                // 终态，红色
                output += "peripheries=2, color=red, label=\""
                        + mAnsTable->item(i, 1)->text() + "\\n"
                        +  mAnsTable->item(i, 2)->text()  + "\"];\n";
            }
            else if (item->text() == "-") // 初态，绿色
                output += "color=green, label=\"" + mAnsTable->item(i, 1)->text() + "\"];\n";
            else // 初终态，黄色
                output += "peripheries=2, color=yellow, label=\""
                        + mAnsTable->item(i, 1)->text() + "\\n"
                        + mAnsTable->item(i, 2)->text()  + "\"];\n";
        } else
            output += "label=\"" + mAnsTable->item(i, 1)->text() + "\"];\n";
    }

    // 添加边
    for (int from = 0; from < mAnsTable->rowCount(); from++)
        for (int to = 3; to < mAnsTable->columnCount(); to++)
            if (mAnsTable->item(from, to) != nullptr) {
                QString label = mAnsTable->horizontalHeaderItem(to)->text();
                QString edgeId = QString("e%1_%2").arg(from).arg(to);
                QString fromId = QString("s%1").arg(from);

                QString toSet = mAnsTable->item(from, to)->text();
                toSet = toSet.mid(1,toSet.size()-2);
                QStringList toList = toSet.split(",");
                for(auto & toText :toList ){
                    QString toId = QString("s%1").arg(toText);
                    output += QString("\t%1 -> %2 [label=\"").arg(fromId).arg(toId);
                    output += label.toUtf8() + "\"];\n";
                }
            }
    output += "}\n";
    return output;
}
