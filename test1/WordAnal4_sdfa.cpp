#include "WordAnal.h"
/**

@brief 从 DFA 生成 SDFA
@param endDFAState DFA 的终止状态集合
@note 该函数会根据类成员变量DFAstates 中存储的 DFA状态,创建对应的 SDFA。
1. 调用 createPartSet 函数创建 SDFA 的划分集合 partSet;
2. 根据每个划分集合,创建对应的 SDFA 状态,设置初始状态、终止状态和变量名;
3. 根据 trans 数组,创建 SDFA 状态之间的边，将边添加到对应的 SDFA 状态。
4. SDFA 状态和边就创建完成,存储在类成员变量 SDFAstates 中。
*/
void WordAnal::CreateSDFA(const set<size_t>& endDFAState) {
    vector<map<QString, size_t>> trans;  //存储partSet的边<值，dest>
    vector<set<size_t>> partSet = createPartSet(trans,endDFAState); //用于存储所有的划分集合

    vector<Edge> tmpEdges;
    for (size_t i = 0; i < partSet.size(); i++) {
        set<size_t> currPartset = partSet[i];
        if(!currPartset.size())
            continue;

        SDFAState tmpSDFA(currPartset, i);
        //新建对应的边
        for (auto & itChar : transChar)
            if (trans[i][itChar] != NO_EDGE) {
                Edge ne(i, trans[i][itChar], itChar);
                tmpEdges.push_back(ne);
            }
        // 初态判断
        if (partSet[i].find(0) != partSet[i].end()){
            tmpSDFA.setIsStart(true);
            SDFAstartID = i;
        }
        // 终态判断
        for (auto & it : partSet[i])
            for (auto & it1 : endDFAState)
                if (it == it1) {
                    tmpSDFA.setIsEnd(true);
                    tmpSDFA.setVarName(DFAstates[it1].getVarName());
                    break;
                }
        SDFAstates.push_back(tmpSDFA);
    }
    //将状态添加对应的边
    for (auto & it: tmpEdges)
        SDFAstates[it.head].addEdge(it);
}

/**
@brief 从 DFA 中创建 SDFA 的划分集合
@param endDFAState DFA 的终止状态集合
@return vector<set<size_t>> SDFA 的划分集合
@note 该函数会根据类成员变量 DFAstates 中存储的 DFA 状态,创建 SDFA 的划分集。
首先根据 DFA 终止状态,将 DFA 状态划分为若干个终止状态集合和一个非终止状态集合;
对于每个 DFA 状态,如果输入符号对应的边导致划分集合数量增加,则进行新的划分;
循环执行上一步骤,直到不会再进行新的划分。
最终返回得到的所有划分集合。
在创建划分集的过程中,trans 变量用来记录每个终结符对应的划分集合。
*/
vector<set<size_t>> WordAnal::createPartSet(vector<map<QString, size_t>> &trans,const set<size_t>& endDFAState) {
    vector<set<size_t>> partSet;
    map<size_t, set<size_t>> tmpSet;  // <转移到的partset，源partset>
    bool hasEdge;  //判断该DFA状态是否存在与该终结符对应的弧

    //遍历DFA状态数组,将不同的终态和非终态划分开
    for(auto & it : groupByVarName(endDFAState))
        partSet.push_back(it.second);  //终态集
    size_t firstNon = partSet.size();
    partSet.push_back({});
    for (auto & itDFA :  DFAstates) {// 遍历每个DFA状态
        if (!itDFA.getIsEnd())  //如果该DFA状态不是终态
            partSet[firstNon].insert(itDFA.getStateID());  //加入到最后一个划分集合中

        bool cutflag = true;  //上次是否产生新的划分
        while (cutflag) {  //一直循环，直到上次没有产生新的划分
            int cutCount;  //划分次数
            for (size_t i = 0; i < partSet.size(); i++) {// 遍历每个划分集合partSet
                cutCount = 0;
                trans.push_back({});

                for (auto & itChar : transChar) {// 遍历每个终结符
                    tmpSet.clear();
                    for (auto & itStateID: partSet[i]) {// 遍历集合partSet[i]中的每个DFAstate
                        hasEdge = false;
                        for (auto& edge : DFAstates[itStateID].getEdges()) // 遍历state的每条边
                            if (edge.Value == itChar) {// 如果存在某条边的输入为当前终结符
                                size_t setId = FindSet(edge, partSet);//找到该弧转换到的状态所属的划分集合id
                                tmpSet[setId].insert(itStateID); //将该DFAstate的ID加入到缓冲区中能转换到setId的状态集合中
                                hasEdge = true;
                                break;
                            }
                        if (!hasEdge)
                            tmpSet[NO_EDGE].insert(itStateID);
                    }

                    auto itMap = tmpSet.begin();
                    if (tmpSet.size() > 1) {// 缓冲区中元素个数大于1，则需要划分
                        cutCount++;        // 划分次数 +1
                        for (itMap++; itMap != tmpSet.end(); itMap++) {// 从1开始，要将temp[0]中的元素保留在原划分集合中
                            partSet.push_back(itMap->second);  // 创建新的划分集合
                            for (auto & it : itMap->second) // 删除原划分集合partSet中temp[i] 中的元素
                                partSet[i].erase(it);
                        }
                    }  //否则无需划分，记录对应的tmpSet
                    else if (tmpSet.size() == 1)
                        trans[i][itChar] = itMap->first;//创建新边
                } // transChar
            } // partSet
            cutflag = cutCount;  //划分次数大于0说明本次产生了新的划分
        }
    }
    return partSet;
}

/**
@brief 将终态集合按照变量名分组
@param endDFAState DFA终态的集合
@return map<QString, set<size_t>> 返回按照变量名分组的结果
*/
map<QString, set<size_t>> WordAnal::groupByVarName(const set<size_t> &endDFAState) {
    map<QString, set<size_t>> res;
    for(auto& it : endDFAState){
        QString str = DFAstates[it].getVarName();
        res[str].insert(it);
    }
    return res;
}

/**
 * @brief 找到该弧转换到的状态所属的划分的 id集合
 * @param e 一条DFA边
 * @param partSet DFA状态的划分集合
 * @return size_t 该弧转换到的状态所属的划分的 id集合
*/
size_t WordAnal::FindSet(const Edge& e, const vector<set<size_t>>& partSet) {
    size_t res = 0;
    for (; res < partSet.size(); res++) {//遍历各个划分
        for (auto& it: partSet[res])//遍历划分内的每个状态
            if (it == e.tail)
                return res;
    }
    return res;
}
