#include "BCGA.hpp"
#include "Greedy.hpp"
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
    int m = std::atoi(argv[2]);
    int n = std::atoi(argv[3]);

    // Sotnezov's mutation parameters
    int K = n / 4;
    double C = 0.001;
    double alpha = 0.5;

    // GA parameters
    int population_size = 50;
    int max_iter = 1000;
    int norank_iter = max_iter / 2;
    int seed = std::atoi(argv[1]);;

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
    RankType: ElementWise or GroupWise. Group or column-wise rank mean. Sum is deprecated.
    */

    RankType rank_function;
    if(std::string(argv[4]) == "elementwise")
        rank_function = RankType::ElementWise;
    else if(std::string(argv[4]) == "groupwise")
        rank_function = RankType::GroupWise;
    else if(std::string(argv[4]) == "sum")
        rank_function = RankType::Sum;
    else
        throw std::invalid_argument("Wrong rank function");

    Fitness fitness_function;
    if(std::string(argv[5]) == "covlen")
        fitness_function = Fitness::CovLen;
    else if(std::string(argv[5]) == "maxbinsnum")
        fitness_function = Fitness::MaxBinsNum;
    else if(std::string(argv[5]) == "mixed")
        fitness_function = Fitness::Mixed;
    else
        throw std::invalid_argument("Wrong fitness function");

    //SotnezovBCGA A = SotnezovBCGA(population_size, K, C, max_iter, seed, OutputMode::Max);
    //EncSotnezovBCGA A = EncSotnezovBCGA(population_size, groups_idx, Fitness::MaxBinsNum, K, C, max_iter, seed, OutputMode::Max);
    if(std::string(argv[6]) == "gencode+") {
        REncSotnezovBCGA A = REncSotnezovBCGA(population_size, groups_idx, ranks, rank_function, fitness_function, K, C, alpha, max_iter, norank_iter, seed, OutputMode::Max);
        A.fit(M);
        A.print_solution(results_filename);
    } else if(std::string(argv[6]) == "gencode") {
        EncSotnezovBCGA A = EncSotnezovBCGA(population_size, groups_idx, Fitness::MaxBinsNum, K, C, max_iter, seed, OutputMode::Max);
        A.fit(M);
        A.print_solution(results_filename);
    } else if(std::string(argv[6]) == "greedy") {
        Greedy::EncodingGreedyAlgorithm A = Greedy::EncodingGreedyAlgorithm(groups_idx);
        A.fit(M);
        A.print_solution(results_filename, n);
        A.analyze();
    } else
        throw std::invalid_argument("Wrong algorithm");
    return 0;
}