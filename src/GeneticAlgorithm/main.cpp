#include "Genetic.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>
#include <random>
#include <numeric>

using namespace Genetic;

int main()
{
    int m = 2500;
    int n = 7500;

    double p = 0.001;
    std::random_device rd{};
    std::mt19937 rng{rd()};
    rng.seed(317);
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
            //M[i][j] = false;
            if (i == j)
                M[i][j] = true;
        }
    }

    //int population_size, int extended_population_size, int chromosome_len, double mutation_proba, int max_iter = 100, str task
    GeneticAlgorithm A = GeneticAlgorithm(25, 100, n, 1.0, 5);
    // A.print_individuals();
    A.fit(M);
    A.analyze_solution(M);

    return 0;
}