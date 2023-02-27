#include "Genetic.hpp"
#include <iostream>
#include <random>
#include <numeric>

using namespace Genetic;

int main()
{
    int m = 10;
    int n = 10;

    double p = 0.5;
    std::random_device rd{};
    std::mt19937 rng{rd()};
    std::bernoulli_distribution d(p);

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

    BooleanMatrix M(m, n);
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            //M[i][j] = static_cast<bool> (d(rng));
            M[i][j] = Matrix[i][j];
        }
    }

    //int population_size, int extended_population_size, int chromosome_len, double mutation_proba, int max_iter = 100, str task
    GeneticAlgorithm A = GeneticAlgorithm(5, 15, n, 0.2, 25);
    A.fit(M);
    A.print_solution(M);

    return 0;
}