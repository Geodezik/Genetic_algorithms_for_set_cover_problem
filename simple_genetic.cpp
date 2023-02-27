#include "Genetic.hpp"
/*
#include <vector>
#include <random>
#include <numeric>
#include <algorithm>
*/
using namespace Genetic;

int main()
{
    
    //Create data
    int m = 6;
    int n = 6;

    bool Matrix[6][6] = {
        {0, 1, 1, 1, 1, 0},
        {1, 0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0, 1},
        {0, 1, 1, 1, 1, 0},
        {0, 0, 1, 0, 1, 0},
        {0, 0, 1, 1, 1, 0}
    };
    BooleanMatrix M(m, n);
    // std::cout << M[0][2];

    //int population_size, int extended_population_size, int chromosome_len, double mutation_proba, int max_iter = 100
    GeneticAlgorithm A = GeneticAlgorithm(3, 6, n, 0.2, 3);

    double p = 0.5;
    std::random_device rd{};
    std::mt19937 rng{rd()};
    std::bernoulli_distribution d(p);

    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            //M[i][j] = static_cast<bool> (d(rng));
            M[i][j] = Matrix[i][j];
        }
    }


    A.fit(M);
    A.print_solution(M);

    /*
    for (int i = 0; i < m; i++)
        delete M.M[i];
    delete M.M;
    */

    return 0;
}