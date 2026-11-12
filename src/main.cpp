#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <cstdlib>

#include "string_processing.h"
#include "document.h"
#include "search_server.h"
#include "file_reader.h"
#include "txtfile_scanner.h"

using namespace std;
using std::cout;
using std::getline;

//удобные операторы вывода документа и вектора документов
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


//восстановление файла из текстового "слепка" в index.txt
SearchServer CreateSearchServerFromIndex(const string& path) {
    ifstream in(path);
    string stop_words_str;
    getline(in, stop_words_str);
    SearchServer server(stop_words_str);
    server.LoadIndex(path);
    return server;
}

//доступные команды
enum class Command {
    Exit,
    Index,
    Search,
    Help,
    Test,
    SetStopWords,
    Unknown
};
//функция для идентификации команды
Command IdentifyCommand(const string& com) {
    string unicom = ToLower(com);
    if (unicom == "exit") {return Command::Exit;}
    if (unicom == "index") {return Command::Index;}
    if (unicom == "search") {return Command::Search;}
    if (unicom == "help") {return Command::Help;}
    if (unicom == "test") {return Command::Test;}
    if (unicom == "setstopwords") {return Command::SetStopWords;}
    return Command::Unknown;
}
//вывод при команде help
void WriteHelpBlock() {
    cout << "There are commands to use program:" << endl << endl;
    cout << "exit           -   close program" << endl << endl;
    cout << "help           -   view commands" << endl << endl;
    cout << "index <path>   -   index .txt files from directory <path>, leave empty to index all (only for Windows)" << endl << endl;
    cout << "search <query> -   type your query instead of <query> to search" << endl << endl;
    cout << "setstopwords   -   to set your stop-words, do not type your words until asked" << endl;
}
//создание индекса в оперативной памяти и выгрузка "слепка" в index.txt
SearchServer DoIndex(const filesystem::path& path, SearchServer& server) {
    int id = 0;
    SearchServer new_server(server.GetStopWords());
    new_server.SetIndexedDir(server.GetIndexedDir());
    for (const auto& file : TxtFinder(path)) {
        new_server.AddDocument(++id, ReadTxtFileContent(file), file.string());
        //cout << "File " << file.string() << " got id - " << id << endl;
    }
    new_server.SaveIndex("index.txt");
    cout << "Indexed." << endl;
    return new_server;
}
//делает то же самое, но для группы входных папок
SearchServer DoIndex(const vector<filesystem::path>& roots, SearchServer& server) {
    int id = 0;
    SearchServer new_server(server.GetStopWords());
    new_server.SetIndexedDir(server.GetIndexedDir());
    for (const auto& root : roots) {
        for (const auto& file : TxtFinder(root)) {
            new_server.AddDocument(++id, ReadTxtFileContent(file), file.string());
            //cout << "File " << file.string() << " got id - " << id << endl;
        }
    }

    new_server.SaveIndex("index.txt");
    cout << "Indexed." << endl;
    return new_server;
}

//блок тестирования, скрытая команда test
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

void IndexCom(const string& input_path, SearchServer& searchserver) {
    string directory = TrimPath(input_path);
    if ((!directory.empty())) {
        cout << "You chose to index " << directory << "." << endl;
            if (filesystem::exists(directory) && filesystem::is_directory(directory)) {
                searchserver.SetIndexedDir(directory);
                searchserver = DoIndex(directory, searchserver);
            } else {
                cout << "Can't find this directory." << endl;
            }                   
    } else {
        searchserver.SetIndexedDir("all");
        searchserver = DoIndex(FindEntries(), searchserver);
    }
}

int main() {
    cout << "----===== File Search Server has started =====----" << endl << "Type \"help\" to view usage." << endl;
    //дабы всегда было что индексировать - при запуске и отсутствии тестовой директории будет создана новая.
    //инициализация класса поискового ядра + настройка стоп-слов по желанию пользователя
    SearchServer searchserver = [&]() {
        if (filesystem::exists("index.txt")) {
            cout << "Index file found and loaded!" << endl;
            return CreateSearchServerFromIndex("index.txt");
        } else {
            cout << "Index file not found, initializing new search server" << endl;
            string stp_wrds = "the a an in on at to and or but"s;
            cout << "Wish to set your stop-words? (y/n)"s << endl;
            string respond;
            cin >> respond;
            while (ToLower(respond) != "y" && ToLower(respond) != "n") {
                cout << "Invalid answer. Please enter y for yes and n for no" << endl;
                cin >> respond;
            }
            string trash;
            getline(cin, trash);
            if (ToLower(respond) == "y") {
                cout << "Please enter your stop-words divided by spaces" << endl;
                getline(cin, stp_wrds);

            } else if (ToLower(respond) == "n") {
                cout << "Stop-words will be set by default" << endl;
            }
            SearchServer server(stp_wrds);
            cout << "Stop-words set" << endl;
            return server;
        }
    }();
    //код интерфейса. здесь происходит реакция на соответствующие команды
    string command;
    while (cin >> command) {
        Command cmd = IdentifyCommand(command);
        switch (cmd) {
                case Command::Exit: {
                    return 0;
                }
                case Command::Index: {
                    string input_path;
                    getline(cin, input_path);
                    IndexCom(input_path, searchserver);   
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
                    {
                    WriteHelpBlock();
                    break;
                    }
                }
                case Command::Test: {
                    TestingBlock(searchserver);
                    break;
                }
                case Command::SetStopWords: {
                    string stp_wrds = "the a an in on at to and or but"s;
                    cout << "You wish to set your stop-words. Enter new stop-words divided by spaces:"s << endl;
                    string trash;
                    getline(cin, trash);
                    getline(cin, stp_wrds);
                    searchserver.SetStopWords(stp_wrds);
                    string dir = searchserver.GetIndexedDir();
                    if (dir.empty()) {
                        cout << "Stop words set, now indexing can be made" << endl;
                    } else if (dir != "all") {
                        searchserver = DoIndex(dir, searchserver);
                    } else {
                        IndexCom("", searchserver);
                    }
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