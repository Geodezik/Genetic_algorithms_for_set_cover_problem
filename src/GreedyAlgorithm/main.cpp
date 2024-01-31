#include "Greedy.hpp"
#include "BooleanMatrix.hpp"
#include "misc.cpp"
#include <random>
#include <algorithm>
#include <numeric>
#include <vector>

using namespace Greedy;

int main()
{
    std::string filename = "../../data/titanic_bool.csv";
    std::string results_filename = "./results.txt";

    // matrix size
    int m = 15456;
    int n = 159;

    BooleanMatrix::BooleanMatrix M(m, n);
    std::vector<int> groups_idx = {}; // columns feature groups
    std::vector<float> ranks(n);
    //std::generate(ranks.begin(), ranks.end(), std::rand); // generate random ranks
    std::vector<std::pair<int, std::vector<bool>>> csv_data = read_csv(filename);
    fill_matrix(M, csv_data, groups_idx);

    EncodingGreedyAlgorithm A = EncodingGreedyAlgorithm(groups_idx);
    A.fit(M);
    //A.print_solution(M);
    A.analyze();
    A.print_solution(results_filename, n);

    return 0;
}