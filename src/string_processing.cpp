#include <vector>
#include <string>
#include <set>
#include <cctype>

#include "string_processing.h"

using namespace std;

bool IsPunctuation(char ch) {
    return (!isalnum(static_cast<unsigned char>(ch)) && ch != ' '); //isalnum проверяет является ли числом или буквой, в этом условии если !число/буква/пробел, значит
}                                                                   //понимаем что это знак препинания, а значит - разделитель
                                                                    //unsigned char защищает от русского и тд. пока нет поддержки русского языка
string ToLower(const string& str) {
    string lowered;
    for (const char& ch : str) {
        lowered += static_cast<char>(tolower(ch));
    }
    return lowered;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (size_t i = 0; i < text.size(); ++i) {
        if (static_cast<char>(text[i]) == ' ') {
            if (!word.empty()) {
                words.push_back(ToLower(word));
                word.clear();
            }
        } else if (IsPunctuation(static_cast<char>(text[i]))) { //теперь знаки препинания тоже считаются разделителями
            if (static_cast<char>(text[i]) == '.' && i > 0 && i + 1 < text.size() && isdigit(static_cast<unsigned char>(text[i - 1])) && isdigit(static_cast<unsigned char>(text[i + 1]))) {
                word += text[i]; //isdigit проверяет является ли char числом, тут проверяем не является ли вещественным числом если у нас точка, а вокруг нее - числа и мы не выходим за границы - берём как слово
            } else if (text[i] == '-' && (i >= 0) && i + 1 < text.size() && isalnum(static_cast<unsigned char>(text[i + 1]))) {
            //если - стоит перед буквой или цифрой - считаем как минус-слово
                word += text[i];
            } else {
                if (!word.empty()) {
                words.push_back(ToLower(word));
                word.clear();
                }
            }
        } else {
            word += text[i];
        }
    }
    if (!word.empty()) {
        words.push_back(ToLower(word));
    }

    return words;
}

string TrimPath(const string& input) {
    const auto first_smbl = input.find_first_not_of(" \t\n");
    if (first_smbl != string::npos) {
        const auto last_smbl = input.find_last_not_of(" \t\n");
        return input.substr(first_smbl, last_smbl-first_smbl + 1);
    } else {
        return "";
    }
                                                                          
}