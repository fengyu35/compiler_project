#include "WordAnal.h"

/**
 * @brief 将给定的正则表达式转换为后缀表达式
 *
 * @param QString exp1 给定的正则表达式
 * @return QString 转换后的后缀表达式
 *
 * @details
 * 1. 调用AddConnectSymbol函数添加连接符 &
 * 2. 调用InfixToPostfix函数将中缀表达式转换为后缀表达式。
 */
QStringList WordAnal::postfix(const QStringList &exp) {
    QStringList expList = AddConnectSymbol(exp); // 添加连接符 &
    QStringList postfixList = InfixToPostfix(expList); // 将中缀表达式转换为后缀表达式
    return postfixList; // 返回后缀表达式结果
}
/**
 * @brief 在表达式中添加连接符 &
 *
 * @param exp QStringList类型的给定表达式
 * @return QStringList类型的添加了连接符的表达式列表
 * @details 该函数实现了在表达式中添加连接符 & 的功能。具体实现如下：
 * 1. 如果给定表达式 exp 为空，则返回一个空的 QStringList。
 * 2. 初始化一个空的 QStringList expList，并将表达式中的第一个字符加入该列表中。
 * 3. 遍历表达式中的每个字符：
 *    - 如果两个连续字符都是单目运算符，则报错
 *    - 如果前一个字符是操作数、右括号、单目运算符，且当前字符是操作数或左括号，则在该字符前面添加连接符 &
 *    - 将当前字符加入 expList 中。
 * 4. 返回添加了连接符 & 的表达式列表 expList。
 */
QStringList WordAnal::AddConnectSymbol(const QStringList &exp) {
    QStringList expList;
    if (exp.empty()) return expList; // 1. 如果给定表达式 exp 为空，则返回一个空的 QStringList。
    expList.append(exp.first()); // 2. 将表达式中的第一个字符加入 expList 中。
    for (int i = 1; i < exp.size(); ++i) { // 3. 遍历表达式中的每个字符
        const QString &prevChar = exp[i - 1];
        const QString &currChar = exp[i];
        // 如果两个连续字符都是单目运算符，则报错
        if (isUnaryOperator(prevChar) && isUnaryOperator(currChar)) {
            qWarning("Error: Two consecutive unary operators are not allowed.");
            return QStringList();
        }

        // 如果前一个字符是操作数、右括号、单目运算符，且当前字符是操作数或左括号，则在该字符前面添加连接符 &
        if ( (isOperand(prevChar) || prevChar == ")"  || isUnaryOperator(prevChar))
                &&(isOperand(currChar) || currChar == "(") )
            expList.append("&");
        expList.append(currChar);   // 将当前字符加入 expList 中
    }
    return expList; // 4. 返回添加了连接符 & 的表达式列表 expList。
}
/**
 * @brief 将中缀表达式转换为后缀表达式
 *
 * @param exp QStringList类型的给定中缀表达式
 * @return QStringList类型的转换后的后缀表达式
 * @details
 * 1. 初始化一个空栈 operStack，并将栈底标志 "#" 压入栈中。
 * 2. 遍历中缀表达式 exp 中的每个 token：
 *    - 如果 token 是可以计算的字符（即操作数），则将 token 加入后缀表达式 postfixRes
 *          - 如果不是变量，则记录该 token 为转换字符
 *    - 如果 token 是单目运算符 +、* 或 ?
 *          - 如果前面有操作数，则将运算符加入后缀表达式 postfixRes ，否则报错。
 *    - 如果 token 是左括号，则将其压入栈 operStack 中。
 *    - 如果 token 是右括号，则弹出栈 operStack 中的运算符，加入后缀表达式 postfixRes ，直到遇到左括号，然后将左括号弹出。
 *          - 如果栈 operStack 中的运算符变为空，说明表达式前面没有左括号，则报错。
 *    - 如果 token 是双目运算符，则循环弹出 operStack 中优先级高于或等于 token 运算符并加入 postfixRes ，然后将 token 压入 operStack 中。
 * 3. 循环弹出 operStack 中剩余的运算符并加入后缀表达式 postfixRes ，直到栈顶为栈底标志 "#"。
 * 4. 返回后缀表达式 postfixRes
 */
QStringList WordAnal::InfixToPostfix(const QStringList &exp) {
    QStringList postfixRes;
    QStack<QString> operStack;
    operStack.push("#"); // 1. 压入栈底标志
    for (const QString &token : exp) { // 2. 遍历中缀表达式 exp 中的每个 token
        if (isOperand(token)) {   // 如果 token 是可以计算的字符（即操作数）
            postfixRes.append(token); // 将 token 加入后缀表达式 postfixRes
            if (varString.find(token) == varString.end()) // 如果不是变量
                transChar.insert(token); // 记录该 token 为转换字符
        } else if (isUnaryOperator(token)) {   // 如果 token 是单目运算符 +、* 或 ?
            if (postfixRes.size()) postfixRes.append(token); // 如果前面有操作数，则将运算符加入后缀表达式 postfixRes
            else {
                qWarning("Error in InfixToPostfix(const QStringList&): + * ? must have character before"); // 否则报错
                return QStringList();
            }
        } else if (token == "(") {   // 如果 token 是左括号
            operStack.push(token); // 将其压入栈 operStack 中
        } else if (token == ")") {   // 如果 token 是右括号
            while (operStack.top() != "(" && !operStack.isEmpty())
                postfixRes.append(operStack.pop()); // 弹出栈 operStack 中的运算符，加入后缀表达式 postfixRes，直到遇到左括号，然后将左括号弹出
            if(operStack.isEmpty()){
                qWarning("ERROR in InfixToPostfix(const QStringList&): The ')' Has NO Pair With '('");
                return QStringList();
            }
            operStack.pop(); // 弹出左括号
        } else {   // 如果 token 是双目运算符 & |
            while (isp(operStack.top()) >= icp(token))
                postfixRes.append(operStack.pop()); // 循环弹出 operStack 中优先级高于或等于 token 的运算符并加入后缀表达式 postfix
            operStack.push(token); // 将 token 压入 operStack 中
        }
    }
    while (operStack.top() != "#")
        postfixRes.append(operStack.pop()); // 3. 循环弹出 operStack 中剩余的运算符并加入后缀表达式 postfix，直到栈顶为栈底标志 "#"
//    qDebug() << postfixRes;
    return postfixRes; // 4. 返回后缀表达式 postfixRes
}
//相同运算符，栈内比站外优先级更高
/**
 * @brief 判断给定运算符在栈内的优先级
 *
 * @param oper 给定运算符
 * @return size_t 给定运算符在栈内的优先级
 */
size_t WordAnal::isp(const QString &oper) {
    if (oper == "&") return 2;
    else if (oper == "|") return 1;
    else if (oper == "(") return 0;
    else return 0;
}
/**
 * @brief 判断给定运算符在栈外的优先级
 *
 * @param oper 给定运算符
 * @return size_t 给定运算符在栈外的优先级
 */
size_t WordAnal::icp(const QString &oper) {
    if (oper == "&") return 2;
    else if (oper == "|") return 1;
    else if (oper == "(") return 5;
    else return 0;
}

/**
 * @brief 判断给定字符是否为运算符 * + ? ( ) & |
 *
 * @param ch 给定字符
 * @return true 如果给定字符是运算符，则返回 true；否则返回 false
 */
bool WordAnal::isOperator(const QString& ch) {
    if (ch == '(' || ch == ')' || ch == '&' || ch == '|' || isUnaryOperator(ch))
        return true;
    return false;
}

bool WordAnal::isUnaryOperator(const QString &ch) {
    if(ch == '*' || ch == '+' || ch == '?')
        return true;
    return false;
}

/**
 * @brief 判断给定字符是否为操作数
 *
 * @param ch 给定字符
 * @return true 如果给定字符是操作符，则返回 false 否则返回 true
 */
bool WordAnal::isOperand(const QString& ch) {return !isOperator(ch);}
