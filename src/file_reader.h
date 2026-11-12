#pragma once

#include <string>
#include <filesystem>
//описание функции чтения файлов.
std::string ReadTxtFileContent(const std::filesystem::path& path);