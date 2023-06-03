#ifndef _XFA_H
#define _XFA_H

#include <vector>
#include <QSet>
#include <QString>
#include "Util.h"
// 基类 Edge
struct Edge {
    size_t head;    // 头部指向的 stateID
    size_t tail;    // 尾部指向的 stateID
    QString Value;  // 边上的值
    Edge(size_t _head = 0, size_t _tail = 0, QString _Value = epsilon)
        : head(_head), tail(_tail), Value(_Value) {}
    Edge& operator = (const Edge& e){
        if(this != &e){
            head = e.head;
            tail = e.tail;
            Value = e.Value;
        }
        return *this;
    }
};

// 基类 State
class State {
protected:
    size_t stateID;  // 状态id
    QString varName;  // 变量名
    bool isStart;  // 是否为始态
    bool isEnd;  // 是否为终态
    vector<Edge> edges; // 状态转移边
public:
    State(size_t _id = 0, bool _isStart = false, bool _end = false)
        : stateID(_id), varName(""), isStart(_isStart), isEnd(_end), edges({}){}
    void addEdge(const Edge& e){ edges.push_back(e); }
    // 用来搜索指定值的出边集合
    set<size_t> getTrans(const QString& ch){
        set<size_t> res;
        for(auto & edge: edges)
            if(edge.Value == ch)
                res.insert(edge.tail);
        return res;
    }

    bool operator<(const State& s) const { return stateID < s.stateID; }
    size_t getStateID() const { return stateID; }
    vector<Edge> getEdges() const {return edges;}
    void setEdges(const vector<Edge>& _edges) { edges = _edges; }
    const QString& getVarName() const { return varName; }
    void setVarName(const QString& name) { varName = name; }
    bool getIsStart() const { return isStart; }
    void setIsStart(bool start) { isStart = start; }
    bool getIsEnd() const { return isEnd; }
    void setIsEnd(bool end) { isEnd = end; }
};

// NFA
class NFAState : public State {
protected:
    set<size_t> epTrans;  // 通过 epsilon 可以转移到的状态号集合,为后面DFA准备
public:
    NFAState(size_t _id = 0) : State(_id),epTrans({_id}) {}
    void addEdge(const Edge& e){
        // 重载基类，添加一条边到 edges 和 epTrans
        edges.push_back(e);
        if(e.Value == epsilon)
            epTrans.insert(e.tail);
    }

    void insertEpTrans(const size_t tail) { epTrans.insert(tail); }
    const set<size_t>& getEpTrans() const { return epTrans; }
    void setEpTrans(const set<size_t>& s) { epTrans = s; }
};

// DFA
class DFAState : public State {
protected:
    set<size_t> stateSet; // 包含前一张状态图的状态集合
public:
    DFAState() : State(){ isStart = false; isEnd = false;}
    DFAState(set<size_t> _set, size_t _id = 0, bool _isStart = false, bool _end = false)
        : State(_id, _isStart, _end), stateSet(_set){}
    const set<size_t>& getStateSet() const { return stateSet; }
    void setStateSet(const set<size_t>& set) { stateSet = set; }
};

// SDFA
class SDFAState:public State {
protected:
    set<size_t> stateSet; // 包含前一张状态图的状态集合
public:
    SDFAState() : State(0){}
    SDFAState(set<size_t> _set, size_t _id = 0, bool _isStart = false, bool _end = false)
        : State(_id, _isStart, _end), stateSet(_set){}
    const set<size_t>& getStateSet() const { return stateSet; }
    void setStateSet(const set<size_t>& set) { stateSet = set; }
};
#endif // _XFA_H
