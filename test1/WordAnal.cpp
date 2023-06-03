#include "WordAnal.h"
/**
 * @brief 解析词法分析器表达式
 * @param expstring 词法分析器表达式字符串
 * @param currState 当前窗口状态
 * @note 该函数会根据给定的词法分析器表达式字符串 expstring，解析出其中的参数和正则表达式，并将它们转化为 NFA、DFA 和 SDFA。
 * 具体的解析过程为：
 * 首先根据换行符将表达式字符串分割成多个子串；
 * 然后按照等号将每个子串分割为参数和正则表达式两部分；
 * 接着对于每个正则表达式，按照词法分析器的规则进行分词，并将每个词汇转化为 NFA 规则；
 * 最后根据参数进行各种检查和转化，如果合法，则可以达到DFA，最终得到 SDFA。
 * 如果当前窗口状态为 NFA 或者参数设置不合法，那么函数只会生成 NFA，并返回；
 * 如果当前窗口状态为 DFA，那么函数只会生成 DFA 并返回；
 * 否则，函数会生成完整的 SDFA。函数执行结束后，会将生成的 SDFA 添加到类成员变量中。
 * */
// NFA -> DFA -> SDFA 流程
void WordAnal::parseExpressions(const QString& expstring, const WindowState currState) {
    clearArgs();
    transChar = {epsilon};

//    获得 expressions 用换行符分隔
    QStringList expressions = expstring.split("\n");
    if(expressions.empty()){
        qWarning("ERROR From parseExpressions(): the expressions is empty!!");
        return;
    }
    NFAstates.push_back(NFAState(0));
    NFAstates[0].setIsStart(true);
//  遍历每行的表达式
    bool flg = false;
    for(auto & exp : expressions){
        QString substr = getExpressionAfter(exp,"=");// 提取第一个等号后面的内容
        if(exp=="" || flg){    // 遇到第一个空行，则认为是后面的行都是设置参数
            flg = true;
            if(exp!="")
                setArgs(exp);
        } else{
            QStringList tokens = segment(substr);  // 普通正则表达式
            addNfaRule(tokens,getExpressionBefore(exp,"=").trimmed()); // 等号左边为 正则表达式的变量名
        }
    }
//    添加其他参数的 NFA 图
    setNfaArgs();


    // 更新一次各个 NFA 状态包含的空边集合
    for(size_t i=1;i<NFAstates.size();i++)
        checkEpEdge(i);
    checkEpEdge(0);
//    如果窗口目前的状态是NFA 或者 检查对应的参数不合法，返回
    if(currState==nfa||!checkArgs())
        return;

    auto it = transChar.find(epsilon);
    transChar.erase(it);
    set<size_t> endDFAState = CreateDFA(endNFAState);

    if(currState==dfa)   // 完成 DFA
        return;

    CreateSDFA(endDFAState);
    return;
}

// 提取保留字，块注释的开始符和结束符
void WordAnal::setArgs(const QString &str) {
    QStringList args = str.split(' ',QString::SkipEmptyParts);
    if(args[0] == "ReservedWord"){  // 获取保留字
        for(int i=1;i<args.size();i++)
            ReservedWord.insert(args[i]);
    }
    else if(args[0] == "LineCommentSign")
        LineCommentSign = args[1];
    else if(args[0] == "BlockCommentBegin")
        BlockCommentBegin = args[1];
    else if(args[0] == "BlockCommentEnd")
        BlockCommentEnd = args[1];
    else if(args[0] == "IgnoreCase")
        IgnoreCase = true;
    else if(args[0] == "varReservedWord")
        varReservedWord = args[1];
    else if(args[0] == "SpecialSymbol")
        for(int i=1; i< args.size();i++)
            SpecialSymbol.insert(args[i]);
    else qWarning("ERROR From Setting(): No Such command!!");
}
/**
 * @brief 设置 NFA 参数并生成块注释、行注释和特殊符号的 NFA
 * @return id 下一个 NFA 状态的 ID
 * @note
 * 该函数会根据类成员变量中保存的参数，生成块注释、行注释和特殊符号的 NFA。
    对于块注释和行注释，会根据类成员变量中保存的 BlockCommentBegin、BlockCommentEnd 和 LineCommentSign，
    将它们转化为对应的 NFA 规则，并添加到类成员变量 NFAstates 中。
    对于特殊符号，会遍历类成员变量 SpecialSymbol，将其中的每个符号转化为对应的 NFA 规则，
    并添加到类成员变量 NFAstates 中。
    在生成完 NFA 规则之后，该函数会返回下一个 NFA 状态的 ID。
*/
void WordAnal::setNfaArgs(){
    //    生成块注释和行注释的NFA
    if(BlockCommentBegin!="" && BlockCommentEnd!=""){
        QStringList tokens;
        for(auto & itChar : BlockCommentBegin)
            tokens << itChar;
        tokens << "AnyChar" << "*";
        for(auto & itChar : BlockCommentEnd)
            tokens << itChar;
        addNfaRule(tokens, "BlockComment");
    }
    if(LineCommentSign!=""){
        QStringList tokens;
        for(auto & itChar : LineCommentSign)
            tokens << itChar;
        tokens << "AnyChar" << "*" << "\n";
        addNfaRule(tokens, "LineComment");
    }
    // 生成 特殊符号的NFA
    if(SpecialSymbol.size()){
        for(const auto& symbol: SpecialSymbol){
            addNfaRule(segment(symbol), symbol);
        }
    }
}
/**
@brief 添加 NFA 规则
@param tokens 词汇列表
@param varName 变量名
@note 该函数会根据给定的词汇 tokens，将它们转化为对应的 NFA 规则，
并添加到类成员变量 NFAstates 中。具体的转化过程为：首先将词汇列表转化为后缀表达式；
然后调用 CreateNFA() 函数将后缀表达式转化为 NFA，并返回起点和终点的编号；
接着将起点为 0 的状态与新生成的 NFA 进行连接，
并将终点状态的变量名和编号添加到类成员变量 endNFAState 和 varString 中。
*/
void WordAnal::addNfaRule(const QStringList& tokens, const QString& varName) {
    QStringList postExp = postfix(tokens);
    pair<size_t,size_t> ansNFA = CreateNFA(postExp);
    Edge e(0,ansNFA.first);
    NFAstates[0].addEdge(e);
    // 终点设置内容
    NFAstates[ansNFA.second].setVarName(varName);
    endNFAState.insert(ansNFA.second);
    varString.insert(varName);
}
/**
 * @brief 清空词法分析器的所有参数和状态，以便进行下一轮的分析
 */
void WordAnal::clearArgs() {
    ReservedWord.clear();
    LineCommentSign = "";
    BlockCommentBegin = "";
    BlockCommentEnd = "";
    varReservedWord = "";
    IgnoreCase = false;
    varString.clear();
    NFAstates.clear();
    DFAstates.clear();
    SDFAstates.clear();
    transChar={};
    endNFAState = {};
}
// 检查该类中的几个成员变量是否设置正确，同时修正部分变量
/**
 * @brief 检查词法分析器的所有参数是否合法
 * @return true 如果所有参数都合法
 * @return false 如果存在不合法的参数
 * @note
 * 该函数会检查保留字变量是否为空，是否在变量集合 varString 中能找到。
 * 同时还会检查 BlockCommentBegin 和 BlockCommentEnd 是否同时为空或者同时非空，
 * 以及是否忽略大小写。
 * 如果参数合法，函数返回 true，否则返回 false。
*/
bool WordAnal::checkArgs() {
    // 设置了保留字，如果 保留字变量 为空或者在 varString 中找不到 保留字变量 ，返回 false
    if(ReservedWord.size())
        if(varReservedWord == "" || varString.find(varReservedWord) == varString.end())
            return false;
//    如果BlockCommentBegin 和 BlockCommentEnd只有一个为空，返回false
    if(BlockCommentBegin == "" && BlockCommentEnd != "")
        return false;
    if(BlockCommentBegin != "" && BlockCommentEnd == "")
        return false;
    if(ReservedWord.size() && IgnoreCase){
        set<QString> newWords;
        for (auto& word : ReservedWord) {
            newWords.insert(word.toLower());
        }
        ReservedWord = newWords;
    }
    return true;
}
/**
 * @brief 根据 SDFA 自动机的状态生成词法分析器的代码
 * @param [in, out] text 输出的代码流
 * @note
 * @note 该函数会根据已经构建好的 SDFA 和 ReservedWord，生成一个 C++ 代码的词法分析器程序。
 * 该程序会从文件中读入字符，对每个字符根据 SDFA 进行状态转移，并记录已经读入的 token。
 * 如果读入的字符可以转移到终止状态，那么对应的 token 和终止状态的名称会被记录到输出文件中。
 * 如果读入的字符无法转移至任何状态，那么会将已经读入的 token 记录为错误，并重新从 SDFA 的初始状态开始处理。
 * 如果整个过程结束，那么最后会关闭输入输出文件流并返回 0。
 * 函数执行结束后，会将生成的程序代码输出到给定的文本流中。
*/
void WordAnal::genProgram(QTextStream& text) const {
    if(SDFAstates.empty())
        return ;
    QString program;
    QString startID = QString::number(SDFAstartID);
    QString errorID = QString::number(SDFAstates.size());
    // 程序的开始部分
    text << "#include <string>\n"
           "#include <fstream>\n"
           "using namespace std;\n";
    // 写入转为大写字母的函数
    text << "string toUpper(string str){for(int i=0; i< str.size(); i++) \n"
            "if(str[i] >= 'a' && str[i] <= 'z')str[i] = str[i]-'a'+'A';return str;}\n";
    text << "int main(int argc, char** argv) {\n"
           "if(argc!=3)\n\t{printf(\"Must 2 FileName to Input and Output\"); return 1;}\n"
           "ifstream infile(argv[1],ios::in);\n"
               "if(!infile)\n\t{printf(\"Can't Open Infile %s\", argv[1]); return 1;}\n"
           "ofstream outfile(argv[2],ios::out);\n"
               "if(!outfile)\n\t{printf(\"Can't Open Outfile %s\", argv[2]); return 1;}\n";
//    设置全局变量
    text << "string token; char ch; unsigned int state = " + startID + ";\n";
    text << "string ReservedWords["+ QString::number(ReservedWord.size()) +"] = {";
    QString setStr;
    for(auto it = ReservedWord.begin();it != ReservedWord.end();it++){
        setStr += "\"" + *it + "\", ";
    }
    setStr[setStr.size()-1] = '}';
    text << setStr << ";" << endl;

//    函数循环
    text << "bool flgRead = true;"
            "while(infile.peek() != EOF){\n"
            "if(flgRead) infile.get(ch);\n else flgRead = true;\n"
            "switch(state){" << endl;
    for(auto& state : SDFAstates){
//  默认不存在状态 既是初态，又是终态。因为这意味着程序没有字符也合法
        text << "case "<< QString::number(state.getStateID()) << ":\n";
        bool flgElseIf = false;     // 输出if为true，输出else 为false
        bool flgAnyChar = false;
        size_t AnyCharTail = SIZE_MAX;  //稍后标记
        if(state.getIsStart()){// 初始状态需要跳过空白字符
            text << "if(ch == ' ' || ch == '\\t' || ch == '\\n') continue;\n";
            flgElseIf = true;
        }
        if(!state.getEdges().empty()){
            for(auto & edge:state.getEdges()){
                if(flgElseIf){text << "else "; flgElseIf = false; }
                QString Value = edge.Value;
                if(Value == "AnyChar"){ // 暂时缓存，到最后再添加AnyChar的代码
                    flgAnyChar = true;
                    AnyCharTail = edge.tail;
                } else {
                    if(Value == "[0-9]")
                        text << "if(ch >= '0' && ch <= '9') {\n";
                    else if(Value == "[a-zA-Z]"){
                        text << "if(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z') {\n";
                        if(IgnoreCase)// 如果设置了忽略大小写，相当于所有字母都换成大写
                            text << "if(ch >= 'A' && ch <= 'Z') ch = ch - 'a' + 'A';\n";
                    }
                    else if(Value == "\n")  // 换行符 特殊处理
                        text << "if(ch == '\\n') {\n";
                    else if(Value.size() == 1)
                        text << "if(ch == '"+ Value +"') {\n";
                    else if(Value.size() == 2 && Value[0]=='\\')
                        text << "if(ch == '"+ Value[1] +"') {\n";
                    else{
                        qWarning()<< "ERROR from genProgram(): The size of edgeValue more Than 2!!!" << Value;
                        return;
                    }
                    text << " token += ch; state = " + QString::number(edge.tail) + ";}\n";

                    flgElseIf = true;
                }
            }
        }
        if(state.getIsEnd()){
            QString varName = state.getVarName();
            if(flgElseIf){ text << "else "; flgElseIf = false;}
            if(varName == "BlockComment" || varName == "LineComment")
                text << "{";
            else if(varName == varReservedWord){
                text << "{bool flg = false;\n for(int i=0; i < " + QString::number(ReservedWord.size()) +"; i++)\n";
                text << "if(token == ReservedWords[i])\n"
                        "{outfile << token << '\\t' << toUpper(ReservedWords[i]) << endl; flg = true; break;}\n";
                text << "if(!flg)outfile << token << '\\t' << \""+ varName +"\" << endl;";
            }
            else
                text << "{outfile << token << '\\t' << \""+ varName +"\" << endl;";
            text << "token = \"\"; state = " + startID + "; flgRead = false;}\n";
        }
        // 添加AnyChar的代码
        if(flgAnyChar){
            if(flgElseIf){ text << "else "; flgElseIf = false;}
            text << "{ token += ch; state = " + QString::number(AnyCharTail) + ";}\n";
        }
        // 如果最后有 if 或者 else if ，而不是 else，说明可能存在出错的状态。
        if(flgElseIf){
            text<< "else \n{outfile << token << '\\t' << \"ErrorState\";token = \"\"; state = "+ errorID +"; }\n";
        }
        text << "break;" << endl;
    }
//    最后出错则会到达errorID，直接结束程序
    text <<"case " + errorID + ":\n defalt: {infile.close(); outfile.close(); return 0;}\n";
    // 最后关闭文件和返回
    text << "}\n}\n"
            "infile.close();\n"
            "outfile.close();\n"
            "return 0;\n}\n";
}
/**
 * @brief 将输入的字符串按照一定规则进行分割
 * @param exp 输入的待分割字符串
 * @return QStringList 返回分割后的字符串列表
 * @note 该函数会对输入的字符串进行状态转移，根据不同状态，将字符串分割成不同的 Token，并添加到 QStringList 中返回。
*/
QStringList WordAnal::segment(const QString &exp) {
    QStringList tokens;
    QString token;
    size_t state = 0;
    int i = 0;
    while(i<exp.size()) {
        switch (state) {
        case 0: // 初始节点，分支节点
            if(exp[i] == '\\'){
                state = 1;
                token += exp[i++];
            }else if(exp[i] == ' ' || exp[i] == '\t'){
                state = 2;
            }else if (exp[i] == '['){
                state = 4;
                token += exp[i++];
            }else{
                state = 3;
            }
            break;
        case 1:
            token += exp[i++];
            tokens.append(token);
            state = 0;
            token = "";
            break;
        case 2: // 终态 空格 \t \n
            i++;
            state = 0;
            token = "";
            break;
        case 3: // 终态
            if(exp[i] != ' ' && exp[i]!='\t')
                token += exp[i++];
            // 拆散并逐个字符加入 tokens
            for(auto & it : token)
                if(it != ' ' || it!='\t')
                    tokens.append(it);
            state = 0;
            token = "";
            break;
        case 4:
            if(exp[i] == '0'){
                state = 5;
                token += exp[i++];
            }else if (exp[i] == 'a'){
                state = 13;
                token += exp[i++];
            }
            else{
                state = 3;
            }
            break;
        case 5:
            if(exp[i] =='-'){
                state = 6;
                token += exp[i++];
            } else{
                state = 3;
            }
            break;
        case 6:
            if(exp[i] =='9'){
                state = 7;
                token += exp[i++];
            } else{
                state = 3;
            }
            break;
        case 7:
            if(exp[i] == ']'){
                state = 8;
                token += exp[i++];
            } else{
                state = 3;
            }
            break;
        case 8:
            tokens.append(token);
            state = 0;
            token = "";
            break;
        case 13:
            if(exp[i] =='-'){
                state = 14;
                token += exp[i++];
            } else{
                state = 3;
            }
            break;
        case 14:
            if(exp[i] =='z'){
                state = 15;
                token += exp[i++];
            } else{
                state = 3;
            }
            break;
        case 15:
            if (exp[i] == 'A'){
                state = 17;
                token += exp[i++];
            } else{
                state = 3;
            }
            break;
        case 17:
            if(exp[i] =='-'){
                state = 18;
                token += exp[i++];
            } else{
                state = 3;
            }
            break;
        case 18:
            if(exp[i] =='Z'){
                state = 19;
                token += exp[i++];
            } else{
                state = 3;
            }
            break;
        case 19:
            if(exp[i] == ']'){
                state = 20;
                token += exp[i++];
            } else{
                state = 3;
            }
            break;
        case 20:
            tokens.append(token);
            state = 0;
            token = "";
            break;
        default:
            qWarning("Wrong state!!");
            break;
        }
    }
    if(token!="")
        tokens.append(token);
    return tokens;
}
