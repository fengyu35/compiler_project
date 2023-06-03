#include "GramAnal.h"

/**
 * @brief 从文法中消除直接左递归
 *
 * @param[in] i 要处理的文法的数组下标
 *
 * @warning
 * 1. 当产生式只有一个符号的时候，不能删除第一个元素，否则会出现错误。
 * 2. 该函数只能消除直接左递归，对于间接左递归或循环左递归等形式的左递归无法处理
 * 3. 该函数会修改输入的文法，因此需要在调用前备份原始文法。
 *
 * @details
 * 1. 从文法中选取左边的非终结符作为当前处理的非终结符 currNonTerm，并为新的非终结符添加一个后缀 newLeftPostfix。
 * 2. 将当前非终结符的产生式集合 prods 分为两个部分：
 *      - 以当前非终结符开头的产生式集合 StartWithCurrNonterm；
 *      - 不以当前非终结符开头的产生式集合 NotStartWithCurrNonterm。
 * 3. 如果 StartWithCurrNonterm 集合为空，则说明文法中不存在直接左递归，直接返回。
 * 4. 如果 NotStartWithCurrNonterm 集合为空，则说明所有产生式都是直接左递归的形式，需要在文法中添加一个新的非终结符，以避免左递归。
 * 5. 如果 NotStartWithCurrNonterm 集合不为空，则需要将 NotStartWithCurrNonterm 集合中的产生式修改为不含当前非终结符开头的形式，并将新的产生式添加到文法中。
 * 6. 对于以当前非终结符开头的产生式集合 StartWithCurrNonterm，需要将每个产生式修改为不含当前非终结符开头的形式，
 *      并添加一个新的非终结符作为后缀。具体操作是：
 *          - 将产生式中第一个符号（即当前非终结符）删除；
 *          - 将新的非终结符添加到产生式的末尾。
 * 7. 最后，将一个新的空串产生式 {epsilon} 添加到 StartWithCurrNonterm 集合中，并将修改后的产生式集合添加到文法中。
 * @remark 时间复杂度为 O(n^2)，其中 n 是文法的大小。
 */
bool GramAnal::rmLeftDirectRecersive(const QString& Vn) {
    QString newLeft = Vn + NewNonTermPostfix;
    while(grammars.contains(newLeft))
        newLeft += NewNonTermPostfix;
    QSet<QStringList> prods = grammars[Vn].right;
    vector<QStringList> StartWithVn; // 如果不为空，则作为新的 grammar
    vector<QStringList> NotStartWithVn; // 如果不为空，则作为原来 grammar 的新右部
    for(const auto& prod : prods){ // 遍历产生式
        if(prod.front() == Vn){ // 左递归
            if(prod.size()>1)
                StartWithVn.push_back(prod);
            //            else{} // 输出错误，存在 P -> P 的产生式 ERROR，但是前面 有害规则已经去除
        }else if(!isEpProd(prod)) // 排除空串
            NotStartWithVn.push_back(prod);// 不是左递归
    }

    if(StartWithVn.empty())
        return false;// 没有直接左递归，直接返回 false

    bool flgRewriteGram = NotStartWithVn.empty();  // 一般来说如果出现此状况
    if(flgRewriteGram){
        newLeft = Vn; // 如果全部都是左递归，则不需要后缀
    } else {
        QSet<QStringList> resRight;
        for(auto& prod : NotStartWithVn)
            resRight.insert(prod << newLeft); // 新非终结符
        grammars[Vn].right = resRight;  // 修改 原来右部
    }
/***
 * 把 P -> Pa | b   =>变为
 *      P -> bP'  (resRight)
 *      P' -> aP' | 空串 (newRight)
 *  a 去掉左递归的非终结符，后面加入 P' ，最后添加 epsilon 空串
**/
//      处理左递归
    QSet<QStringList> newRight;
    for(const auto& prod : StartWithVn) {
        QStringList newProd = prod.mid(1);
        newProd << newLeft; // 后面添加 新非终结符
        newRight.insert(newProd);
    }
    newRight.insert({epsilon});// 添加空串
    if(flgRewriteGram){
        grammars[Vn].right = newRight;
        rmNoArriveGram();// 消除直接左递归，可能会产生不可到达的文法，需要额外清除。同时更新文法
        return false;
    }else{
        grammars.insert(newLeft,newRight);
        rmNoArriveGram();// 消除直接左递归，可能会产生不可到达的文法，需要额外清除。同时更新文法
        return true;
    }
    return true;
}
/**
 * @brief GramAnal::rmLeftRecursive
 * 该函数用于从文法中移除左递归。

A.	对不同的两个产生式 i,和 j
如果i产生式有以j为起始的产生式
用j产生式拆分i产生式,存入新集合newRight
否则,直接添加i产生式到newRight
B.	更新i产生式的右部为newRight
C.	调用rmLeftDirectRecersive()移除i的直接左递归
D.	如果成功移除左递归,将标志 flgChange 设置为true,继续下一轮循环
E.	循环直到不可再移除左递归,即 flgChange 为false
F.	调用 rmNoArriveGram()移除可能产生的不可到达产生式
 */
void GramAnal::rmLeftRecursive() {
    QSet<QStringList> newRight;
    if(grammars.size() < 1)
        return;
    // 对于每个产生式i，从前面的产生式j中查找是否有左递归，并进行消除左递归
    bool flgChange = true;
    while(flgChange){
        flgChange = false;
        for(auto& iVn: grammars.keys()) {
            for(auto& jVn: grammars.keys()) {
                if(iVn == jVn) break;
                newRight = {};
                // 遍历产生式i的每个右部产生式
                for(const auto & iprod: grammars[iVn].right) {
                    if(iprod.front() == jVn) { // 如果以 jVn 开头
                        // 对于i产生式中以jNonterm开头的每个产生式，将其拆分为两个产生式，加入到newRight集合中
                        for(const auto & jprod: grammars[jVn].right) {
                            QStringList newProd = jprod;
                            newRight.insert(newProd << iprod.mid(1));
                        }
                    }
                    else newRight.insert(iprod); // 否则直接添加
                }
                grammars[iVn].right = newRight; // 更新右部
            } // for gram j
            flgChange = rmLeftDirectRecersive(iVn); // 消除直接左递归
            if(flgChange) break;
        } // for gram i
        rmNoArriveGram();// 消除直接左递归，可能会产生不可到达的文法，需要额外清除。同时更新文法
    } // end While
}
