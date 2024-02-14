#include "BCGA.hpp"
#include "BooleanMatrix.hpp"
#include "misc.cpp"
#include <random>
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>

using namespace BCGA;

int main(int argc, char* argv[])
{
    std::string filename = "./data/bool.csv";
    std::string ranks_filename = "./data/ranks.csv";
    std::string results_filename = "./data/results.txt";

    // matrix size
    int m = std::atoi(argv[1]);
    int n = std::atoi(argv[2]);

    // Sotnezov's mutation parameters
    int K = n / 5;
    double C = 0.001;
    double alpha = 0.2;

    // GA parameters
    int population_size = 25;
    int max_iter = 1000;
    int norank_iter = max_iter / 2;
    int seed = 42;

    BooleanMatrix::BooleanMatrix M(m, n);
    std::vector<int> groups_idx = {}; // columns feature groups
    std::vector<double> ranks;
    //std::generate(ranks.begin(), ranks.end(), std::rand); // generate random ranks
    read_ranks(ranks_filename, ranks);
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

    RankType rank_function;
    if(std::string(argv[3]) == "elementwise")
        rank_function = RankType::ElementWise;
    else if(std::string(argv[3]) == "groupwise")
        rank_function = RankType::GroupWise;
    else if(std::string(argv[3]) == "sum")
        rank_function = RankType::Sum;
    else
        throw std::invalid_argument("Wrong rank function");

    Fitness fitness_function;
    if(std::string(argv[4]) == "covlen")
        fitness_function = Fitness::CovLen;
    else if(std::string(argv[4]) == "maxbinsnum")
        fitness_function = Fitness::MaxBinsNum;
    else if(std::string(argv[4]) == "mixed")
        fitness_function = Fitness::Mixed;
    else
        throw std::invalid_argument("Wrong fitness function");

    //SotnezovBCGA A = SotnezovBCGA(population_size, K, C, max_iter, seed, OutputMode::Max);
    //EncSotnezovBCGA A = EncSotnezovBCGA(population_size, groups_idx, Fitness::MaxBinsNum, K, C, max_iter, seed, OutputMode::Max);
    REncSotnezovBCGA A = REncSotnezovBCGA(population_size, groups_idx, ranks, rank_function, fitness_function, K, C, alpha, max_iter, norank_iter, seed, OutputMode::Silent);

    A.fit(M);
    A.print_solution(results_filename);

    return 0;
}