#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <fstream>

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
        std::ofstream file1(directory / "Register.txt");
        if (file1) {
            file1  << "ReGiStEr iS nOt ThE pRoBlEm"s;
        }
        file1.close();

        std::ofstream file2(directory / "Pushkin.txt");
        if (file2) {
            file2  << "A captive, alone in a dungeon I dwell, \n"
                      "Entombed in the stillness and murk of a cell. \n"
                      "Outside, in the courtyard, in wild, frenzied play, \n"
                      "My comrade, an eagle, has pounced on his prey. \n\n"
                      "Then, leaving the blood-spattered morsel, his eye, \n"
                      "He fixes on me with a dolorous cry, \n"
                      "A cry that is more like a call or a plea- \n"
                      "\"\'Tis time,\" he is saying, \"\'tis time, let us flee! \n"
                      "\"We\'re both wed to freedom, so let us away \n\n"
                      "To where lonely storm-clouds courageously stray, \n"
                      "Where turbulent seas rush to merge with the sky, \n"
                      "Where only the winds dare to venture and I!...\""s;
        }
        file2.close();

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
    string unicom = ToLower(com);
    if (unicom == "exit") {return Command::Exit;}
    if (unicom == "index") {return Command::Index;}
    if (unicom == "search") {return Command::Search;}
    if (unicom == "help") {return Command::Help;}
    if (unicom == "test") {return Command::Test;}
    return Command::Unknown;
}

void WriteHelpBlock() {
    cout << "There are commands to use program:" << endl;
    cout << "exit - close program" << endl;
    cout << "help - view commands" << endl;
    cout << "index <path> - index .txt files from directory <path>, leave empty to index testing directory" << endl;
    cout << "search <query> - search, type your query instead of <query>" << endl;
    cout << "This is working prototype, program will be upgraded," << endl;
    cout << "Thank you for trying my project." << endl;
}

SearchServer DoIndex(const filesystem::path& path) {
    SearchServer server("the a an in on at to and or but"s);
    int id = 0;
    for (const auto& file : TxtFinder(path)) {
        server.AddDocument(++id, ReadTxtFileContent(file), file.string());
        cout << "File " << file.string() << " got id - " << id << endl;
    }
    server.SaveIndex("index.txt");
    return server;
}

void TestingBlock(const SearchServer& server) {
    {
        const auto Found = server.FindTopDocuments("ReGiStEr");
        cout << "With query \"ReGiStEr\" found " << Found.size() << " docs, which is:" << endl;
        cout << Found;
    }

    {
        const auto Found = server.FindTopDocuments("DUNGEON");
        cout << "With query \"DUNGEON\" found " << Found.size() << " docs, which is:" << endl;
        cout << Found;
    }

    {
        const auto Found = server.FindTopDocuments("The problem is register. Not the captive -alone");
        cout << "With query \"The problem is register. Not the captive -alone\" found " << Found.size() << " docs, which is:" << endl;
        cout << Found;
    }

    {
        const auto Found = server.FindTopDocuments("not empty query");
        cout << "With query \"not empty query\" found " << Found.size() << " docs, which is:" << endl;
        cout << Found;
    }

    {
        const auto Found = server.FindTopDocuments("-");
        cout << "With query \"-\" found " << Found.size() << " docs, which is:" << endl;
        cout << Found;
    }

    {
        const auto Found = server.FindTopDocuments("");
        cout << "With query \"\" found " << Found.size() << " docs, which is:" << endl;
        cout << Found;
    }

    {
        const auto Found = server.FindTopDocuments("register -register");
        cout << "With query \"register -register\" found " << Found.size() << " docs, which is:" << endl;
        cout << Found;
    }
}

int main() {
    cout << "----===== File Search Server has started =====----" << endl << "Type \"help\" to view usage." << endl;
    SearchServer searchserver = [&]() {
        if (filesystem::exists("index.txt")) {
            cout << "Index file found and loaded!" << endl;
            return CreateSearchServerFromIndex("index.txt");
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
                    string directory = dir.string();
                    string input_path;
                    getline(cin, input_path);
                    if ((!input_path.empty())) {
                        const auto first_smbl = find_if(input_path.begin(), input_path.end(), [](const auto& el) {
                                                                                                if (el != ' ') {
                                                                                                    return true;
                                                                                                }
                                                                                                return false;
                                                                                            });
                        if (first_smbl != input_path.end()) {
                            int pos = distance(input_path.begin(), first_smbl);
                            directory = input_path.substr(pos);
                            cout << "You chose to index directory " << directory << "." << endl;
                        }
                    }
                    searchserver = DoIndex(directory);
                    break;
                }
                case Command::Search: {
                    string query;
                    getline(cin, query);
                    const vector<Document> Found = searchserver.FindTopDocuments(query);
                    if (Found.empty()) {
                        cout << "Nothing found with query \"" << query << "\"." << endl;
                    } else {
                        cout << Found;
                    }
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
            string trash;
            getline(cin, trash);
            cout << "Unknown command, see \"help\"" << endl;
            break;
        }
    }


    return 0;
}