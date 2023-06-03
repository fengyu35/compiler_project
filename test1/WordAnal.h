#ifndef XFA_H
#define XFA_H
/*
 * 文件名:xfa.h
 * 摘要：完成 NFA DFA SDFA 的功能
 * 版本号:1
 * 作者:20202132039 刘梓扬
 * 日期:2023年4月1日
 *
 * 版本历史
 * 1 2023年4月1日    初始版本
*/
#include <QDebug>
#include <iostream>
#include <string>
#include <QTextStream>

#include <vector>
#include <QStack>
#include <queue>
#include <set>
#include <map>

#include "Util.h"
#include "BaseXFA.h"
using namespace std;

class WordAnal{
private:
    set<QString> ReservedWord;  // 保留字集合
    bool IgnoreCase;        // 保留字是否忽略大小写，默认false
    QString LineCommentSign;    // 行注释符号
    QString BlockCommentBegin;  // 块注释开始符
    QString BlockCommentEnd;    // 块注释结束符
    QString varReservedWord;    // 保留字对应的变量名，从前面的varString中的一个
    set<QString> SpecialSymbol; // 特殊符号
    void setArgs(const QString& args); // 设置上面的私有变量
    void setNfaArgs();
    void clearArgs(); // 清除上面的私有变量
    bool checkArgs(); // 检查上面的私有变量
public:
    WordAnal():transChar({}),NFAstates({}),DFAstates({}),SDFAstates({}) {}
    // 把正则表达式转换为有限状态自动机
    void parseExpressions(const QString& expstring, const WindowState state);
    QStringList segment(const QString &exp); // 字符串转为token

//  postfix
private:
    set<QString> transChar;// 转移字符集
    set<QString> varString; // 变量string集

    QStringList postfix(const QStringList &exp);  //正则表达式-->后缀表达式
    QStringList AddConnectSymbol(const QStringList &exp); // 添加连接符 &
    QStringList InfixToPostfix(const QStringList &exp);// 将中缀表达式转换成后缀表达式

    size_t isp(const QString& oper);             // 栈内优先级
    size_t icp(const QString& oper);             // 栈外优先级
    bool isOperand(const QString& token);   // 是否为可以操作的字符: 大小写、数字、等号左边的变量
    bool isOperator(const QString& token);    // 是否为运算字符
    bool isUnaryOperator(const QString& token);    // 是否为单目运算符
public:
    set<QString> getTransChar() const {return transChar;}

//  NFA
private:
    vector<NFAState> NFAstates;
    set<size_t> endNFAState;
    void addNfaRule(const QStringList& tokens, const QString& varName);
    pair<size_t, size_t> CreateNFA(const QStringList &expression);    // 接收经过处理的后缀表达式，返回终态的id
    void NFAprocess(QStack<Edge>& es, const QString& ch);
    void checkEpEdge(size_t i);  // DFA之前更新各个状态空边集合
public:
    vector<NFAState> getNFAstates() const {return NFAstates;}

//  DFA
private:
    vector<DFAState> DFAstates;
    set<size_t> CreateDFA(const set<size_t> &endNFAState);
    bool isFinal(size_t stateID,const set<size_t> &endNFAState);  // 检查是否为终态
    size_t findVector(const set<size_t>& s) const;//查找是否在此数组,相当于set的find函数
    set<size_t> getNextSet(const set<size_t>& s, const QString &ch);
public:
    vector<DFAState> getDFAstates() const {return DFAstates;}

//  SDFA
private:
    vector<SDFAState> SDFAstates;
    size_t SDFAstartID; // sdfa 的开始ID，CreateSDFA 函数会赋值
    void CreateSDFA(const set<size_t>& endDFAState);
    vector<set<size_t>> createPartSet(vector<map<QString, size_t>>& trans, const set<size_t>& endDFAState);
    map<QString, set<size_t>> groupByVarName(const set<size_t>& endDFAState);
    size_t FindSet(const Edge& e, const vector<set<size_t>>& partSet);
public:
    vector<SDFAState> getSDFAstates() const {return SDFAstates;}

//    代码生成
public:
    void genProgram(QTextStream& text) const;
};

#endif // XFA_H
