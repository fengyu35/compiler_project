#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::on_btnSimGram_clicked() {
    currState = GrammarSimplify;
    setGramAns();
}

void MainWindow::on_btnRmRecursion_clicked() {
    currState = removeLeftRecursive;
    setGramAns();
}

void MainWindow::on_btnRmFactor_clicked() {
    currState = removeLeftCommonFactor;
    setGramAns();
}

void MainWindow::on_btnFirstFollow_clicked() {
    currState = FirstFollow;
    clearAnswer(ui->gridLayout_GramAnal);
    QString tmp = ui->inputText->toPlainText();
    if(tmp.isEmpty()){
        QMessageBox::information(this,"解析文本为空", "请输入文本");
        return;
    }
    // 布局
    mLabelResGram = new QLabel("First Follow 集合");
    mAnsTable2 = new QTableWidget();
    mAnsTable2->setFont(EnglishFont);
    ui->gridLayout_GramAnal->addWidget(ui->GroupGrammar, 0, 0, 1, 4);
    ui->gridLayout_GramAnal->addWidget(mLabelResGram, 1, 0, 1, 4);
    ui->gridLayout_GramAnal->addWidget(mAnsTable2, 2, 0, 1, 4);
    mAnsTable2->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 单元格内容不可编辑

    if(mQues02.parseStrToGrammar(tmp)){
        mQues02.Run(currState);
        mGrammars = mQues02.getGrammars();
    } else{
        QMessageBox::information(this,"解析文本错误", "请输入正确的语法格式");
        return;
    }

    QStringList header;
    header << " First 集合 " << " Follow 集合 ";        // 设置列标签
    mAnsTable2->setColumnCount(2);                    // 设定列的数量 2
    mAnsTable2->setHorizontalHeaderLabels(header);    // 设定列名
    header = mQues02.getNonTerms();                     // 设置行标签
    mAnsTable2->setRowCount(header.size());             // 设定行的数量
    mAnsTable2->setVerticalHeaderLabels(header);        // 设置行名称

    // 逐行更新item
    size_t row = 0;
    QString res;
    QSet<QString> f; // 缓存 first 和 follow 集合
    for(const auto & leftVn: mGrammars.keys()) {
        // 第 0 列 first 集合
        f = mGrammars[leftVn].first;
        QString res = QStringList(f.toList()).join(", ");
        mAnsTable2->setItem(row, 0, new QTableWidgetItem(res));

        // 第 1 列 follow 集合
        f = mGrammars[leftVn].follow;
        res = QStringList(f.toList()).join(", ");
        mAnsTable2->setItem(row, 1, new QTableWidgetItem(res));
        row++;
    }
    mAnsTable2->resizeColumnsToContents();// 根据内容来确定列宽度
}

void MainWindow::on_btnLL1_clicked() {
    currState = LLtable;
    QString tmp = ui->inputText->toPlainText();
    clearAnswer(ui->gridLayout_GramAnal);
    if(tmp.isEmpty()){
        QMessageBox::information(this,"解析文本为空", "请输入文本");
        return;
    }
    if(mQues02.parseStrToGrammar(tmp)){
        mQues02.Run(currState);
        mGrammars = mQues02.getGrammars();
        mAnalyTable = mQues02.getAnalyTable();
    } else{
        QMessageBox::information(this,"解析文本错误", "请输入正确的语法格式");
        return;
    }
    QSet<QString> mTerms = mQues02.getTerms();
    mTerms.insert(stackBottom);  // 分析表需要额外添加

    // 布局
    mLabelResGram = new QLabel("LL1 分析表");
    mAnsTable2 = new QTableWidget();
    mAnsTable2->setFont(EnglishFont);
    ui->gridLayout_GramAnal->addWidget(ui->GroupGrammar, 0, 0, 1, 4);
    ui->gridLayout_GramAnal->addWidget(mLabelResGram, 1, 0, 1, 4);
    ui->gridLayout_GramAnal->addWidget(mAnsTable2, 2, 0, 1, 4);

    // 设置行列标签
    QStringList header = {stackBottom};
    for(const auto& Vt: mTerms)
        header << Vt;
    mAnsTable2->setColumnCount(header.size());        // 设定列的数量
    mAnsTable2->setHorizontalHeaderLabels(header);    // 设定列名
    header = mQues02.getNonTerms();                     // 设置行标签
    mAnsTable2->setRowCount(header.size());             // 设定行的数量
    mAnsTable2->setVerticalHeaderLabels(header);        // 设置行名称
    mAnsTable2->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 单元格内容不可编辑

    int row = -1;
    QSet<QString> f; // 缓存 first 和 follow 集合
    for(const auto & leftVn: mGrammars.keys()){
        row++;
        // 产生式
        int col = 0;
        // 第一列 栈底元素
        auto& prod = mAnalyTable[leftVn][stackBottom];
        QString res = prod.join(" ");
        QTableWidgetItem* tmpItem = new QTableWidgetItem(res);
        if(prod.size()==1 && prod.front() == ERRORstr){
            tmpItem->setText("");
//            tmpItem->setTextColor(QColor::fromRgb(200, 192, 203));  // 灰色
        }
        mAnsTable2->setItem(row, 0, tmpItem);

        for(const auto& Vt:mTerms){
            col++;
            prod = mAnalyTable[leftVn][Vt];
            QString res = prod.join(" ");
            QTableWidgetItem* tmpItem = new QTableWidgetItem(res);
            if(prod.size()==1 && prod.front() == ERRORstr){
                tmpItem->setText("");
//                tmpItem->setTextColor(QColor::fromRgb(200, 192, 203));  // 灰色
            }
            mAnsTable2->setItem(row, col, tmpItem);
        }
    }
    mAnsTable2->resizeColumnsToContents();// 根据内容来确定列宽度
}

void MainWindow::on_btnTreeAnal_clicked() {
    currState = GrammarTree;
    clearAnswer(ui->gridLayout_GramAnal);

    // 布局
    QPushButton* btnOpenLexFile = new QPushButton("打开词法分析结果文件");
    QPushButton* btnTreeShow = new QPushButton("显示分析结果");
    mLabelResGram = new QLabel("输入词法分析结果");
    QLabel* titleLabel = new QLabel("语法树");
    mLexInput = new QTextEdit();
    mLabelResGram->setFont(ChineseFont);

    btnOpenLexFile->setFont(ChineseFont);
    btnTreeShow->setFont(ChineseFont);
    titleLabel->setFont(ChineseFont);
    mLexInput->setFont(EnglishFont);
    ui->gridLayout_GramAnal->addWidget(ui->GroupGrammar, 0, 0, 1, 4);

    ui->gridLayout_GramAnal->addWidget(mLabelResGram, 1, 0, 1, 1);
    ui->gridLayout_GramAnal->addWidget(btnOpenLexFile, 1, 1, 1, 1);
    ui->gridLayout_GramAnal->addWidget(titleLabel, 1, 2, 1, 1);
    ui->gridLayout_GramAnal->addWidget(btnTreeShow, 1, 3, 1, 1);

    ui->gridLayout_GramAnal->addWidget(mLexInput, 2, 0, 1, 2);


//   绑定按钮点击信号的槽函数
    connect(btnOpenLexFile, SIGNAL(clicked()), this, SLOT(openLexFileAnal()));
    connect(btnTreeShow, SIGNAL(clicked()), this, SLOT(showTreeAnal()));

}

void MainWindow::openLexFileAnal() {
    QString fileName = QFileDialog::getOpenFileName(this, "打开 词法分析结果 文件", "../test_data", "");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly)){
            mLexInput->setText(file.readAll()); // 显示 词法分析结果
            file.close();
        } else
            QMessageBox::information(this, "Failed to Read File!", "Maybe the file not exist!");
    } else
        QMessageBox::information(this, "Failed to Open File!", "File name is Empty!");
}

void MainWindow::showTreeAnal() {
    QString tmpGrammar = ui->inputText->toPlainText();
    QString tmpTokens = mLexInput->toPlainText();
    if(tmpGrammar.isEmpty()){
        QMessageBox::information(this," 输入语法文本 为空", "请输入语法");
        return;
    }
    if(tmpTokens.isEmpty()){
        QMessageBox::information(this," 词法分析结果文本 为空", "请输入 词法分析结果！");
        return;
    }

    // 运行
    if(!mQues02.parseStrToGrammar(tmpGrammar)){
        QMessageBox::information(this,"解析文本错误", "请输入正确的语法格式");
        return;
    }
    mQues02.Run(currState, tmpTokens);
    QTreeWidget* treeGram;
    treeGram = new QTreeWidget();
    ui->gridLayout_GramAnal->addWidget(treeGram, 2, 2, 1, 2);
    treeGram->setColumnCount(2);             // 设定列数量
    treeGram->setHeaderLabels(QStringList() << "type" << "content"); // 设置列名称
    treeGram->setFont(EnglishFont);
    TokenNode* root = mQues02.getRoot();

    if(root == nullptr){
        QMessageBox::information(this,"语法树构建失败", "");
        return;
    }

    // 显示结果
    QTreeWidgetItem* rootItem = new QTreeWidgetItem(treeGram);
    rootItem->setText(0,root->type);
    rootItem->setText(1,root->content);
    treeGram->addTopLevelItem(rootItem);

//     使用队列 BFS 遍历树节点
    QQueue<TokenNode*> queueToken;
    QQueue<QTreeWidgetItem*> queueItem;
    queueToken.enqueue(root);
    queueItem.enqueue(rootItem);

    while(!queueToken.empty()){
        TokenNode* currTokenNode = queueToken.dequeue();
        QTreeWidgetItem* currTreeItem = queueItem.dequeue();

        currTreeItem->setText(0,currTokenNode->type);
        currTreeItem->setText(1,currTokenNode->content);

        if (!currTokenNode->child.empty()) {
            // 倒序 显示，因为语法树的构建是倒序的
            for (auto iter = currTokenNode->child.rbegin(); iter != currTokenNode->child.rend(); ++iter) {
                TokenNode* c = *iter;
                queueToken.enqueue(c);
                QTreeWidgetItem* tmp = new QTreeWidgetItem(currTreeItem);
                queueItem.enqueue(tmp);
            }
        }
    }
    treeGram->expandAll();
    treeGram->header()->setSectionResizeMode(QHeaderView::ResizeToContents);// 根据内容来确定列宽度
}

void MainWindow::setGramAns() {
    QString tmp = ui->inputText->toPlainText();
    if(tmp.isEmpty()){
        QMessageBox::information(this,"解析文本为空", "请输入文本");
        return;
    }
    // 运行
    if(mQues02.parseStrToGrammar(tmp)){
        mQues02.Run(currState);
    } else{
        QMessageBox::information(this,"解析文本错误", "请输入正确的语法格式");
        return;
    }

    // 布局
    clearAnswer(ui->gridLayout_GramAnal);
    mResGrammar = new QListWidget();
    mLabelResGram = new QLabel(getStateStr());
    mResGrammar->setFont(EnglishFont);
    mLabelResGram->setFont(ChineseFont);
    ui->gridLayout_GramAnal->addWidget(ui->GroupGrammar, 0, 0, 1, 4);
    ui->gridLayout_GramAnal->addWidget(mLabelResGram, 1, 0, 1, 4);
    ui->gridLayout_GramAnal->addWidget(mResGrammar, 2, 0, 1, 4);

    // 显示结果
    for(const auto & leftVn : mQues02.getNonTerms())
        mResGrammar->addItem(mQues02.toGramString(leftVn));

}
