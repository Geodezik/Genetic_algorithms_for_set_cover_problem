#include "BCGA.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>
#include <random>
#include <numeric>

using namespace BCGA;

int main()
{
    int m = 1000;
    int n = 1500;
    double p = 0.0025;
    int seed = 417;

    int K = 200;
    float C = 0.001;
    int population_size = 5;
    int max_iter = 1000;

    std::random_device rd{};
    std::mt19937 rng{rd()};
    rng.seed(seed);
    std::bernoulli_distribution bd(p);
    std::uniform_int_distribution<> uid(1, n - 1);

    /*
    bool Matrix[m][n] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {1, 1, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 1},
        {1, 1, 0, 0, 0, 0, 1, 0, 0, 0}
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

    std::vector<int> groups_idx = {0, 22, 74, 122, 198, 244, 399, 524, 666, 680, 722, 740, 900, 1099, 1311};

    //SotnezovBCGA A = SotnezovBCGA(population_size, K, C, max_iter, seed, OutputMode::Normal);
    EncSotnezovBCGA A = EncSotnezovBCGA(population_size, groups_idx, Fitness::MaxBinsNum, K, C, max_iter, seed, OutputMode::Normal);
    A.fit(M);
    A.analyze_solution(M);

    return 0;
}