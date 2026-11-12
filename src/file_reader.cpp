#include <string>
#include <filesystem>
#include <fstream>

#include "file_reader.h"
//здесь содержится реализация функции чтения текстовых файлов. В дальнейшем - планируется добавить больше форматов для обработки.
std::string ReadTxtFileContent(const std::filesystem::path& path) {
    std::string result;
    std::string line;
    std::ifstream in(path);
    
    if (!in) {
        return result;
    }
    
    while (std::getline(in, line)) {
        result += line + " ";
    }

    return result;
}