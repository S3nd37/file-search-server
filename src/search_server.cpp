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


SearchServer::SearchServer(const string& stop_words_text)
        : SearchServer(SplitIntoWords(stop_words_text))
    {
        stop_words_string_ = stop_words_text;
    }

void SearchServer::SetStopWords(const string& text) {
    stop_words_string_ = text;
    stop_words_ = MakeUniqueNonEmptyStrings(SplitIntoWords(text));
}

string SearchServer::GetStopWords() {
    return stop_words_string_;
}

void SearchServer::AddDocument(int document_id, const string& document, const string& path) {
    if ((document_id < 0) || (document_ids_.count(document_id) > 0)) {
        throw invalid_argument("Invalid document_id"s);
    }
    const auto words = SplitIntoWordsNoStop(document);

    const double inv_word_count = 1.0 / words.size();
    for (const string& word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
    }
    document_ids_.insert(document_id);
    document_ids_vector_.push_back(document_id);
    document_paths_[document_id] = path;
}

int SearchServer::GetDocumentCount() const {
    return document_ids_.size();
}

int SearchServer::GetDocumentId(int index) const {
    return document_ids_vector_.at(index);
}

string SearchServer::GetDocumentPath(int id) const {
    if (document_paths_.count(id) == 0) {
        return "";
    }
    return document_paths_.at(id);
}

bool SearchServer::SaveIndex(const  string& index_file) const {
    ofstream out(index_file);
    if (!out) {
        return false;
    }
    out << stop_words_string_ << "\n"s << document_paths_.size() << "\n"s;

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

bool SearchServer::LoadIndex(const string& index_file) {
    ifstream in(index_file);
    if (!in) {
        return false;
    }
    string line;
    getline(in, line); //здесь костыль для пропуска сохраненных в файле стоп-слов. 
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

bool SearchServer::IsStopWord(const string& word) const {
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const string& word) {
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
    });
}

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

SearchServer::QueryWord SearchServer::ParseQueryWord(const string& text) const {
    if (text.empty()) {
        throw invalid_argument("Query word is empty"s);
    }
    string word = text;
    bool is_minus = false;
    if (word[0] == '-') {
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
    double idf = log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    return idf;
}