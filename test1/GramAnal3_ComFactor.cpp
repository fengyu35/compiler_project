#include "GramAnal.h"
/**
 * @brief GramAnal::rmLeftCommonFactor
 * 该函数用于从文法中移除间接左公因子
算法实现：
A.	遍历每个左部非终结符 leftVn
B.	如果产生式数量不满足条件,跳过
C.	遍历不同的产生式 iprod 和 jprod如果它们的 first 集有交集,且不是终结符或相同的非终结符  更新标志 flgChange 为 true ，开始推导它们的公因子：
D.	将 iprod 和 jprod 压入队列 qProd1 和 qProd2
E.	如果 qProd1[0] 和 qProd2[0] 的首个符号不同，根据首个符号推导新的产生式压入队列，删除旧的产生式
F.	推导直到：产生式数量增加 或 推导出直接左公因子
G.	将未访问的产生式添加到 newRight ,以及 qProd1 和 qProd2 的产生式
H.	更新 leftVn 的右部为 newRight
I.	调用 rmLeftDirectCommonFactor() 移除直接左公因子
J.	调用 rmNoArriveGram() 移除不可达产生式

 */
void GramAnal::rmLeftCommonFactor(){
    genFirst();// 先调用 genFirst 函数 ，生成所有非终结符的 first 集合
    bool flgChange = true;
    while(flgChange){
        flgChange = false;
        for(const auto& leftVn: grammars.keys()){
            int tmpSize1 = grammars[leftVn].right.size();
            if((tmpSize1 == 2 && grammars[leftVn].right.contains({epsilon}))
                    ||(tmpSize1 == 1)) // 只有一个产生式 或者 2个产生式且其中一个为空串，跳过
                continue;
            QSet<QStringList> newRight;  // 形成新的右部
            QSet<QStringList> visited;   // 已访问的产生式
            QQueue<QStringList> qProd1 , qProd2; // 含有A和B的若干次推导的产生式

            for(const auto& iprod: grammars[leftVn].right) {
                visited.insert(iprod); // 添加已访问的产生式
                if(isEpProd(iprod)) continue; // 跳过空串
                for(const auto& jprod: grammars[leftVn].right) {
//                    if(leftVn == "stmt_seq'"&& jprod.front()==";"){
//                        debugGram("check  stmt_seq'");
//                        qDebug("leftVn == \"stmt_seq'\"&& iprod.front()==\";\"");
//                    }
                    if(isEpProd(jprod)) continue; // 跳过空串
                    if(iprod == jprod)
                        break;
                    const auto& prod1f = iprod.front();  // 重命名 首个符号
                    const auto& prod2f = jprod.front();

                    if(getProdFirst(iprod).intersects(getProdFirst(jprod))// 两个产生式的first集合有交集   （确保这次推导是有左公因子的）
                            && !(isTerm(prod1f)&&isTerm(prod2f)) // 且不（都是终结符）且 不是（相同的非终结符）  （将交给直接左递归函数处理）
                            && !(isNonTerm(prod1f) && isNonTerm(prod2f) && (prod1f == prod2f))
                            ) {
                        flgChange = true;// 更新循环标志,if结束后 退出到while循环内
                        qProd1.clear();
                        qProd2.clear();
                        qProd1.push_back(iprod);
                        qProd2.push_back(jprod);
//                        iprod jprod 的首个符号 只可能是  A B / A a / a A
//                        如果成功进行一次 增加产生式数量 的推导 或者 推导出直接左公因子 ， 则退出循环
                        while(qProd1.size() == 1 && qProd2.size() == 1
                              && qProd1[0].front() != qProd2[0].front()) {
//                            推导一次
                            if(isNonTerm(qProd1[0].front())) {
                                for(const auto& kprod: grammars[qProd1[0].front()].right)
                                    qProd1.enqueue(kprod + qProd1[0].mid(1));
                                qProd1.dequeue(); // 推导完成，去掉旧的产生式
                            }
                            if(isNonTerm(qProd2[0].front())) {
                                for(const auto& kprod: grammars[qProd2[0].front()].right)
                                    qProd2.push_back(kprod + qProd2[0].mid(1));
                                qProd2.dequeue(); // 推导完成，去掉旧的产生式
                            }
                        }
                    }
//                    由于 C++ 不允许在循环内部修改 循环范围，必须要跳出到 iprod jprod循环以外修改
                    if(flgChange)   break;
                } // for jprod
                if(flgChange)  break;
            } // for iprod
            if(flgChange) {
                for(const auto& prod: grammars[leftVn].right)
                    if(!visited.contains(prod))
                        newRight.insert(prod);// 添加未访问的产生式
                // 添加新推导出来的产生式
                for(const auto& prod: qProd1)
                    newRight.insert(prod);
                for(const auto& prod: qProd2)
                    newRight.insert(prod);
                grammars[leftVn].right = newRight; // 更新
                break;
            }
        } // for leftVn
        debugGram("Before Direct ComFactor");
        rmLeftDirectCommonFactor();
    } // while
//    debugGram("before Com");
    rmNoArriveGram();
}

/**
 * @brief 消除直接左公因子
 *
 * @warning 调用前需要先消除左递归
 * @details
 * 1. 遍历文法中的每个产生式，将其右部的各个产生式存储在一个 QVector 中。
 * 2. 对每对产生式进行比较，如果它们有相同前缀符号，则将这些前缀符号抽出来形成一个新的非终结符，并将这个新的非终结符添加到文法中。
 * 3. 将原来具有相同前缀符号的产生式中的前缀符号删除，并添加到新的产生式中。
 * 4. 将更新后的产生式重新加入文法中。
 */
void GramAnal::rmLeftDirectCommonFactor() {
    bool flgChange = true;
    while(flgChange){
        flgChange = false;
        QSet<QStringList> newRight;  // A' -> c1 | c2 非公共因子 填入新的右部
        QSet<QStringList> resRight;  // A -> a A' | b 填入现在的右部
        QString currLeft , newLeft;
        for(const auto& Vn: grammars.keys()){

            int tmpSize1 = grammars[Vn].right.size();
            if((tmpSize1 == 2 && grammars[Vn].right.contains({epsilon}))
                    ||(tmpSize1 == 1)) // 只有一个产生式 或者 2个产生式且其中一个为空串，跳过
                continue;

            currLeft = newLeft = Vn;
            QMap<QString,QSet<QStringList>> count; // 统计 Vn 所有产生式的 <首个符号 对应的产生式>
            int maxIndex = INT32_MAX;
            for(const auto& prod : grammars[Vn].right){
                count[prod.front()].insert(prod);
                if(maxIndex > prod.size())
                    maxIndex = prod.size();
            }
            for(const auto& front: count.keys()){
                if(count[front].size()>1){// 寻找 大于1 的统计,进行左公因子的提取.提取一次后退出 for 循环
                    flgChange = true; // 修改循环标志
                    while(grammars.contains(newLeft)) // 如果存在 newLeft
                        newLeft += NewNonTermPostfix;  // 则需要一直添加到没有 对应的左部 ，作为新的非终结符
                    // 获取目前统计到的所有以这个左公因子产生式 最长的左公因子
                    QStringList CommonSymbols; // 第一个左公因子
                    int index = 0; // 不同公因子的第一个元素索引
                    bool flgCommon = true;
                    while(flgCommon){
                        QString CheckSymbol = ""; // 初始化
                        if(++index == maxIndex) // 检查符号 向右移一位
                            break;
                        for(const auto& kprod: count[front]){
                            if(CheckSymbol == "")
                                CheckSymbol = kprod[index];
                            if(CheckSymbol != kprod[index]){
                                flgCommon = false;
                                break; // 循环到有一个不相同的，退出循环
                            }
                        }
                    }
                    QStringList tmp = *count[front].begin();
                    CommonSymbols = tmp.mid(0,index); // 0 到 index-1 为左公因子

                    // 开始修改右部    有直接左公因子 A -> a c1 | a c2 | b
                    for(const auto& prod: count[front]){
                        if(prod.size() > index){
                            newRight.insert(prod.mid(index)); // A' -> c1 | c2 非公共因子
                        }else if (prod.size() == index){
                            newRight.insert({epsilon}); // 如果长度相等，则增加空串
                        }
                    }
                    for(const auto& prod: grammars[Vn].right)// A -> a A' | b
                        if(!count[front].contains(prod))
                            resRight.insert(prod); // b
                    resRight.insert(CommonSymbols << newLeft); //a A'
                    break;
                } // end if
                if(flgChange)break;
            } // end for QMap count
            if(flgChange) break;
        }// end for Vn
        if(flgChange){
            grammars[currLeft].right = resRight;
            grammars.insert(newLeft,newRight);
        }
    }// end while flgChange
}
