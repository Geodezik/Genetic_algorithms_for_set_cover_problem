#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <chrono>
#include <numeric>      // std::iota
#include <algorithm>    // std::sort, std::stable_sort

#include <iostream>
#include <vector>
#include <set>
#include <time.h>
#include <string>
#include <iomanip>
#include <chrono>
#include <bitset>
#include <unordered_map>

#include <boost/dynamic_bitset.hpp>


using namespace std;
using namespace boost;
// const int m = 20, n = 13;
// using BitRow = std::bitset<n>;
// using BitCol = std::bitset<m>;


// struct BitMatrix
// {
//     vector<BitRow> row_mat = vector<BitRow>(m);
//     vector<BitCol> col_mat = vector<BitCol>(n);
// };
struct BitMatrix
{
    vector<dynamic_bitset<unsigned char>> row_mat;
    vector<dynamic_bitset<unsigned char>> col_mat;
};


struct ItemSet {
	std::vector<int> itemset;
    dynamic_bitset<unsigned char> conjuction;
	// std::bitset<m> conjuction;
	bool terminal = true;
};


// vector<BitMatrix> GenerateInput() {
//     int count = 10;

//     vector<BitMatrix> res;
//     while(count--) {
//         bool bad = false;
//         vector<std::bitset<n>> row_mat(m);
//         vector<std::bitset<m>> col_mat(n);
//         for (int i = 0; i < m; ++i) {
//             row_mat[i] = std::bitset<n>(0);
//         }
//         for (int j = 0; j < (n / 2); ++j) {
//             col_mat[2 * j] = std::bitset<m>(0);
//             col_mat[2 * j + 1] = std::bitset<m>(0);
//         }
//         for (int i = 0; i < m; ++i) {
//             for (int j = 0; j < (n / 2); ++j) {
//                 if (rand() % 2) {
//                     row_mat[i].set(2 * j + 1);
//                     col_mat[2 * j + 1].set(i);
//                 }
//                 else {
//                 	row_mat[i].set(2 * j);
//                     col_mat[2 * j].set(i);
//                 }
//             }
//             if (row_mat[i].count() == 0) {
//                 bad = true;
//             }
//         }
//         if (bad) {
//             ++count;
//             continue;
//         }
//         BitMatrix now = {row_mat, col_mat};
//         res.push_back(std::move(now));
//         }

//     return res;
// }


struct Matrix {
	int num_rows, num_cols;
	std::vector<bool> flat_matrix;
	Matrix(int, int);
	// bool& operator()(int, int);
	bool operator()(int, int) const;
};

Matrix::Matrix(int m=0, int n=0): num_rows(m), num_cols(n) {
	int size = m * n;
	for (int i = 0; i < size; i++) {
		bool elem = rand() % 2;
		flat_matrix.push_back(elem);
	}
};

using EC = std::vector<int>;

bool Matrix::operator()(int i, int j) const {
	return flat_matrix[i * num_cols + j];
};

bool is_freq(Matrix& M, EC& e, int rank) {
	int cnt = 0;
	for (int i = 0; i < M.num_rows; i++) {
		int value = 1;
		for (int j : e) {
			if (M(i, j) == 0) {
				value = 0;
				break;
			}
		}
		cnt += value;
		if (cnt >= rank) {
			return true;
		}
	}
	return false;
}

bool is_minimal_cov(Matrix& M, EC& e) {
	int r = 1;
	if (!is_freq(M, e, r)) {
		if (e.size() == 1) {
			return true;
		}
		for (int i = 0; i < e.size(); i++) {
			auto tmp = e;
			tmp.erase(tmp.begin() + i);
			if (!is_freq(M, tmp, r)) {
				return false;
			}
		}
		return true;
	}
	return false;
}


void depth_search_adr(ItemSet& current_node, std::vector<ItemSet>& freq, std::vector<ItemSet>& regular, int s) {
	auto max = std::max_element(current_node.itemset.begin(), current_node.itemset.end());
	bool t = true;

	if (current_node.itemset.size() == s) {
		regular.push_back(current_node);
	}
	else {
		for (ItemSet item : freq) {
			if (*max < item.itemset[0]) {
				auto tmp = current_node;
				tmp.itemset.push_back(item.itemset[0]);
				tmp.conjuction &= item.conjuction;
				if (tmp.conjuction.count() >= s) {
					depth_search_adr(tmp, freq, regular, s);
				}
			}
		}
	}
}

std::vector<ItemSet> ADR(BitMatrix& input, int p) {
    int m = input.row_mat.size(), n = input.col_mat.size();
	std::vector<ItemSet> freq;
    std::vector<ItemSet> regular;

    for (int j = 0; j < n; ++j) {
    	if (input.col_mat[j].count() >= p) {
    		ItemSet tmp;
    		tmp.itemset.push_back(j);
    		tmp.conjuction = input.col_mat[j];
    		freq.push_back(tmp);
    	}
    }

    for (auto elem : freq) {
    	depth_search_adr(elem, freq, regular, p);
    }
    return regular;
}

std::ofstream out_1("./data/scores_0_irr.txt", ios::app);
std::ofstream out_2("./data/scores_0_pred.txt", ios::app);
std::ifstream x_k("./data/X_k.txt");
std::ifstream x_no("./data/X_no.txt");
std::ifstream x_te("./data/X_test.txt");

void printMat(const BitMatrix& mat)
{
    for (const auto& row : mat.row_mat) {
        cout << row << endl;
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cout << "You must specify one command line argument - p" << endl;
        return 0;
    }
	int p = atoi(argv[1]);
	int ch;
	
	// BitMatrix X_k = GenerateInput()[0];
	// for (int i = 0; i < m; i++) {
	// 	for (int j = 0; j < n; j++) {
	// 		x_k >> ch;
	// 		X_k.row_mat[i][j] = ch;
	// 		X_k.col_mat[j][i] = ch;
	// 	}
	// }

    /* Building X_k */
    BitMatrix X_k;
    string row;
    while(getline(x_k, row)) {
        row.erase(remove(row.begin(), row.end(), ' '), row.end());
        reverse(row.begin(), row.end()); // bitset[0] -> first column
        X_k.row_mat.push_back(dynamic_bitset<unsigned char>(row));
    }
    for (size_t j = 0; j < X_k.row_mat[0].size(); ++j) {
        X_k.col_mat.push_back(dynamic_bitset<unsigned char>(X_k.row_mat.size(), 0));
        for (size_t i = 0; i < X_k.row_mat.size(); ++i) {
            X_k.col_mat[j][i] = X_k.row_mat[i][j];
        }
    }


	// Matrix X_no(m, n);
	// for (int i = 0; i < m * n; i++) {
	// 	x_no >> ch;
	// 	X_no.flat_matrix[i] = ch;
	// }

    /* Building X_no */
    Matrix X_no;
    int num_rows = 0, num_cols = 0;
    while(getline(x_no, row)) {
        num_rows++;
        num_cols = 0;
        row.erase(remove(row.begin(), row.end(), ' '), row.end());
        for (const char& digit : row) {
            X_no.flat_matrix.push_back(digit - '0');
            num_cols++;
        }
    }
    X_no.num_rows = num_rows;
    X_no.num_cols = num_cols;

	// Matrix X_te(m, n);
	// for (int i = 0; i < m * n; i++) {
	// 	x_te >> ch;
	// 	X_te.flat_matrix[i] = ch;
	// }

    /* Building X_te */
    Matrix X_te;
    num_rows = 0, num_cols = 0;
    while(getline(x_te, row)) {
        row.erase(remove(row.begin(), row.end(), ' '), row.end());
        num_rows++;
        num_cols = 0;
        for (const char& digit : row) {
            X_te.flat_matrix.push_back(digit - '0');
            num_cols++;
        }
    }
    X_te.num_rows = num_rows;
    X_te.num_cols = num_cols;

    // cout << X_te.flat_matrix.size() << "pato" << endl;


	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::vector<ItemSet> result = ADR(X_k, p);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	auto total = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin);

	std::vector<float> scores_1(X_te.num_rows, 0);
	std::vector<float> scores_2(X_te.num_rows, 0);
	int cnt_1 = 0;
	int cnt_2 = 0;
	std::chrono::steady_clock::time_point begin_1 = std::chrono::steady_clock::now();
	for (auto elem : result) {
		int r = 1;
		if (is_minimal_cov(X_no, elem.itemset)) {
			int frequency = elem.conjuction.count();
			cnt_1 += frequency;
			for (int i = 0; i < X_te.num_rows; i++) {
				int value = frequency;
				for (int j : elem.itemset) {
					if (X_te(i, j) == 0) {
						value = 0;
						break;
					}
				}
				scores_1[i] += value;
			}
		}
	}
	std::chrono::steady_clock::time_point begin_2 = std::chrono::steady_clock::now();
	for (auto elem : result) {
		int r = 1;
		if (!is_freq(X_no, elem.itemset, r)) {
			int frequency = elem.conjuction.count();
			cnt_2 += frequency;
			for (int i = 0; i < X_te.num_rows; i++) {
				int value = frequency;
				for (int j : elem.itemset) {
					if (X_te(i, j) == 0) {
						value = 0;
						break;
					}
				}
				scores_2[i] += value;
			}
		}
	}
	std::chrono::steady_clock::time_point end_2 = std::chrono::steady_clock::now();

	out_1 << std::setprecision(10) << (total + std::chrono::duration_cast<std::chrono::nanoseconds> (begin_2 - begin_1)).count() / 1'000'000 << " | ";
	out_2 << std::setprecision(10) << (total + std::chrono::duration_cast<std::chrono::nanoseconds> (end_2 - begin_2)).count() / 1'000'000 << " | ";
	if (cnt_1 != 0) {
		for (int i = 0; i < scores_1.size(); i++){
			scores_1[i] /= cnt_1;
		}
	}
	if (cnt_2 != 0) {
		for (int i = 0; i < scores_2.size(); i++){
			scores_2[i] /= cnt_2;
		}
	}
	out_1 << cnt_1 << " | ";
	for (float score : scores_1) {
			out_1 << score << ' ';
		}
	out_1 << std::endl;
	out_1.close();

	out_2 << cnt_2 << " | ";
	for (float score : scores_2) {
			out_2 << score << ' ';
		}
	out_2 << std::endl;
	out_2.close();
}