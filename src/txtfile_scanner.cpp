#include <algorithm>
#include <filesystem>
#include <vector>
#include <iostream>
#include "txtfile_scanner.h"

using namespace std;

vector<filesystem::path> FindEntries() {
    /*суть этой функции в том, что если в команду index не передана директория - вызывается поиск точек входа:
    а именно находятся входы на диски, определяется их количество,
    затем уже эти входы идут на то же рекурсивное сканирование и мы получаем на выходе полный список текстовых файлов,
    а в перспективе и других файлов 
    ПОКА СПЕЦИАЛИЗИРОВАНА ПОД WINDOWS, В ПЛАНАХ ВЕРНУТЬ КРОССПЛАТФОРМЕННОСТЬ,
    так как именно эта функция режет способность приложения работать на любой ОС*/
    error_code err;
    vector<filesystem::path> result;
    for (char i = 'A'; i <= 'Z'; i++) {
        filesystem::path name = string(1,i) + ":\\";
        if (filesystem::exists(name, err)) {
            result.push_back(name);
        } 
    }
    return result;
}

vector<filesystem::path> TxtFinder(const filesystem::path& dir) {
    vector<filesystem::path> result;
    const vector<string> excluding = {
        "index.txt", "CMakeLists.txt", "CMakeCache.txt",
        "build", ".git", ".vscode", ".vs",
        "System Volume Information", "$Recycle.Bin", "MSOCache",
        "Windows", "Program Files", "Program Files (x86)",
        "ProgramData", "Recovery", "Config.Msi",
        "Temp", "OneDriveTemp", "PerfLogs",
        "Documents and Settings",
        "AppData", "Application Data", "Local Settings",
        "Cookies", "NetHood", "PrintHood", "Recent", "SendTo",
        "Temporary Internet Files", "History",
        "Dev", "XAMPP", "Packages", "Intel", "Google", "Microsoft",
        "MicrosoftEdge", "OneDrive"
    }; 
    if (!filesystem::exists(dir) || !filesystem::is_directory(dir))  {
        return result;
    }
    
    try {
        for (const auto& entry : filesystem::directory_iterator(dir)) {
            try {
                string name = entry.path().filename().string();
                if (find(excluding.begin(), excluding.end(), name) != excluding.end()) {
                    continue;
                }
                if (name.empty() || name[0] == '.') {
                    continue;
                }
                if (entry.is_symlink()) {
                    continue;
                }
                if (entry.is_directory()) {
                    for (const auto& found : TxtFinder(entry.path())) {
                        result.push_back(found);
                    }
                }
                if ((entry.is_regular_file()) && (entry.path().extension() == ".txt")) {
                    result.push_back(entry.path());
                }
            } catch (const filesystem::filesystem_error& err) {
                cerr << "Caught error: " << err.what() << endl;
            }
        }
    } catch (const filesystem::filesystem_error& err) {
        //cerr << "Caught error: " << err.what() << endl;
    }

    return result;
}