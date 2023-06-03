#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QMessageBox>
#include <QDebug>
#include <QHeaderView>
#include <QTableWidget>
#include <QTreeWidget>
#include <QFileDialog>
#include <QProcess>
#include <QTemporaryFile>
#include <QDesktopServices>
#include <QGridLayout>
#include <QListWidget>
#include "WordAnal.h"
#include "GramAnal.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
      // 词法分析
      void showGraph();           // 显示状态转换图
      void getProgram();          // 获取程序代码
      void runProgram();          // 运行程序
      void openEncoding();        // 打开文件编码
      void saveEncoding();        // 保存文件编码
      void on_btnSelectFile_clicked();    // 点击选择文件按钮
      void on_btnSaveFile_clicked();      // 点击保存文件按钮
      void on_btnNfa_clicked();           // 点击 NFA 按钮
      void on_btnDfa_clicked();           // 点击 DFA 按钮
      void on_btnSdfa_clicked();          // 点击简化 DFA 按钮
      void on_btnWordAnal_clicked();      // 点击词法分析按钮

      // 语法分析
      void on_btnSimGram_clicked();       // 点击消除无用符号按钮
      void on_btnRmRecursion_clicked();   // 点击消除左递归按钮
      void on_btnRmFactor_clicked();     // 点击消除左公因子按钮
      void on_btnFirstFollow_clicked();   // 点击求 First Follow 集按钮
      void on_btnLL1_clicked();           // 点击 LL(1) 分析按钮
      void on_btnTreeAnal_clicked();    // 点击 分析树构建 按钮
      void openLexFileAnal();           // 打开 词法分析结果 文件
      void showTreeAnal();             // 显示 语法树分析结果
private:
    Ui::MainWindow *ui;
    WindowState currState; // 当前窗口状态
    QFont ChineseFont; // 中文字体
    QFont EnglishFont; // 英文字体

    void clearAnswer(QGridLayout*);// 清空答案区域

    // 问题1 词法分析
    // 函数
    void resetQues01_TableLayout();
    void resetQues01_SrcCodeLayout();
    QByteArray genDotFile(); // 生成图片所需的dot文件
    void setTable(); // 生成表格视图的答案

    // NFA DFA SDFA: 三种有限状态自动机
    WordAnal mQues01; // 问题1 对象
    QString getStateStr(); // 获取状态字符串的函数
    set<QString> mTransChars; // 状态转移字符集合
    vector<State*> mNodes; // 状态节点集合

    // 问题1  界面元素
    QLabel* mTitle; // 标题
    QTableWidget* mAnsTable; // 答案表格
    QPushButton* mBtnGraph; // 绘制图形按钮
    QLabel* mLabelTestCode; // 测试代码标签
    QLabel* mLabelEncoding; // 单词编码标签
    QTextEdit* mProgram; // 源程序文本
    QTextEdit* mTestCode; // 测试代码文本
    QTextEdit* mEncoding; // 单词编码文本
    QPushButton* mBtnGenProgram;// 生成源程序按钮
    QPushButton* mBtnOpenTestCode;// 打开测试代码按钮
    QPushButton* mBtnRunProgram;// 运行源程序按钮
    QPushButton* mBtnSaveEncoding;// 保存编码按钮

    // 问题2 语法分析 变量
    GramAnal mQues02;  // 问题2 对象
    QMap<QString, Grammar> mGrammars; // 语法
    QMap<QString,QMap<QString,QStringList>>mAnalyTable; // 分析表

    // 函数
    void setGramAns();// 设置 文法列表视图 答案界面
    void setTreeAns();// 设置 树形结构视图 答案界面

    // 问题2  界面元素
    QLabel* mLabelResGram; // 标题 结果文法
    QTableWidget* mAnsTable2; // 答案表格
    QListWidget* mResGrammar;// 结果文法显示
    QPushButton* mRunLL1;// 运行 LL1 分析按钮
    QTextEdit* mLexInput; // 词法分析结果输入
};
#endif // MAINWINDOW_H
