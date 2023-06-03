#include "GramAnal.h"


void GramAnal::genLLtable() {
    for(const auto& leftVn : grammars.keys()){
        AnalyTable[leftVn] = {};// 分析表初始化 left 行
        for(const auto& prod : grammars[leftVn].right){ // 遍历产生式
//         1.  获得并记录产生式的 first 集合
            QSet<QString> ProdFir = getProdFirst(prod);
            for(const auto& itFir: ProdFir)
                if(itFir != epsilon)
                    AnalyTable[leftVn][itFir] = prod;
//        2.   如果 first 集合 包含 epsilon ，则 在左部的 follow 集合的元素添加 产生式
            if(ProdFir.contains(epsilon)){
                for(const auto& itFol: grammars[leftVn].follow){
                    AnalyTable[leftVn][itFol] = prod;
//                    qDebug() << QString("AnalyTable[ %1 ][ %2 ] = %3;").arg(leftVn).arg(itFol).arg(prod.join(" "));
                    // 可以在这里 强行规定 LL1 分析表某个表格的内容 ...
                }
            }
        }// each prod
//         3.  如果 对应的终结符元素 没有内容，则填入ERROR标志
        for(const auto& t: Terms)
            if(!AnalyTable[leftVn].contains(t)){
                AnalyTable[leftVn][t] = QStringList(ERRORstr);
            }
//         4.  再加一个栈底元素的出错标志
        if(!AnalyTable[leftVn].contains(stackBottom)){
            AnalyTable[leftVn][stackBottom] = QStringList(ERRORstr);
        }
    }// each grammar
//    debugTerm("genTable : ");
}
/**
 * @brief LL1 分析的总控程序
 * @return bool 分析成功 或者 失败
 * @note 该函数会基于已经构建好的分析表 AnalyTable 和输入的 Token 序列 tokens 进行分析。
 * 在分析的过程中，会根据分析表中的内容，对输入的 token 进行匹配和推导，并记录语法分析树的结构。
 * 如果分析成功，函数返回 true，否则返回 false。
*/
bool GramAnal::LL1() {
    if(AnalyTable.empty())
        return false;
    if(tokens.empty())
        return false;
    QStack<QString> analStack;     // 语法符号分析 栈
    QStack<TokenNode*> treeStack;  // 语法分析树 栈
    analStack.push(stackBottom); // 先压入  一个栈底符号
    analStack.push(firstNonterm);  // 压入 文法开始符号
    root = new TokenNode(firstNonterm,"");
    treeStack.push(root);       // 压入文法符号的根节点

    size_t readIndex = 0;       // token 数组下标
    Token readSym = tokens[readIndex++];    // 读入符号
    bool flg = true;
    while(!treeStack.empty() && !analStack.empty() && flg){
        QString expectedSym = analStack.pop();// 读入符号
        TokenNode* currNode = treeStack.pop();// 树的节点

        if(isTerm(expectedSym)){        // 如果栈顶字符是 终结符
            if(expectedSym == readSym.type){ // 如果 读入字符 和 栈顶字符 type 匹配，读入成功
                if(readIndex < tokens.size()){
                    currNode->content = readSym.content;  // 记录节点的 content
                    readSym = tokens[readIndex++]; // 读入下一个字符
                }else
                    readSym = Token(stackBottom, ""); // 达到 token 数组的末尾，添加栈底符号
            } else{  // ERROR 退出
                return false;
            }
        }
        else if(expectedSym == epsilon){
            continue;
        }
        else if(expectedSym == stackBottom){  // 如果栈顶字符是 栈底符号
            if(expectedSym == readSym.type){ // 如果 读入字符 和 栈顶字符一致 都是栈底符号，读入成功
                flg = false; // 正常结束
            } else{ // ERROR 退出
                return false;
            }
        }
        else {// 剩余的就是 非终结符
            QStringList resProd = AnalyTable[expectedSym][readSym.type];
             // 如果 分析表对应表格 存在一个 产生式
            if(resProd.empty())
                qDebug() << "ERROR TOKEN IN ANALYZE table" << expectedSym << readSym.type;
            else if(!(resProd.front() == ERRORstr && resProd.size() == 1)){
//                  记录推导过程 到语法树当中
                for(auto symbol = resProd.rbegin(); symbol != resProd.rend(); symbol++){
                    TokenNode* newNode = new TokenNode(*symbol,"");  // 设置树节点的 type
                    currNode->child.push_back(newNode);
                    treeStack.push(newNode);
                    analStack.push(*symbol);  // 倒序入栈 各个内容
                }
            }else{ // 不存在产生式 ERROR 退出
                return false;
            }
        }
    } // end while
    return true;
}

