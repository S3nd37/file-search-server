#pragma once

struct Document {
    Document();

    Document(int id, double relevance);

    int id = 0;
    double relevance = 0.0;
};