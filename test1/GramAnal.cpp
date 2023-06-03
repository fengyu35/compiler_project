#include "GramAnal.h"

// 主控程序
// 需要先调用 parseStrToGrammar 函数，得到对应的 grammars
bool GramAnal::Run(WindowState state, const QString tokenStr) {
    rmHarmfulProd();
    rmNoArriveGram();
    rmNoStopGram();
    if(state == GrammarSimplify)
        return true;

    rmLeftRecursive();
    if(state == removeLeftRecursive)
        return true;

    rmLeftCommonFactor();
    if(state == removeLeftCommonFactor)
        return true;

    genFirst();
    genFollow();
    if(state == FirstFollow)
        return true;

    genLLtable();
    if(state == LLtable)
        return true;

    if(!setTokens(tokenStr))  // 设置 词法分析结果 失败，返回 false
        return false;

    LL1();
    if(state == GrammarTree)
        return true;
    return true;
}
GramAnal::GramAnal() {
    root = nullptr;
}

GramAnal::~GramAnal() {
    clearArg();
}

void GramAnal::clearArg() {
    firstNonterm = "";
    grammars.clear();
    Terms.clear();
    AnalyTable.clear();
    order.clear();
    tokens.clear();
}


/**
 * @brief 设置语法分析器的 token
 * @param strToken: 包含 token 的字符串，每行格式为 "token_type\ttoken_value"
 * @return 设置成功返回 true，否则返回 false
 * @details 该函数将字符串 strToken 解析成一组 token，并存储在类成员变量 tokens 中
 *          如果输入格式不正确，函数会输出错误信息并返回 false
 */
bool GramAnal::setTokens(const QString strToken)
{
    QStringList lines = strToken.split("\n"); // 按行分割字符串
    for(const auto& line : lines){
        QStringList section = line.split("\t",QString::SkipEmptyParts); // 按制表符分割每行字符串
        if(section.size()== 0){
            qWarning() << "ERROR from setTokens(): this line size is 0!!!" << section;
        }
        else if(section.size()!= 2){ // 如果分割结果不正确，输出错误信息并返回 false
            qWarning() << "ERROR from setTokens(): this line is more than 2!!!" << section;
            return false;
        }else{
            tokens.push_back({section[1],section[0]}); // 将 token 加入列表
        }
    }
    return true; // 返回设置成功
}
QString GramAnal::toGramString(const QString &Vn) const {
    QStringList prods;
    for (const auto& prod : grammars[Vn].right)
        prods << prod.join(" "); // symbol 中间添加 空格
    return QString("%1 -> %2").arg(Vn).arg(prods.join(" | ")); // prod 中间添加 |
}

/**
 * @brief 更新文法中的终结符集合和非终结符集合
 */
void GramAnal::updateGrammars(const QSet<QString>& removeVn) {
    if(removeVn.empty())
        return;
//    debugGram("Before updateGrammars: ");
    QSet<QString> checkVn = {""};      // 检查非终结符 可能需要删除
    QSet<QString> checkVt;              // 检查终结符   可能需要删除
    QSet<QString> clearedVn = removeVn; // 非终结符     必须删除
    QSet<QString> clearVt;              // 终结符       必须删除,在 check确认后 增加

    while(checkVt.size() || checkVn.size()){
        // 获取所有 需要检查的 Vn 和 Vt
        checkVt.clear();
        checkVn.clear();
        for(const auto& ckVn: clearedVn)
            if(grammars.contains(ckVn))
                for(const auto& prod: grammars[ckVn].right)
                    for(const auto& symbol : prod) { // 遍历所有 需要删除文法含有的 符号，添加到check集合
                        if(isNonTerm(symbol) && !clearedVn.contains(symbol) && symbol != firstNonterm) {
                            checkVn.insert(symbol);
                        }else if (isTerm(symbol) && !clearVt.contains(symbol)) {
                            checkVt.insert(symbol);
                        }
                    }
        if(checkVt.empty() && checkVn.empty())
            break;

        // 遍历其他 grammar 删除含有Vn的产生式， 以及含有的 Vt 从检查集合移除
        for(auto& leftVn: grammars.keys()) {
            if(clearedVn.contains(leftVn)) { // 如果是 将要被删除的 非终结符 ，跳过
                continue;
            } else {
                QSet<QStringList> newRight;
                for(auto& prod: grammars[leftVn].right) {
                    bool flgRmProd = false;  // 判断是否 含有clearedVn 的产生式，
                    //   当 checkVn 元素出现在其他的 prod 里面
                    for(const auto& symbol: prod)
                        if(clearedVn.contains(symbol)) {
                            flgRmProd = true;
                            break;
                        }
                    if(!flgRmProd)
                        newRight.insert(prod);
                } // producer
                if(newRight.size()!=0){ // 如果删除后的结果不为空
                    grammars[leftVn].right = newRight;
                    for(const auto& prod: newRight)
                        for(const auto& symbol: prod){
                            checkVt.remove(symbol);  // 终结符 在其他 gram 出现，不需要删除
                            if(!clearedVn.contains(symbol)){
                                checkVn.remove(symbol);  // 不是必须删除的非终结符 在其他 gram 出现，不需要删除
                            }
                        }
                } else { // 如果删除后,得到的右部为空
                    clearedVn.insert(leftVn);
                    checkVn.insert(leftVn);
                }
            }
        }// gram
        for(const auto& Vt:checkVt)
            clearVt.insert(Vt);
    }// end while

    for(const auto& Vn:clearedVn)
        grammars.remove(Vn); // 删除非终结符左部
    for(const auto& Vt: clearVt)
        Terms.remove(Vt);
    debugGram("After updateGrammars: ");
}
/**
 * @brief 将字符串形式的文法转换为内部数据结构
 *
 * @param QString str 输入的字符串形式的文法
 * @return bool true 转换成功，false 转换失败
 * @note
 * 该函数会解析输入的字符串，将产生式左部和右部分别提取出来，
 * 并将符号添加到终结符集合和语法规则中，
 * 来实现将字符串形式的文法转换为内部数据结构的过程。
 */
bool GramAnal::parseStrToGrammar(const QString& str) {
    if(str == "")
        return false;
    clearArg();  // 初始化成员变量
    QStringList lines = str.split("\n", QString::SkipEmptyParts); // 跳过空行
    for(const auto& line: lines){
        QStringList parts = line.split("->");
        if (parts.size() != 2){
            return false; // 含有错误产生式：产生式不符合 -> 格式 或者 存在多个，返回错误
        }
        QString left = parts[0].trimmed();// 左部非终结符
        grammars[left] = {};        // 初始化 grammar
    }

    for (const auto& line : lines) {
        QStringList parts = line.split("->");
        QString left = parts[0].trimmed();// 左部非终结符
        QStringList prods = parts[1].split("|");// 右部产生式
        QSet<QStringList> newRight;
        for (const auto& prod : prods) {
            QStringList symbols = prod.trimmed().split(" ",QString::SkipEmptyParts); // 右部符号序列
//            qDebug() << symbols;
            newRight.insert(symbols);
            for (const auto& symbol : symbols) { // 遍历所有符号
                if (!isNonTerm(symbol) && !isTerm(symbol) && symbol != epsilon) // 如果没有记录过 且不是 空串
                    Terms.insert(symbol);// 将右部中未出现过的符号添加到终结符集合中
            }
        }
        // 得到的语法添加到grammars
        if(grammars.contains(left)){
            for(const auto& r: newRight)
                grammars[left].right.insert(r);
        } else
            grammars[left].right = newRight;

        if(firstNonterm == "")
            firstNonterm = left;// 默认第一个就是文法开始符号
    }
    return true;
}
