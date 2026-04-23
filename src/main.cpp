#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "read_input_functions.h"
#include "string_processing.h"
#include "document.h"
#include "search_server.h"

using namespace std;

void PrintDocument(const Document& document) {
    cout << "{ "s
         << "document_id = "s << document.id << ", "s
         << "relevance = "s << document.relevance << " }"s << endl;
}

ostream& operator<<(ostream& os, const Document& doc) {
    os << "{ document_id = " << doc.id << ", relevance = " << doc.relevance << " }" << endl;
    return os;
}

ostream& operator<<(ostream& os, const vector<Document>& docs) {
    for (const auto& doc : docs) {
        os << doc;
    }
    return os;
}

int main() {
    // Здесь будет код для работы с файловой поисковой системой
    return 0;
}