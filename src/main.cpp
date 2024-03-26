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
    double alpha = std::atoi(argv[6]);

    // GA parameters
    int population_size = 25;
    int max_iter = std::atoi(argv[4]);
    int norank_iter = max_iter / std::atoi(argv[5]);
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
    if(std::string(argv[7]) == "elementwise")
        rank_function = RankType::ElementWise;
    else if(std::string(argv[7]) == "groupwise")
        rank_function = RankType::GroupWise;
    else if(std::string(argv[7]) == "sum")
        rank_function = RankType::Sum;
    else
        throw std::invalid_argument("Wrong rank function");

    Fitness fitness_function;
    if(std::string(argv[8]) == "covlen")
        fitness_function = Fitness::CovLen;
    else if(std::string(argv[8]) == "maxbinsnum")
        fitness_function = Fitness::MaxBinsNum;
    else if(std::string(argv[8]) == "mixed")
        fitness_function = Fitness::Mixed;
    else
        throw std::invalid_argument("Wrong fitness function");

    if(std::string(argv[9]) == "gencode+") {
        GENCODE_plus A = GENCODE_plus(population_size, groups_idx, ranks, rank_function, fitness_function, K, C, alpha, max_iter, norank_iter, seed, OutputMode::Max);
        A.fit(M);
        A.print_solution(results_filename);
    } else if(std::string(argv[9]) == "gencode") {
        GENCODE A = GENCODE(population_size, groups_idx, Fitness::MaxBinsNum, K, C, max_iter, seed, OutputMode::Max);
        A.fit(M);
        A.print_solution(results_filename);
    } else if(std::string(argv[9]) == "code3") {
        CODE3 A = CODE3(population_size, groups_idx, Fitness::MaxBinsNum, K, C, max_iter, seed, OutputMode::Max);
        A.fit(M);
        A.print_solution(results_filename);
    } else if(std::string(argv[9]) == "greedy") {
        Greedy::EncodingGreedyAlgorithm A = Greedy::EncodingGreedyAlgorithm(groups_idx);
        A.fit(M);
        A.print_solution(results_filename, n);
        A.analyze();
    } else
        throw std::invalid_argument("Wrong algorithm");
    return 0;
}