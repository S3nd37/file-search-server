#pragma once
#include <string>
//здесь содержится описание структуры документов.
struct Document {
    Document();
    //конструкторы с пустыми параметрами и при заданных.
    Document(int id, double relevance, std::string path);

    int id = 0;
    double relevance = 0.0;
    std::string path;
};