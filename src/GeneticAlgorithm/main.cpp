#include "BCGA.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>
#include <random>
#include <numeric>

using namespace BCGA;

int main()
{
    int m = 750;
    int n = 1200;
    double p = 0.0025;
    int seed = 417;

    int K = 250;
    float C = 0.001;
    int population_size = 20;
    int max_iter = 500;

    std::random_device rd{};
    std::mt19937 rng{rd()};
    rng.seed(seed);
    std::bernoulli_distribution bd(p);
    std::uniform_int_distribution<> uid(1, n - 1);

    /*
    bool Matrix[m][n] = {
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0}
    };
    */

    BooleanMatrix::BooleanMatrix M(m, n);
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            M[i][j] = static_cast<bool> (bd(rng));
            //M[i][j] = Matrix[i][j];
        }
        M[i][uid(rng)] = true;
    }

    std::vector<int> groups_idx = {0, 109, 411, 512, 666, 899, 909, 1113, 1177};
    std::vector<float> ranks(n);
    ranks[4] = 10000;
    ranks[32] = 2;
    ranks[488] = 2;
    ranks[1002] = 2;
    ranks[441] = 6;
    ranks[41] = 4;
    ranks[22] = 1;
    ranks[910] = 2;
    ranks[914] = 10;
    ranks[331] = 1;
    ranks[516] = 2;

    //SotnezovBCGA A = SotnezovBCGA(population_size, K, C, max_iter, seed, OutputMode::Normal);
    REncSotnezovBCGA A = REncSotnezovBCGA(population_size, groups_idx, ranks, Fitness::MaxBinsNum, K, C, max_iter, seed, OutputMode::Max);
    A.fit(M);
    A.analyze_solution(M);
    //A.print_solution(M);

    return 0;
}