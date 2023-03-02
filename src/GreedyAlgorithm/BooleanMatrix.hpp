#ifndef BOOLEANMATRIX_H
#define BOOLEANMATRIX_H

#include <iostream>

namespace BooleanMatrix {
    class BooleanMatrix;
}

class BooleanMatrix::BooleanMatrix {
    int m;
    int n;
public:
    bool** M;
    BooleanMatrix(int m, int n);
    BooleanMatrix(const BooleanMatrix& B);
    int get_m();
    int get_n();
    bool* operator[](int);
    ~BooleanMatrix();

};

#endif