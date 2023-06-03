#include "GramAnal.h"

/**
 * @brief 生成文法各个非终结符的 first 集合
 *
 * @details
 * 1. 遍历所有非终结符 left
 * 2. 对 left 的所有产生式 prod 进行遍历
 * 3. 如果 prod 的第一个符号是终结符或者 prod 只有一个 epsilon 符号,则将该终结符/epsilon 添加到 left 的 FIRST 集合中,并标记有改变
 * 4. 否则,对 prod 中的每个符号 symbol 进行遍历
 *      - 如果 symbol 是终结符(且不为 epsilon),将其添加到 left 的 FIRST 集合中,并退出内层循环
 *      - 否则,将 symbol 的 FIRST 集合中的所有元素添加到 left 的 FIRST 集合中
 *          - 如果 symbol 的 FIRST 集合包含 epsilon,则标记 prod 产生式可能推导出空
 *      - 如果 prod 的所有符号的 FIRST 集合都包含 epsilon,则将 epsilon 添加到 left 的 FIRST 集合中,并标记有改变
 * 重复 step 2-4,直到对 left 的 FIRST 集合没有更新
 */
void GramAnal::genFirst() {
    // 初始化所有非终结符的First集合
    for (auto& gram : grammars)
        gram.first = {};

    bool flgChange = true;
    while (flgChange) {
        flgChange = false;
        for (const auto& leftVn : grammars.keys()) {
            auto& leftFirst = grammars[leftVn].first;       // 重命名 左部非终结符 的 first 集合
            for (const auto& prod : grammars[leftVn].right) {
                if(isEpProd(prod)){ // 如果 产生式 是 一个空串
                    flgChange |= !leftFirst.contains(epsilon);   // 直接将其添加到 First 集合中
                    leftFirst.insert(epsilon);
                    continue; // 检查下一个 产生式
                }
                // 遍历非空产生式的 所有元素
                for (auto symbol = prod.begin(); symbol!=prod.end(); symbol++) {
                    QString currSymbol = *symbol;
                    auto& currFirst = grammars[currSymbol].first;   // 重命名 右部 symbol 非终结符 的 first 集合
                    if (isTerm(currSymbol)) { // 如果是终结符
                        flgChange |= !leftFirst.contains(currSymbol);   // 直接将其添加到 First 集合中
                        leftFirst.insert(currSymbol);
                        break;  // 退出循环
                    } else { // 对于非终结符
                        //将其 First 集合中的所有不是 epsilon 的元素  添加到 当前非终结符的 First 集合中
                        for (const auto& firstSymbol : currFirst) {
                            if (firstSymbol != epsilon) {
                                flgChange |= !leftFirst.contains(firstSymbol);
                                leftFirst.insert(firstSymbol);
                            }
                        }

                        // 判断是否继续读取下一个 symbol
                        if (currFirst.contains(epsilon)){ // 如果该符号的 First 集合包含空串，则继续处理下一个符号
                            if (symbol == prod.end() - 1) {// 如果已经处理到产生式的最后一个符号
                                flgChange |= !leftFirst.contains(epsilon);// 则将空串添加到当前非终结符的 First 集合中
                                leftFirst.insert(epsilon);
                            }
                        } else
                            break; // 不包含空串,否则结束循环
                    }
                } // each symbol
            } // each producer
        } // each grammar
    }

    // 由于 map 的中括号在不包含元素时会自动创建，需要在结束前清除 终结符和空产生式为左部 的语法
    for(const auto& term: Terms)
        grammars.remove(term);
    grammars.remove(epsilon);
}
/**
 * @brief 该函数用于从文法中生成 Follow 集
A.	初始化每个非终结符的 Follow 集为空集
B.	将开始符号的 Follow 集加入 stackBottom
C.	设置标志 flgChange,表示是否发生变化
D.	开始循环,直到 flgChange 为 false。遍历所有语法的每个产生式 prod
E.	如果产生式为空串,跳过
F.	否则遍历每个符号 symbol
G.	如果是终结符,跳过
H.	否则取符号 symbol 的 Follow 集 currFollow
I.	查看下一个符号 peek
如果是终结符,加入到 currFollow
否则(非终结符),加入 peek 的 First 集非空元素到 currFollow
J.	如果后面的串可以推导为空串,或者 peek 是结尾:
K.	将 leftVn 的 Follow 集加入到 currFollow
L.	设置 flgChange 为 true,表示 Follow 集发生变化
M.	最终移除终结符和空产生式作为左部的文法

 */
void GramAnal::genFollow() {
    for (auto& gram : grammars)
        gram.follow = {};// 初始化
    grammars[firstNonterm].follow.insert(stackBottom);

    bool flgChange = true;
    while(flgChange) {
        flgChange = false;
        for(const auto& leftVn: grammars.keys()){
            auto& leftFollow = grammars[leftVn].follow;       // 重命名 左部非终结符 的 follow 集合
            for(auto& prod : grammars[leftVn].right){
                if(isEpProd(prod))  continue; // 空串 则跳过

                QStringList tmpProd = prod; // 暂存后面的符号串
                for(auto symbol = prod.begin(); symbol!=prod.end(); symbol++){
                    tmpProd = tmpProd.mid(1); // 去掉最左边的一个符号
                    if(isTerm(*symbol)) // 终结符 则跳过
                        continue;
                    else{ // 非终结符 ， 查看下一个字符
                        auto& currFollow = grammars[*symbol].follow;   // 重命名
                        auto peek = symbol;
                        peek++;
                        if(peek != prod.end()) { // 不是结尾
                            if(isTerm(*peek)){ // 终结符，则加入到 自己的Follow集合
                                flgChange |= !currFollow.contains(*peek);
                                currFollow.insert(*peek);
                            }else{ // 下一字符 非终结符
                                for (const auto& xFirst : grammars[*peek].first)
                                    if(xFirst != epsilon){// 加入下一个字符的first集合非空元素
                                        flgChange |= !currFollow.contains(xFirst);
                                        currFollow.insert(xFirst);
                                    }
                            }
                        }
                        // 到了结尾 或者 后面的串包含空，则Follow(grammar.left) 元素加入到 Follow(currSymbol)
                        if(getProdFirst(tmpProd).contains(epsilon) || peek == prod.end())
                            for(const auto &aFollow: leftFollow) {
                                flgChange |= !currFollow.contains(aFollow);
                                currFollow.insert(aFollow);
                            }
                    }
                }// each symbol
            }// each producer
        }// each grammar
    }// end while flgChange

    // 由于 map 的中括号在不包含元素时会自动创建，需要在结束前清除 终结符和空产生式为左部 的语法
    for(const auto& term: Terms)
        grammars.remove(term);
    grammars.remove(epsilon);
}
/**
 * @brief 该函数用于计算给定产生式的 First 集
 * @param prod
 * @return 。
具体实现:
A.	如果是空串，则返回 空串。
B.	初始化 ProdFir 为产生式的 First 集。
C.	遍历每个符号 symbol  如果是终结符,直接加入 ProdFir 后退出循环 否则(非终结符):
D.	将该非终结符的 First 集加入 ProdFir
E.	如果该非终结符的 First 集包含空串
F.	如果 symbol 已经是产生式的最后一个符号,则加入空串到 ProdFir
否则继续处理下一个符号。否则直接退出循环
 */
QSet<QString> GramAnal::getProdFirst(const QStringList& prod) {
    if(prod.empty()) return{};
    if(isEpProd(prod)) return {epsilon};
    // 遍历非空产生式的 所有元素
    QSet<QString> ProdFir; // 产生式的 first 集合
    for  (auto symbol = prod.begin(); symbol!=prod.end(); symbol++){
        QString currSymbol = *symbol;
        if (isTerm(currSymbol)) { // 如果是终结符，直接将其添加到 First 集合中
            ProdFir.insert(currSymbol);
            break;  // 退出循环
        } else { // 对于非终结符，将其 First 集合中的所有元素添加到当前非终结符的 First 集合中
            const auto& currFirst = grammars[currSymbol].first;  // 重命名 左部非终结符 的 first 集合
            for (const auto& currFir : currFirst) {
                if (currFir != epsilon) {
                    ProdFir.insert(currFir);
                }
            }

            // 判断是否继续读取下一个 symbol
            if (currFirst.contains(epsilon)) { // 如果该符号的 First 集合包含空串，则继续处理下一个符号
                if (symbol + 1 == prod.end()) // 如果已经处理到产生式的最后一个符号
                    ProdFir.insert(epsilon); // 则将空串添加到当前非终结符的 First 集合中
            } else
                break; // 否则结束循环
        }
    }// each symbol

    return ProdFir;
}
