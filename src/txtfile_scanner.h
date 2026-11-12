#pragma once

#include <filesystem>
#include <vector>

std::vector<std::filesystem::path> FindEntries();

std::vector<std::filesystem::path> TxtFinder(const std::filesystem::path& dir);