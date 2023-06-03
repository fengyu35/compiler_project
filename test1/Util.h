#ifndef UTIL_H
#define UTIL_H
#include <QByteArray>
#include <QStringList>
#include <vector>
#include <set>
using namespace std;

const QString epsilon = "@";//  空边转换符 空产生式
const QString stackBottom = "#";//  分析表的栈底元素
const size_t NO_EDGE = SIZE_MAX;// SDFA 没有边时，tail 设置为SIZE_MAX
const QString NewNonTermPostfix = "'";// 定义消除左递归和左公因子的新非终结符的后缀
const QString ERRORstr =  "ERROR_TOKEN"; // LL1 分析表 表格为空时，填入的字符串

// 设定当前窗口的执行状态。
enum WindowState{nothing, nfa, dfa, sdfa, GENprogram, RUNprogram,
                GrammarSimplify, removeLeftRecursive, removeLeftCommonFactor,
                 FirstFollow, LLtable, GrammarTree };

// 获取输入字符串中匹配指定字符的前面一段子串
QString getExpressionBefore(const QString& inputStr, const QString& chars);

// 获取输入字符串中匹配指定字符的后面一段子串
QString getExpressionAfter(const QString& inputStr, const QString& chars);

// 将一个 set 容器转换为字符串形式
QString setTOstr(const set<size_t>& s);

// 判断空串
bool isEpProd(const QStringList& prod);
#endif // UTIL_H
