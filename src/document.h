#pragma once
#include <string>

struct Document {
    Document();

    Document(int id, double relevance, std::string path);

    int id = 0;
    double relevance = 0.0;
    std::string path;
};