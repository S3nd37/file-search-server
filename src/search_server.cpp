#include <set>
#include <cmath>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
#include <fstream>


#include "search_server.h"
#include "string_processing.h"

using namespace std;
//здесь содержится поискового ядра, реализованного в виде отдельного класса.
//конструктор, учитывающий введённую строку стоп-слов
SearchServer::SearchServer(const string& stop_words_text)
        : SearchServer(SplitIntoWords(stop_words_text))
    {
        stop_words_string_ = stop_words_text;
    }
//метод для возможности задания стоп-слов.
void SearchServer::SetStopWords(const string& text) {
    stop_words_string_ = text;
    stop_words_ = MakeUniqueNonEmptyStrings(SplitIntoWords(text));
}
//метод получения стоп-слов - для записи в файл
string SearchServer::GetStopWords() {
    return stop_words_string_;
}
//добавление документа, здесь подробнее.
void SearchServer::AddDocument(int document_id, const string& document, const string& path) {
    //если мы вдруг пытаемся добавить документ с id < 0 или уже существующим номером - бросем исключение.
    if ((document_id < 0) || (document_ids_.count(document_id) > 0)) {
        throw invalid_argument("Invalid document_id"s);
    }
    const auto words = SplitIntoWordsNoStop(document);
    //здесь высчитывается tf для каждого слова путем прибавления константного значения столько раз, сколько слово встречается в документе.
    const double inv_word_count = 1.0 / static_cast<double>(words.size());
    for (const string& word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
    }
    document_ids_.insert(document_id);
    document_ids_vector_.push_back(document_id);
    document_paths_[document_id] = path;
}
//получить количество документов
int SearchServer::GetDocumentCount() const {
    return static_cast<int>(document_ids_.size());
}
//получить id документа
int SearchServer::GetDocumentId(int index) const {
    return document_ids_vector_.at(index);
}
//получить путь к документу 
string SearchServer::GetDocumentPath(int id) const {
    if (document_paths_.count(id) == 0) {
        return "";
    }
    return document_paths_.at(id);
}
//сохранение "слепка" экземпляра класса.
bool SearchServer::SaveIndex(const  string& index_file) const {
    ofstream out(index_file);
    if (!out) {
        return false;
    }
    out << stop_words_string_ << "\n"s
    << indexed_dir_ << "\n"s
    << document_paths_.size() << "\n"s;

    for  (const auto& [id, path] : document_paths_) {
        out << id << " " << path << "\n"s;
    }

    for (const auto& [word, info] : word_to_document_freqs_) {
        out << word << " "s << info.size() << " "s;
        for (const auto& [id, rel] : info) {
            out << id << " "s << rel << " "s;
        }
        out << "\n"s;
    }

    return true;
}

void SearchServer::SetIndexedDir(const string& dir) {
    if (dir.empty()) {
        indexed_dir_ = "all"s;
    } else {
        indexed_dir_ = dir;
    }
}

string SearchServer::GetIndexedDir() const {
    return indexed_dir_;
}

//загрузка индекса и создание класса из текстового файла
bool SearchServer::LoadIndex(const string& index_file) {
    ifstream in(index_file);
    if (!in) {
        return false;
    }
    string line;
    getline(in, line); //пропускаем стоп-слова, потому что метод не может создать экземпляр класса и это мы уже считали и использовали. Точка для развития
    getline(in, line);
    indexed_dir_ = line;
    int size = 0;
    in >> size;
    in.ignore(); //пропуск остатка строки
    for (int i = 0; i < size; ++i) {
        int id;
        string path;
        string temp;
        if (!getline(in, temp)) {return false;}
        auto pos = temp.find(' ');
        id = stoi(temp.substr(0, pos));
        path = temp.substr(pos + 1);

        document_ids_.emplace(id);
        document_ids_vector_.push_back(id);
        document_paths_[id] = path;
    }

    while (getline(in, line)) {
        vector<string> data = SplitIntoWords(line);
        string word = data[0];
        int col = stoi(data[1]);

        for (int i = 2; i < data.size(); i += 2) {
            word_to_document_freqs_[word][stoi(data[i])] = stod(data[i+1]);
        }
    } 

    return true;
}
//проверка на стоп-слово
bool SearchServer::IsStopWord(const string& word) const {
    return stop_words_.count(word) > 0;
}
//проверка на содержание спецсимволов в слове
bool SearchServer::IsValidWord(const string& word) {
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
    });
}
//разделение строки с исключением стоп-слов
vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw invalid_argument("Word "s + word + " is invalid"s);
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}
//разделение запроса на плюс-слова и минус-слова
SearchServer::QueryWord SearchServer::ParseQueryWord(const string& text) const {
    if (text.empty()) {
        throw invalid_argument("Query word is empty"s);
    }
    string word = text;
    bool is_minus = false;
    if (word[0] == '-') {//если первый символ слова "-", значит это минус-слово. Число с минусом определяется так же, точка для развития
        is_minus = true;
        word = word.substr(1);
    }
    if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
        throw invalid_argument("Query word "s + text + " is invalid");
    }

    return {word, is_minus, IsStopWord(word)};
}

SearchServer::Query SearchServer::ParseQuery(const string& text) const {
    Query result;
    for (const string& word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.insert(query_word.data);
            } else {
                result.plus_words.insert(query_word.data);
            }
        }
    }
    return result;
}

double SearchServer::ComputeWordInverseDocumentFreq(const string& word) const {
    double idf = log(GetDocumentCount() * 1.0 / static_cast<double>(word_to_document_freqs_.at(word).size()));
    return idf;
}