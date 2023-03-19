#include "Genetic.hpp"

Genetic::SotnezovGeneticAlgorithm::SotnezovGeneticAlgorithm(int population_size, int extended_population_size, double mutation_proba,
                                                            int max_iter): Genetic::BaseGeneticAlgorithm(population_size, extended_population_size,
                                                                                                         mutation_proba, max_iter) {}

void Genetic::SotnezovGeneticAlgorithm::create_zero_generation(int genotype_len)
{

}

Genetic::Individual Genetic::SotnezovGeneticAlgorithm::crossover(Individual& parent1, Individual& parent2)
{
    std::vector<bool> new_genotype;
    return Individual(new_genotype);
}

void Genetic::SotnezovGeneticAlgorithm::mutate(double mutation_proba, int parameter)
{

}

double Genetic::SotnezovGeneticAlgorithm::fitness(Genetic::Individual& individual, BooleanMatrix::BooleanMatrix& M)
{
    return 0.0;
}

void Genetic::SotnezovGeneticAlgorithm::selection(int iteration, int verbose)
{

}
