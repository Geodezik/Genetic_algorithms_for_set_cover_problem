#ifndef BOOLEANMATRIX_H
#define BOOLEANMATRIX_H

#include <iostream>
#include <vector>

namespace BooleanMatrix {
    class BooleanMatrix;
}

class BooleanMatrix::BooleanMatrix {
public:
    int m;
    int n;
    bool** M;
    BooleanMatrix(int m, int n);
    BooleanMatrix(const BooleanMatrix& B);
    int get_m();
    int get_n();
    bool is_covered_by(std::vector<bool>& columns);
    bool* operator[](int);
    ~BooleanMatrix();

};

#endif