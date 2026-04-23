#include <filesystem>
#include <vector>
#include <iostream>
#include "txtfile_scanner.h"

std::vector<std::filesystem::path> TxtFinder(const std::filesystem::path& dir) {
    std::vector<std::filesystem::path> result;
    if (!std::filesystem::exists(dir)) {
        return result;
    }

    if (!std::filesystem::is_directory(dir)) {
        return result;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if ((entry.is_regular_file()) && (entry.path().extension() == ".txt")) {
                result.push_back(entry.path());
            }
        }
    } catch (const std::filesystem::filesystem_error& err) {
        std::cerr << "Caught error: " << err.what() << std::endl;
    }

    return result;
}