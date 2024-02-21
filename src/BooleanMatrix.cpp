#include "BooleanMatrix.hpp"

BooleanMatrix::BooleanMatrix::BooleanMatrix(int m, int n) {
    this->m = m;
    this->n = n;
    rows = std::vector<boost::dynamic_bitset<>>();
    columns = std::vector<boost::dynamic_bitset<>>();
    for (int i = 0; i < m; i++)
        rows.push_back(boost::dynamic_bitset<>(n, 0));
    //for (int j = 0; j < n; j++)
        //columns.push_back(boost::dynamic_bitset<>(m, 0));
}

int BooleanMatrix::BooleanMatrix::get_m() {
    return m;
}

int BooleanMatrix::BooleanMatrix::get_n() {
    return n;
}

bool BooleanMatrix::BooleanMatrix::is_covered_by(boost::dynamic_bitset<>& columns) {
    for(int i = 0; i < m; i++) {
        bool flag = false;
        for(int j = 0; j < n; j++) {
            // gene in set and elem in M is 1
            if(columns[j] && rows[i][j]) {
                flag = true;
                break;
            }
        }
        if(!flag)
            return false;
    }

    return true;
}

BooleanMatrix::BooleanMatrix::~BooleanMatrix() {}

bool BooleanMatrix::BooleanMatrix::get(int i, int j) {
    return rows[i][j];
}

void BooleanMatrix::BooleanMatrix::set(int i, int j, bool val) {
    rows[i][j] = val;
    //columns[j][i] = val;
}
