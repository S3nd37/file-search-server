#pragma once

bool IsPunctuation(char ch);

std::vector<std::string> SplitIntoWords(const std::string& text);

std::string ToLower(const std::string& str);

template <typename StringContainer>
std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string> non_empty_strings;
    for (const std::string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(ToLower(str));
        }
    }
    return non_empty_strings;
}

std::string TrimPath(const std::string& input);