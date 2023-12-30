#ifndef BOOLEANMATRIX_H
#define BOOLEANMATRIX_H

#include <iostream>
#include <vector>
#include <boost/dynamic_bitset.hpp>

namespace BooleanMatrix {
    class BooleanMatrix;
}

class BooleanMatrix::BooleanMatrix {
public:
    int m;
    int n;
    std::vector<boost::dynamic_bitset<>> rows;
    std::vector<boost::dynamic_bitset<>> columns;

    BooleanMatrix(int m, int n);
    int get_m();
    int get_n();
    bool is_covered_by(boost::dynamic_bitset<>& columns);
    bool get(int i, int j);
    void set(int i, int j, bool val);
    ~BooleanMatrix();

};

#endif