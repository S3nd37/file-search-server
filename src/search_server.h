#pragma once
#include <algorithm>
#include <stdexcept>
#include <string>
#include <set>
#include <map>
#include <vector>

#include "string_processing.h"
#include "document.h"

const int MAX_RESULT_DOCUMENT_COUNT = 5;

class SearchServer {
public:

    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words)
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words))
    {
        if (!all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
            throw std::invalid_argument("Some of stop words are invalid");
        }
        bool first = true;
        for (const auto& word : stop_words) {
            if (first) {
                stop_words_string_ += word;
                first = false;
            } else {
                std::string line = " " + word;
                stop_words_string_ += line;
            }
        }
    }

    explicit SearchServer(const std::string& stop_words_text);

    void SetStopWords(const std::string& text);

    std::string GetStopWords();

    void AddDocument(int document_id, const std::string& document, const std::string& path);

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const {
        const auto query = ParseQuery(raw_query);

        auto matched_documents = FindAllDocuments(query);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                return lhs.relevance > rhs.relevance;
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return matched_documents;
    }

    int GetDocumentCount() const;

    int GetDocumentId(int index) const;

    std::string GetDocumentPath(int id) const;

    bool SaveIndex(const  std::string& index_file) const;

    bool LoadIndex(const  std::string& index_file);

    void SetIndexedDir(const std::string& dir);

    std::string GetIndexedDir() const;

private:
    std::set<std::string> stop_words_;
    std::string stop_words_string_;
    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    std::set<int> document_ids_;
    std::vector<int> document_ids_vector_;
    std::map<int, std::string> document_paths_;
    std::string indexed_dir_;

    bool IsStopWord(const std::string& word) const;

    static bool IsValidWord(const std::string& word);

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const;

    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(const std::string& text) const;

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    Query ParseQuery(const std::string& text) const;

    double ComputeWordInverseDocumentFreq(const std::string& word) const;

    std::vector<Document> FindAllDocuments(const Query& query) const {
        std::map<int, double> document_to_relevance;
        for (const std::string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto &[document_id, term_freq] : word_to_document_freqs_.at(word)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }

        for (const std::string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto &[document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        std::vector<Document> matched_documents;
        for (const auto &[document_id, relevance] : document_to_relevance) {
            matched_documents.push_back(
                {document_id, relevance, document_paths_.at(document_id)});
        }
        return matched_documents;
    }
};