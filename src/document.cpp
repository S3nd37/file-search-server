#include "document.h"
#include <string>
//здесь содержится реализация структуры документов.
Document::Document() = default;

Document::Document(int id, double relevance, std::string path)
    : id(id)
    , relevance(relevance)
    , path(path) {
}


