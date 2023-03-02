#include "Genetic.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>
#include <random>
#include <numeric>

using namespace Genetic;

int main()
{
    int m = 500;
    int n = 500;

    double p = 0.15;
    std::random_device rd{};
    std::mt19937 rng{rd()};
    std::bernoulli_distribution d(p);

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
            M[i][j] = static_cast<bool> (d(rng));
            //M[i][j] = Matrix[i][j];
        }BooleanMatrix.o
    }

    //int population_size, int extended_population_size, int chromosome_len, double mutation_proba, int max_iter = 100, str task
    GeneticAlgorithm A = GeneticAlgorithm(50, 500, n, 0.2, 100);
    A.fit(M);
    //A.print_solution(M);

    return 0;
}