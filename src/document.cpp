#include "document.h"
#include <string>

Document::Document() = default;

Document::Document(int id, double relevance, std::string path)
    : id(id)
    , relevance(relevance)
    , path(path) {
}


