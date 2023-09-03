#include "BCGA.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>
#include <random>
#include <numeric>

using namespace BCGA;

int main()
{
    int m = 1000;
    int n = 1000;
    double p = 0.05;
    int seed = 317;

    int K = 100;
    float C = 0.01;
    int population_size = 25;
    int max_iter = 250;

    std::random_device rd{};
    std::mt19937 rng{rd()};
    rng.seed(seed);
    std::bernoulli_distribution bd(p);
    std::uniform_int_distribution<> uid(1, n - 1);

    /*
    bool Matrix[m][n] = {
        {0, 1, 1, 1, 1, 0, 1, 0, 1, 0},
        {1, 0, 1, 0, 0, 0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0, 1, 1, 0, 1, 0},
        {0, 1, 1, 1, 1, 0, 0, 0, 1, 0},
        {0, 0, 1, 0, 1, 0, 1, 0, 1, 0},
        {0, 0, 1, 1, 1, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0}
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

    SotnezovBCGA A = SotnezovBCGA(population_size, K, C, max_iter, seed, OutputMode::Normal);
    // A.print_individuals();
    A.fit(M);
    //A.print_solution(M);
    //A.print_fit_stats(M);

    return 0;
}