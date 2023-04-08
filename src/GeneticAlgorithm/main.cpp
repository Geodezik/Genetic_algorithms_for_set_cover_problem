#include "Genetic.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>
#include <random>
#include <numeric>

using namespace Genetic;

int main()
{
    int m = 2500;
    int n = 10000;
    double p = 0.0005;

    std::random_device rd{};
    std::mt19937 rng{rd()};
    rng.seed(317);
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

    //int population_size, int extended_population_size, int chromosome_len, double mutation_proba, int max_iter = 100, str task
    SotnezovGeneticAlgorithm A = SotnezovGeneticAlgorithm(100, 101, 1.0, 1500);
    // A.print_individuals();
    A.fit(M, 2, false);
    //A.print_solution(M);
    //A.print_fit_stats(M);

    return 0;
}