#include "Genetic.hpp"

Genetic::CoverageGeneticAlgorithm::CoverageGeneticAlgorithm(int population_size, int extended_population_size,
                double mutation_proba, int max_iter, std::string task): Genetic::BaseGeneticAlgorithm(population_size, extended_population_size,
                                                                        mutation_proba, max_iter, task) {}

void Genetic::CoverageGeneticAlgorithm::create_zero_generation(int genotype_len)
{
    //creating zero generation with some genes
    for(int i = 0; i < population_size; i++) {
        std::vector<bool> genotype;
        for(int j = 0; j < genotype_len; j++) {
            genotype.push_back(true);
        }
        population.push_back(Individual(genotype, true));
    };
}

Genetic::Individual Genetic::CoverageGeneticAlgorithm::crossover(Individual& parent1, Individual& parent2)
{
    // pick a random point (but not at the end of a chromosomes), then
    // join two parts from different parents
    std::vector<bool> new_genotype;
    int length = parent1.genotype.size();
    std::uniform_int_distribution<> d(1, length - 1);
    int point = d(rng);

    for(int i = 0; i < point; i++) {
        new_genotype.push_back(parent1.genotype[i]);
    }

    for(int i = point; i < length; i++) {
        new_genotype.push_back(parent2.genotype[i]);
    }

    return Individual(new_genotype);
}

void Genetic::CoverageGeneticAlgorithm::mutate(std::vector<Genetic::Individual>& individual_vector, double mutation_proba, int parameter)
{
    std::bernoulli_distribution bernoulli_d(mutation_proba);
    int genotype_len = individual_vector[0].size();
    for(int mut_iter = 0; mut_iter < parameter; mut_iter++) {
        std::uniform_int_distribution<> genes_d(0, genotype_len - 1);
        for(int j = population_size + 1; j < extended_population_size; j++) {
            bool mutate = bernoulli_d(rng);
            if(!mutate)
                continue;

            int random_gen = genes_d(rng);
            individual_vector[j].genotype[random_gen] = !individual_vector[j].genotype[random_gen];
        }
    }
}

void Genetic::CoverageGeneticAlgorithm::selection(std::vector<Genetic::Individual>& extended_population, std::vector<double>& scores, int iteration, int verbose)
{
    //Take K best
    std::vector<int> argbest = argsort(scores);
    print_stats(scores, argbest, iteration, verbose);

    std::vector<Individual> best;

    // first include non-firstgen that are coverings
    for(int j = 0; j < extended_population_size; j++) {
        if(best.size() == population_size)
            break;
        if(extended_population[argbest[j]].is_from_zero_gen() || (scores[j] > n))
            continue;
        best.push_back(extended_population[argbest[j]]);
    }

    // second include firstgen
    for(int j = 0; j < extended_population_size; j++) {
        if(best.size() == population_size)
            break;
        if(scores[j] > n)
            continue;
        best.push_back(extended_population[argbest[j]]);
    }

    // third include non-coverings
    for(int j = 0; j < extended_population_size; j++) {
        if(best.size() == population_size)
            break;
        best.push_back(extended_population[argbest[j]]);
    }

    population = best;
}
