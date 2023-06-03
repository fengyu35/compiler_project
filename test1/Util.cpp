#include "Util.h"

/**
 * @brief 获取输入字符串中匹配指定字符的前面一段子串
 *
 * @param inputStr 要查找的输入字符串
 * @param chars 要匹配的字符集合
 * @return string 匹配字符前面的子串，如果没有找到匹配字符则返回空字符串
 *
 * @details
 * 1. 使用 string 的 find 函数查找输入字符串中第一个匹配字符集合中任意一个字符的位置。
 * 2. 如果找到了匹配字符，则返回该字符前面的子串；否则，返回空字符串表示没有找到匹配字符。
 */
QString getExpressionBefore(const QString &inputStr, const QString &chars) {
    int pos = inputStr.indexOf(chars);
    int charLen = chars.length();
    if (pos != -1 && pos < inputStr.size() - charLen) {
        return inputStr.mid(0, pos);
    } else
        return "";
}

/**
 * @brief 获取输入字符串中匹配指定字符的后面一段子串
 *
 * @param inputStr 要查找的输入字符串
 * @param chars 要匹配的字符集合
 * @return string 匹配字符后面的子串，如果没有找到匹配字符则返回空字符串
 *
 * @details
 * 1. 使用 string 的 find 函数查找输入字符串中第一个匹配字符集合中任意一个字符的位置。
 * 2. 如果找到了匹配字符，则返回该字符后面的子串；否则，返回空字符串表示没有找到匹配字符。
 */
QString getExpressionAfter(const QString &inputStr, const QString &chars) {
    int pos = inputStr.indexOf(chars);
    int charLen = chars.length();
    if (pos != -1 && pos < inputStr.size() - charLen) {
        return inputStr.mid(pos + charLen);
    } else
        return "";
}

/**
 * @brief 将一个 set 容器转换为字符串形式
 *
 * @param s 要转换的 set 容器
 * @return string 转换后的字符串
 *
 * @details
 * 1. 遍历 set 容器中的每个元素，将元素转换为字符串并添加到结果字符串中。
 * 2. 在结果字符串的开头和结尾添加大括号。
 * 3. 返回转换后的字符串结果。
 */
QString setTOstr(const set<size_t> &s) {
    QString tmp;
    if (s.size()){
        tmp = "{";
        for (auto & it : s) {
            tmp += QString::number(it);
            tmp += ',';
        }
        tmp[tmp.size()-1] = '}';
    }
    return tmp;  // 返回转换后的字符串结果
}

bool isEpProd(const QStringList &prod){
    return prod.size() == 1 && prod.front() == epsilon;
}

