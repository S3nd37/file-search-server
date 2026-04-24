#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <fstream>

#include "read_input_functions.h"
#include "string_processing.h"
#include "document.h"
#include "search_server.h"
#include "file_reader.h"
#include "txtfile_scanner.h"

using namespace std;
using std::cout;

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

//создание тестовой папки с документами
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

enum class Command {
    Exit,
    Index,
    Search,
    Help,
    Test,
    Unknown
};

Command IdentifyCommand(const string& com) {
    if (com == "exit") {return Command::Exit;}
    if (com == "index") {return Command::Index;}
    if (com == "search") {return Command::Search;}
    if (com == "help") {return Command::Help;}
    if (com == "test") {return Command::Test;}
    return Command::Unknown;
}

void WriteHelpBlock() {
    cout << "There are commands to use program:" << endl;
    cout << "exit - type to close program" << endl;
    cout << "help - view commands" << endl;
    cout << "index - reindex or index .txt files from testing directory" << endl;
    cout << "search <query> - search, type your query instead of <query>" << endl;
    cout << "This is working prototype, program will be upgraded," << endl;
    cout << "Thank you for trying my project." << endl;
}

SearchServer DoIndex(const filesystem::path& path) {
    SearchServer server("the a an in on at to and or but"s);
    int id = 0;
    for (const auto& file : TxtFinder(path)) {
        server.AddDocument(++id, ReadTxtFileContent(file), file.string());
        cout << "File " << file.string() << "got id - " << id << endl;
    }
    server.SaveIndex("C:/Users/WRKR/Desktop/index.txt");
    return server;
}

void TestingBlock(const SearchServer& server) {
    const auto Found1 = server.FindTopDocuments("cat dog");
    cout << "With query \"cat dog\" found " << Found1.size() << " docs, which is:" << endl;
    cout << Found1;

    const auto Found2 = server.FindTopDocuments("C++ code");
    cout << "With query \"C++ code\" found " << Found2.size() << " docs, which is:" << endl;
    cout << Found2;

    const auto Found3 = server.FindTopDocuments("parrot");
    cout << "With query \"parrot\" found " << Found3.size() << " docs, which is:" << endl;
    cout << Found3;

    const auto Found4 = server.FindTopDocuments("not empty query");
    cout << "With query \"not empty query\" found " << Found4.size() << " docs, which is:" << endl;
    cout << Found4;
}

int main() {
    cout << "--== File Search Server has started ==--" << endl << "Type \"help\" to view usage" << endl;
    SearchServer searchserver = [&]() {
        if (filesystem::exists("C:/Users/WRKR/Desktop/index.txt")) {
            cout << "Index file found and loaded!" << endl;
            return CreateSearchServerFromIndex("C:/Users/WRKR/Desktop/index.txt");
        } else {
            cout << "Index file not found, initializing new search server" << endl;
            return SearchServer("the a an in on at to and or but"s);
        }
    }();

    filesystem::path dir = "test_directory"s;
    if (!filesystem::exists(dir)) {
        cout << "Testing directory missing, let me create it" << endl;
        CreateTestDocuments(dir);
    }

    string command;
    while (cin >> command) {
        Command cmd = IdentifyCommand(command);
        switch (cmd) {
                case Command::Exit: {
                    return 0;
                }
                case Command::Index: {
                    searchserver = DoIndex(dir);
                    break;
                }
                case Command::Search: {
                    cin.ignore();
                    string query;
                    getline(cin, query);
                    cout << searchserver.FindTopDocuments(query);
                    break;
                }
                case Command::Help: {
                    WriteHelpBlock();
                    break;
                }
                case Command::Test: {
                    TestingBlock(searchserver);
                    break;
                }
        default:
            break;
        }
    }


    return 0;
}