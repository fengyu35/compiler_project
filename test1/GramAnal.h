#ifndef GRAMANAL_H
#define GRAMANAL_H
#include <QSet>
#include <QDebug>
#include <QQueue>
#include <QStack>
#include "Util.h"

struct Token{
    QString content;
    QString type;
    Token(): content(""),type(""){}
    Token(const QString& _type = "", const QString &_content="")
        :content(_content), type(_type){}
};

// 语法树节点 结构体
struct TokenNode{
    QString content;  // 内容
    QString type;       // 类型
    vector<TokenNode*> child;
//    TokenNode* sibling;         // 旁系节点
    TokenNode(const QString& _type = "", const QString &_content="")
        :content(_content), type(_type), child({}){}
//    TokenNode& operator = (const TokenNode& tn) {
//        content = tn.content;
//        type = tn.type;
//        child = tn.child;
//        sibling = tn.sibling;
//        return *this;
//    }
};
// 结构体 记录 非终结符对应的 右部产生式 first
struct Grammar {
    QSet<QStringList> right; // 右部符号序列，有若干个产生式 producer
    QSet<QString> first;
    QSet<QString> follow;
    Grammar(const QSet<QStringList>& r = {})
        :right(r), first({}), follow({})  {}
};


class GramAnal {
protected:
    QString firstNonterm;   // 文法开始符号
    QSet<QString> Terms;    // 终结符集合
    QMap<QString, Grammar> grammars;// 语法规则 key 为 非终结符 ， value
    QMap<QString,QMap<QString,QStringList>> AnalyTable; // LL1 分析表
    vector<QString> order;  // 在最后输出 保证与 输入相同的顺序
    vector<Token> tokens;  // 读取的记号数组 定义
    TokenNode* root;  // 语法树根节点

    void clearArg();        // 清除上面的所有变量
    bool isTerm(const QString& symbol){return symbol != epsilon && Terms.contains(symbol);}
    bool isNonTerm(const QString& symbol){return grammars.keys().contains(symbol);}


    void updateGrammars(const QSet<QString>& removeVn); // 更新语法

    bool hasAllTermProd(const Grammar& grammar); //
    bool isThisVnRecursiveProd(const QString& Vn, const QStringList& prod); // 判断产生式是否为左递归
    void rmHarmfulProd();   // 删除有害产生式
    void rmNoArriveGram();  // 删除不可到达的文法
    void rmNoStopGram();    // 删除不能停止的文法

    void rmLeftRecursive();//    消除间接左递归
    bool rmLeftDirectRecersive(const QString& Vn);//    消除直接左递归

    void rmLeftCommonFactor();  // 消除间接左公因子
    void rmLeftDirectCommonFactor();  // 消除直接左公因子

    void genFirst(); // 生成first集合
    void genFollow(); // 生成 follow 集合

    QSet<QString> getProdFirst(const QStringList& prod); // 产生式的 first 集合WW
    void genLLtable(); // 生成 LL1 分析表
    bool LL1(); // 执行 LL1 分析

public:
    GramAnal();
    ~GramAnal();
    bool parseStrToGrammar (const QString& grammars);  // 分解字符串
    bool Run(WindowState state, const QString strToken = "");

    bool setTokens(const QString strToken);
    QString toGramString(const QString& Vn) const;
    QMap<QString, Grammar> getGrammars() const {return grammars;}
    QMap<QString,QMap<QString,QStringList>> getAnalyTable() const {return AnalyTable;}
    QSet<QString> getTerms() const {return Terms;}
    QStringList getNonTerms() const {return grammars.keys();}
    TokenNode* getRoot() const {return root;}

    // test
    void debugGram(const QString& hint = "test: "){
        qDebug() << hint << "==============\n grammars: \n";
        for(const auto& left:  grammars.keys()){
            qDebug() << toGramString(left);
        }
    }
    void debugTerm(const QString& hint = "test: "){
        qDebug() << "\t\t" << hint << "==============\nTerms: " << Terms;
    }
    void debugNonTerm(const QString& hint = "test: "){
        qDebug() << "\t\t" << hint << "===============\nNonTerms: " << grammars.keys();
    }
    void debugFirst(const QString& hint = "test: "){
        qDebug() << "\t\t" << hint << "===============\nFirst: ";
        for(const auto& Vn: grammars.keys()){
            qDebug() << Vn << grammars[Vn].first;
        }
    }
    void debugFollow(const QString& hint = "test: "){
        qDebug() << "\t\t" << hint << "===============\nFollow: ";
        for(const auto& Vn: grammars.keys()){
            qDebug() << Vn << grammars[Vn].follow;
        }
    }
};

#endif // GRAMANAL_H
