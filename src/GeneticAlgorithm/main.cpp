#include "BCGA.hpp"
#include "BooleanMatrix.hpp"
#include "misc.cpp"
#include <random>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>

using namespace BCGA;

int main()
{
    std::string filename = "../../data/titanic_bool.csv";
    std::string results_filename = "./results.txt";

    // matrix size
    int m = 15456;
    int n = 159;

    // Sotnezov's mutation parameters
    int K = n / 2;
    float C = 0.01;
    float alpha = 0.2;

    // GA parameters
    int population_size = 25;
    int max_iter = 250;
    int norank_iter = max_iter / 2;
    int seed = 42;

    BooleanMatrix::BooleanMatrix M(m, n);
    std::vector<int> groups_idx = {}; // columns feature groups
    std::vector<float> ranks(n);
    std::generate(ranks.begin(), ranks.end(), std::rand); // generate random ranks
    std::vector<std::pair<int, std::vector<bool>>> csv_data = read_csv(filename);
    fill_matrix(M, csv_data, groups_idx);

    /*
    ALGORITHMS COMMENTS

    SotnezovBCGA: finds (approx.) minimum coverage of a bool matrix 
    EncSotnezovBCGA: finds (approx.) minimum encoding coverage of a bool matrix
    Fitness: MaxBinsNum (znachnost') or CovLen (coverage length) or Mixed (weighted sum of two prev)
    REncSotnezovBCGA: finds (approx.) minimum encoding coverage of a bool matrix, but checks columns ranks in selection step
                      this problem can be rewritten with two optimization criterias
    RankType: ElementWise or GroupWise. Group or column-wise rank mean.
    */

    //SotnezovBCGA A = SotnezovBCGA(population_size, K, C, max_iter, seed, OutputMode::Max);
    //EncSotnezovBCGA A = EncSotnezovBCGA(population_size, groups_idx, Fitness::MaxBinsNum, K, C, max_iter, seed, OutputMode::Max);
    REncSotnezovBCGA A = REncSotnezovBCGA(population_size, groups_idx, ranks, RankType::ElementWise, Fitness::MaxBinsNum, K, C, alpha, max_iter, norank_iter, seed, OutputMode::Max);

    A.fit(M);
    A.print_solution(results_filename);

    return 0;
}