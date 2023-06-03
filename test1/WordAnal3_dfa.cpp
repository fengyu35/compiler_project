#include "WordAnal.h"
/**
@brief 创建DFA
@param endNFAState NFA终态的集合
@return set<size_t> DFA的终态id集合
@note 该函数用于创建 DFA，包括状态的合并和转移的计算
*/
/**

@brief 从 NFA 转化为 DFA
@param endNFAState NFA 的终止状态集合
@return set<size_t> DFA 的终止状态集合
@note 该函数会根据类成员变量 NFAstates 中的 NFA 图,创建对应的 DFA 图。
具体的算法是使用子集构造法:
首先将 NFA 的初始状态的空边集合作为第一个 DFA 状态
然后使用队列遍历需要加入 DFA 的所有 NFA 状态的子集。
对于每个子集,遍历输入符号,计算下一个需要加入 DFA 的状态子集
如果该子集不存在,则创建新的 DFA 状态
最后返回 DFA 的终止状态集合。
*/
set<size_t> WordAnal::CreateDFA(const set<size_t> &endNFAState) {
    set<size_t> end;        // DFA的终态id集合
    size_t NumOfState = 0;  // DFA的ID最大值，数值等于 DFAstates.size()-1
    queue<size_t> qSubset;  // 子集队列 DFA状态ID

    DFAState Startstate(NFAstates[0].getEpTrans(), 0, true);
    DFAstates.push_back(Startstate);// 添加初态
    if (isFinal(0,endNFAState))// 更新isEnd end
        end.insert(0);
    qSubset.push(0);

    while (!qSubset.empty()) {// 广度优先遍历 队列中的子集
        size_t topID = qSubset.front();// 队列中取出一个子集
        qSubset.pop();

        //对每个终结符计算可达的状态
        set<size_t> startSet = DFAstates[topID].getStateSet();
        for (auto & itChar : transChar) {
            set<size_t> nextSet = getNextSet(startSet, itChar);  // 下一跳的NFA状态子集
//            qDebug() << topID << itChar << "getNextSet(): " << setTOstr(nextSet);
            if (!nextSet.empty()) {  //如果子集不为空
                size_t resfind = findVector(nextSet);// 是否已存在此子集
                Edge e(topID, resfind, itChar);  // 新建一个DFA边
                DFAstates[topID].addEdge(e);
                if (resfind == DFAstates.size()) {  // 如果是新的子集
                    DFAState newStates(nextSet, ++NumOfState);  //新建状态
                    DFAstates.push_back(newStates);  //存储新状态
                    if (isFinal(NumOfState, endNFAState))
                        end.insert(NumOfState); // 更新isEnd，end
                    qSubset.push(resfind);  //入队列
                }
            }// END IF NO empty nextSet
        }// END FOR transChar
    }// END WHILE QUEUE
    return end;
}
/**
@brief 该函数用于查找 DFA 状态集合中是否存在某个状态集合
@param s 状态集合
@return size_t 如果存在，则返回该状态集合所在的DFA状态的ID，否则返回DFA状态集合的大小
*/
size_t WordAnal::findVector(const set<size_t>& s) const {
    for (auto& it: DFAstates)
        if (it.getStateSet() == s)
            return it.getStateID();
    return DFAstates.size();
}

/**
@brief 更新 DFA 状态，并判断是否为终态
@param DFAstateID DFA状态的ID
@param endNFAState NFA终态的集合
@return true 如果该 DFA 状态为终态，则返回 true，否则返回 false
*/
bool WordAnal::isFinal(size_t DFAstateID, const set<size_t> &endNFAState) {
    if (!DFAstates[DFAstateID].getStateSet().empty()){
        for (auto & NFAstateID: DFAstates[DFAstateID].getStateSet()){
            if (endNFAState.find(NFAstateID) != endNFAState.end()) {
                DFAstates[DFAstateID].setIsEnd(true);
                DFAstates[DFAstateID].setVarName(NFAstates[NFAstateID].getVarName());
                return true;
            }
        }
    }
    return false;
}

/**
@brief 获取下一个 NFA 状态子集
@param startNFASet 当前 NFA 状态子集
@param transCh 输入符号
@return set<size_t> 下一个 NFA 状态子集
@note
该函数会根据给定的 NFA 状态子集 startNFASet 和输入符号 transCh,
计算下一个 NFA 状态子集。具体做法是:
对于 startNFASet 中的每个状态,
如果它有对应 transCh 的转移,则将它的下一状态加入结果 res 中;
将 res 中的所有状态通过空边再次可达的所有状态也加入结果中。
最终返回计算出来的下一个 NFA 状态子集。
*/
set<size_t> WordAnal::getNextSet(
        const set<size_t>& startNFASet,
        const QString &transCh) {
    set<size_t> tmp, tmp1, res;
     // 如果当前状态存在输入字符 c 的转移，则加入其下一状态
    for (auto & it: startNFASet) {
        tmp = NFAstates[it].getTrans(transCh);
        if (!tmp.empty())
            res.insert(tmp.begin(),tmp.end());
    }
    tmp1 = res;
    // 加入当前状态通过空边可达的下一状态
    for (auto & it: tmp1) {
        tmp = NFAstates[it].getEpTrans();
        res.insert(tmp.begin(),tmp.end());
    }
    return res;
}

