#include "GramAnal.h"
/**
 * @brief 判断一个文法是否拥有只由终结符组成的产生式
 * @param grammar: 要判断的文法
 * @return bool: 如果文法中的所有产生式都只由终结符组成，则返回 true；否则返回 false。
 */
bool GramAnal::hasAllTermProd(const Grammar& grammar) {
    for(auto& prod : grammar.right){
        bool flg = true;
        for(auto & symbol : prod)
            if(isNonTerm(symbol)){ // 有一个不是 终结符
                flg = false;
                break;
            }
        if(flg)
            return true;
    }
    return false;
}

/**
 * @brief 判断给定的产生式是否为给定非终结符的递归产生式
 * @param Vn: 要查找的非终结符
 * @param prod: 要查找的产生式
 * @return bool: 如果产生式是给定非终结符的递归产生式，则返回 true；否则返回 false。
 */
bool GramAnal::isThisVnRecursiveProd(const QString& Vn, const QStringList& prod) {
    for(auto & symbol : prod)
        if(Vn == symbol)
            return true;
    return false;
}

/**
 * @brief 删除有害产生式 U -> U
 * 如果文法中含有 U -> U 的有害产生式，则删除产生式。
 */
void GramAnal::rmHarmfulProd() {
    QSet<QString> rmVn;
    for (auto& leftVn : grammars.keys()) {
        QSet<QStringList> newRight;
        for(auto& prod : grammars[leftVn].right){
            if(prod.size() == 1 && prod.front() == leftVn)
                continue;
            else newRight.insert(prod);
        }
        if(newRight.empty())
            rmVn.insert(leftVn);
    }
    updateGrammars(rmVn);// 更新非终结符集合
}

/**
 * @brief 删除无法到达的非终结符和文法
 * @details
 * 1. 首先，创建一个集合 visited，并将起始非终结符 firstNonterm 加入其中，表示该非终结符可以到达。
 * 2. 创建一个队列 queue，并将起始非终结符 firstNonterm 的文法索引加入其中，表示从该非终结符开始搜索。
 * 3. 进入循环，当队列非空时，取出队列顶部的文法索引，并遍历该文法的所有产生式。
 * 4. 对于每个产生式，遍历其中的所有符号。如果符号是一个未访问过的非终结符，则将其加入 visited 集合和队列中。
 * 重复执行步骤 3 到步骤 4，直到队列为空为止。
 * 5. 遍历所有的文法，如果一个文法的左部符号没有被访问过，则删除该文法。
 * 6. 如果删除了文法，则使用 updateGrammars() 函数更新所有文法的左部符号和非终结符的集合。
 *
 * @warning 调用前需要先执行 rmHarmfulProd() 去除有害规则
 */
void GramAnal::rmNoArriveGram() {
//    debugGram("NoArrive() Before Update: ");
    QSet<QString> visited = {firstNonterm}; // 记录访问过的非终结符
    QQueue<QString> queue;// 广度优先搜索
    QSet<QString> clear;// 需要清除的语法
    queue.enqueue(firstNonterm);

//  将已经访问的左部的产生式中  所有未访问的非终结符 添加到访问集合和队列
    while (!queue.empty()) {
        QString currVn = queue.dequeue();
        for (const auto& prod : grammars[currVn].right)
            for (const auto& symbol : prod) { // 遍历 当前非终结符 的右部所有符号
                // 如果一个右部符号是非终结符并且没有被访问过
                if (isNonTerm(symbol) && !visited.contains(symbol)) {
//                    qDebug() << currVn << prod;
//                    qDebug() << symbol <<" enqueue!!! ";
                    visited.insert(symbol);// 将它加入队列和集合中
                    queue.enqueue(symbol);
                }
            }
    }
//    qDebug() <<" visited: " << visited;
    // 删除所有无法到达的非终结符的文法
    for (const auto& leftVn: grammars.keys())
        if (!visited.contains(leftVn)) // 如果这个文法的左部符号没有被访问过
            clear.insert(leftVn);
//    qDebug() <<" clear: " << clear;
    updateGrammars(clear);
//    debugGram("NoArrive() After Update: ");
}

/**
 * @brief 删除无法推导至终结符串的非终结符和文法
 * @details
 * 1. 首先，创建一个集合 visited，并将所有可以推导至终结符的非终结符加入其中，表示这些非终结符可以到达终结符。
 * 2. 创建一个队列 queue，并将所有可以推导至终结符的非终结符的文法索引加入其中，表示从这些非终结符开始搜索。
 * 3. 进入循环，当队列非空时，取出队列顶部的文法索引，并遍历该文法的所有产生式。
 * 4. 对于每个产生式，遍历其中的所有符号。如果符号是一个未访问过的非终结符，则将其加入 visited 集合和队列中。
 * 重复执行步骤 3 到步骤 4，直到队列为空为止。
 * 5. 遍历所有的文法，如果一个文法的左部符号没有被访问过，则删除该文法。
 * 6. 如果删除了文法，则使用 updateGrammars() 函数更新所有文法的左部符号和非终结符的集合。
 *
 * @warning 调用前需要先执行 rmHarmfulProd() 去除有害规则
 */
void GramAnal::rmNoStopGram() {
    QSet<QString> visited; // 记录访问过的非终结符
    QQueue<QString> queue;// 广度优先搜索
    QSet<QString> clear;// 需要清除的语法 Vn

    // 所有可以推导至终结符的非终结符加入其中
    for(const auto& leftVn: grammars.keys())
        if(hasAllTermProd(grammars[leftVn])){
            visited.insert(leftVn);
            queue.enqueue(leftVn);
        }
//  将所有  产生式中含有已经访问非终结符  的左部非终结符添加到访问集合和队列
    while (!queue.empty()) {
        QString left = queue.dequeue();
        for(const auto& leftVn : grammars.keys())
            for (const auto& prod : grammars[leftVn].right)
                for (const auto& symbol : prod){
                    // 如果一个右部符号是非终结符并且被访问过
                    if (isNonTerm(symbol) && visited.contains(symbol)) {
                        if(!visited.contains(leftVn)){// 如果左部没有被访问过
                            visited.insert(leftVn);// 将左部加入队列和集合中
                            queue.enqueue(leftVn);
                        }
                    }
                }
    }
    // 删除所有无法推导至终结符串的非终结符的文法
    for (auto& Vn : grammars.keys())
        if (!visited.contains(Vn)) // 如果这个文法的左部符号没有被访问过
            clear.insert(Vn); // 添加到删除集合
    updateGrammars(clear); // 删除集合有内容，更新文法
//    debugTerm("After ");
}
