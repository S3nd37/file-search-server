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
#include <fstream>

#include "read_input_functions.h"
#include "string_processing.h"
#include "document.h"
#include "search_server.h"
#include "file_reader.h"
#include "txtfile_scanner.h"
//НЕ ЗАБЫТЬ ПОЧИСТИТЬ ИНКЛЮДЫ И ОТСОРТИРОВАТЬ!
using namespace std;

void PrintDocument(const Document& document) {
    cout << "{ "s
         << "document_id = "s << document.id << ", "s
         << "relevance = "s << document.relevance << ", "s
         << "path = "s << document.path << " }"s << endl;
}

ostream& operator<<(ostream& os, const Document& doc) {
    os << "{ "s
         << "document_id = "s << doc.id << ", "s
         << "relevance = "s << doc.relevance << ", "s
         << "path = "s << doc.path << " }"s << endl;
    return os;
}

ostream& operator<<(ostream& os, const vector<Document>& docs) {
    for (const auto& doc : docs) {
        os << doc;
    }
    return os;
}

//создание тестовой папки, пока втупую
void CreateTestDocuments(const filesystem::path& directory) {
    try {
        if(!filesystem::exists(directory)) {
            filesystem::create_directory(directory);
        }
        std::ofstream file1(directory / "cat.txt");
        if (file1) {
            file1  << "cat meets dog at the park in the morning"s;
        }
        file1.close();

        std::ofstream file2(directory / "dog.txt");
        if (file2) {
            file2  << "dog wants to meet cat but want to sleep"s;
        }
        file2.close();

        std::ofstream file3(directory / "programmer.txt");
        if (file3) {
            file3  << "i want to code using C++ so i try my best and seems like i can do it"s;
        }
        file3.close();

        std::ofstream file4(directory / "parrot.txt");
        if (file4) {
            file4  << "sweet parrot came to the park and watched at the sad lonely cat"s;
        }
        file4.close();

    } catch (const filesystem::filesystem_error& err) {
        cerr << "Cought error: " << err.what() << endl;
    }

}

SearchServer CreateSearchServerFromIndex(const string& path) {
    ifstream in(path);
    string stop_words_str;
    getline(in, stop_words_str);
    SearchServer server(stop_words_str);
    server.LoadIndex(path);
    return server;
}

//тесты втупую
int main() {
    bool loaded = false;
    SearchServer searchserver = [&]() {
        if (filesystem::exists("C:/Users/WRKR/Desktop/index.txt")) {
            cout << "Index file found and loaded!" << endl;
            loaded = true;
            return CreateSearchServerFromIndex("C:/Users/WRKR/Desktop/index.txt");
        } else {
            cout << "Index file not found, initializing new search server" << endl;
            return SearchServer("the a an in on at to and or but"s);
        }
    }();
    
    if (!loaded) {
        filesystem::path dir = "test_directory"s;
        CreateTestDocuments(dir);
        int id = 0;

        for (const auto& file : TxtFinder(dir)) {
            searchserver.AddDocument(++id, ReadTxtFileContent(file), file.string());
            cout << "File " << file.string() << "got id - " << id << endl;
        }
    }

    const auto Found1 = searchserver.FindTopDocuments("cat dog");
    cout << "With query \"cat dog\" found " << Found1.size() << " docs, which is:" << endl;
    for (const auto& doc : Found1) {
        PrintDocument(doc);
    }

    const auto Found2 = searchserver.FindTopDocuments("C++ code");
    cout << "With query \"C++ code\" found " << Found2.size() << " docs, which is:" << endl;
    for (const auto& doc : Found2) {
        PrintDocument(doc);
    }

    const auto Found3 = searchserver.FindTopDocuments("parrot");
    cout << "With query \"parrot\" found " << Found3.size() << " docs, which is:" << endl;
    for (const auto& doc : Found3) {
        PrintDocument(doc);
    }

    const auto Found4 = searchserver.FindTopDocuments("not empty query");
    cout << "With query \"not empty query\" found " << Found4.size() << " docs, which is:" << endl;
    for (const auto& doc : Found4) {
        PrintDocument(doc);
    }

    searchserver.SaveIndex("C:/Users/WRKR/Desktop/index.txt");
    return 0;
}