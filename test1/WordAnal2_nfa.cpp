#include "WordAnal.h"
#include "WordAnal.h"
/**

@brief 创建 NFA 图
@param expression 后缀表达式
@return pair<size_t, size_t> NFA 图的起点和终点
@note 该函数会根据给定的后缀表达式 expression，创建对应的 NFA 图。
首先初始化一个边栈 eStack，然后遍历后缀表达式中的每个字符 ch，根据 ch 的类型将边压入栈中。
如果遍历结束后，栈的大小为 1，则将栈顶的边的尾部状态设为终点；
否则，函数会输出错误信息。
最后，函数将栈顶的边的头部状态和尾部状态作为 NFA 图的起点和终点，
返回一个 pair 对象, 记录NFA 图的起点和终点。
*/
pair<size_t, size_t> WordAnal::CreateNFA(const QStringList &expression) {
    QStack<Edge> eStack;  //记录存入栈的边
    // 建立NFA图
    for (auto &ch : expression)
        NFAprocess(eStack, ch);
    //经过上面的迭代，栈的大小应该为 1，确定终点
    if (eStack.size() == 1) {
        Edge e = eStack.top();
        NFAstates[e.tail].setIsEnd(true);
    } else{
        qWarning("From CreateNFA(): edges Stack size() != 1 ");
    }
    return pair<size_t, size_t>(eStack.top().head, eStack.top().tail);
}
/**
 * @brief 对输入的字符进行正则表达式的NFA转换处理
 *
 * @param[in,out] eStack 存储NFA边的栈
 * @param[in] ch 输入的字符
 *
 * @return void
 *
 * @details
 * 如果输入的字符为 '&', 则从栈中弹出两条边，并新增两条边，将其中一条边推回栈中；
 * 否则，新增头尾两个节点，并根据输入字符类型，新增相应的边，并将新边入栈。
 *      - 如果输入的字符为'|', '*', '+', '?'中的一个，则从栈顶弹出若干条边，并新增相应的边，将新边 e 入栈。
 * @note
 * 该函数会修改 NFAstates 中的状态和边，因此在调用该函数之前，需要确保 NFAstates 中的状态和边是正确的。
 * 该函数需要使用一个 QStack<Edge> 类型的栈 eStack 来存储中间结果。需要先创建一个空的 eStack 栈，并将初始状态的边压入栈中。
 * 在调用该函数后，需要检查 eStack 栈中是否只剩下一条边，如果不是，则说明表达式存在错误，需要进行相应的错误处理。
 */
void WordAnal::NFAprocess(QStack<Edge> &eStack, const QString &ch){
    if (ch == '&') {  // 不需要加头尾结点，出栈1条边，新增2条边
        Edge ne2 = eStack.pop();// ne1 ne2 出栈
        Edge ne1 = eStack.pop();
        Edge e1(ne1.tail, ne2.head);
        Edge e(ne1.head, ne2.tail);
        NFAstates[ne1.tail].addEdge(e1);
        eStack.push(e);// e 入栈
    } else {  // 运算符，新增头尾2个节点
        NFAState h(NFAstates.size());
        NFAstates.push_back(h);
        NFAState t(NFAstates.size());
        NFAstates.push_back(t);
        size_t hid = h.getStateID(), tid = t.getStateID();
        if (isOperand(ch)) {      // e 边有内容
            Edge e(hid, tid, ch);  //新增一条边
            NFAstates[NFAstates.size() - 2].addEdge(e);  // 更新
            eStack.push(e);                       // e 入栈
        } else {// 如果 e 是空边，则为普通符号 | * + ？
            Edge e(hid, tid),
                    ne = eStack.pop(),   // ne 出栈
                    e1(hid, ne.head),
                    e3(ne.tail, tid);
            if (ch == '|') {  // 如果是 | 则出栈2条边，新建4条空边
                Edge ne1 = eStack.pop(),
                        e2(hid, ne1.head),
                        e4(ne1.tail, tid);
                h.addEdge(e2);
                NFAstates[ne1.tail].addEdge(e4);
            } else { // 如果是 * + ? 则出栈1条边，新建3条空边
                Edge e2(ne.tail, ne.head);
                if(ch!='+')
                    h.addEdge(e);
                if(ch!='?')
                    NFAstates[ne.tail].addEdge(e2);
            }
            h.addEdge(e1);
            NFAstates[ne.tail].addEdge(e3);
            NFAstates[NFAstates.size() - 2] = h;  //更新
            eStack.push(e);                       // e 入栈
        }
    }
}
/**
 * @brief 更新 NFA 图中各个状态的空边集合
 *
 * @param i 状态编号
 * @note 该函数会从给定的状态 i 开始，遍历所有可以通过空边到达的状态，并将这些状态的编号添加到 i 状态的空边集合中。
 *
 * 具体的实现方法为：首先初始化已访问状态集合和状态栈；
 * 然后将状态 i 压入状态栈中，并循环处理状态栈中的状态，直到栈为空。
 * 在处理每个状态时，函数会将该状态标记为已访问，并更新它的空边集合。
 * 然后遍历该状态的空边指向的所有状态，如果这些状态未被访问，则将它们压入状态栈中并标记为已访问。
 * 最后，将空边指向的状态的空边集合添加到当前状态的空边集合中。
 */
void WordAnal::checkEpEdge(size_t i) {
    set<size_t> visited;// 初始化已访问状态集合
    QStack<size_t> stateStack;// 初始化状态栈
    stateStack.push(i);//并将当前状态压入栈中

    // 循环处理状态栈中的状态，直到栈为空
    while (!stateStack.empty()) {
        size_t stateIndex = stateStack.pop();// 取出栈顶状态
        visited.insert(stateIndex);// 并标记为已访问

        // 更新当前状态的空边集合
        NFAstates[i].insertEpTrans(stateIndex);

        // 处理当前状态的空边指向的所有状态
        for (auto& epEdge : NFAstates[stateIndex].getEpTrans()) {
            // 如果空边指向的状态未被访问，则将其入栈并标记为已访问
            if (visited.find(epEdge) == visited.end()) {
                stateStack.push(epEdge);
                visited.insert(epEdge);
            }
            // 将空边指向的状态的空边集合添加到当前状态的空边集合中
            for (auto& it : NFAstates[epEdge].getEpTrans())
                NFAstates[i].insertEpTrans(it);
        }
    }
}

