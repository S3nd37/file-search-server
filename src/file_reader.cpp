#include <string>
#include <filesystem>
#include <fstream>

#include "file_reader.h"

std::string ReadTxtFileContent(const std::filesystem::path& path) {
    std::string result;
    std::string line;
    std::ifstream in(path);
    
    if (!in) {
        return result;
    }
    //требуется запись многострочного файла в строку, хотя вроде и так норм будет
    //ВЕРНУТЬСЯ, ПОДУМАТЬ
    while (std::getline(in, line)) {
        result += line + " ";
    }

    return result;
}