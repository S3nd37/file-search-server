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
//Уж либо убрать ненужное, либо использовать, а не забывать
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

//тесты втупую
int main() {
    SearchServer searchserver("the a an in on at to and or but"s);
    filesystem::path dir = "test_directory"s;
    CreateTestDocuments(dir);
    int id = 0;

    for (const auto& file : TxtFinder(dir)) {
        searchserver.AddDocument(++id, ReadTxtFileContent(file));
        cout << "File " << file.string() << "got id - " << id << endl;
    }

    //ВЫШЕ ЕСТЬ ФУНКЦИЯ ВЫВОДА DOCUMENT, ОТСЮДА УБРАТЬ САМОПАЛЬНЫЙ ВЫВОД, НЕ ЗАБЫВАТЬ ФУНКЦИЮ
    const auto Found1 = searchserver.FindTopDocuments("cat dog");
    cout << "With query \"cat dog\" found " << Found1.size() << " docs, which is:" << endl;
    for (const auto& doc : Found1) {
        cout << "{ id = " << doc.id << ", relevance = " << doc.relevance << " }" << endl;
    }

    const auto Found2 = searchserver.FindTopDocuments("C++ code");
    cout << "With query \"C++ code\" found " << Found2.size() << " docs, which is:" << endl;
    for (const auto& doc : Found2) {
        cout << "{ id = " << doc.id << ", relevance = " << doc.relevance << " }" << endl;
    }

    const auto Found3 = searchserver.FindTopDocuments("parrot");
    cout << "With query \"parrot\" found " << Found3.size() << " docs, which is:" << endl;
    for (const auto& doc : Found3) {
        cout << "{ id = " << doc.id << ", relevance = " << doc.relevance << " }" << endl;
    }

    const auto Found4 = searchserver.FindTopDocuments("not empty query");
    cout << "With query \"not empty query\" found " << Found4.size() << " docs, which is:" << endl;
    for (const auto& doc : Found4) {
        cout << "{ id = " << doc.id << ", relevance = " << doc.relevance << " }" << endl;
    }
    return 0;
}