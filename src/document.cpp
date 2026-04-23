#include "document.h"

Document::Document() = default;

Document::Document(int id, double relevance)
    : id(id)
    , relevance(relevance) {
}


