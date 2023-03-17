#include "Genetic.hpp"

template <typename T>
std::vector<int> Genetic::BaseGeneticAlgorithm::argsort(const std::vector<T> &v) {
    //Argsort for any type of elem-comparable vectors
    std::vector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::stable_sort(idx.begin(), idx.end(),
        [&v](int i1, int i2) {return v[i1] < v[i2];});

    return idx;
}

Genetic::BaseGeneticAlgorithm::BaseGeneticAlgorithm(int population_size, int extended_population_size,
                    double mutation_proba, int max_iter, std::string task)
{
    this->population_size = population_size;
    this->extended_population_size = extended_population_size;
    this->max_iter = max_iter;
    this->mutation_proba =  mutation_proba;
}

void Genetic::BaseGeneticAlgorithm::print_stats(std::vector<double>& scores, std::vector<int>& argbest, int iteration, int verbose)
{
    switch(verbose) {
        case 0:
            break;
        case 1:
            std::cout << scores[argbest[0]] << std::endl;
            break;
        case 2:
            std::cout << "Generation: " << iteration << std::endl;
            std::cout << "Best individual fitness: " << scores[argbest[0]] << std::endl;
            std::cout << std::endl;
            break;
    }
}

void Genetic::BaseGeneticAlgorithm::fit(BooleanMatrix::BooleanMatrix& M, int verbose, bool finishing_message) {
    std::time_t start = std::time(nullptr);
    this->m = M.get_m();
    this->n = M.get_n();

    create_zero_generation(n);

    //CROSSOVER (creating extended population)
    int delta = extended_population_size - population_size;
    int genotype_len = population[0].genotype.size();
    std::uniform_int_distribution<> parents_d(0, population_size - 1);

    for(int i = 0; i < max_iter; i++) {
        std::vector<BaseIndividual> extended_population = population;
        for(int j = 0; j < delta; j++) {
            int p1, p2;
            do {
                p1 = parents_d(rng);
                p2 = parents_d(rng);
            } while (p1 == p2);
            auto child = crossover(population[p1], population[p2]);
            extended_population.push_back(child);
        }

        //Mutate
        int mutations = (n / 5) * (max_iter - i) / max_iter;
        mutate(extended_population, 1.0, mutations);

        //Get scores
        std::vector<double> scores;
        for(int j = 0; j < extended_population_size; j++) {
            scores.push_back(extended_population[j].fitness(M));
        }

        //Selection
        selection(extended_population, scores, i, verbose);
    }

    std::time_t finish = std::time(nullptr);
    fit_time =  finish - start;

    if(finishing_message) {
        std::cout << "Learning finished in " << fit_time << " s" << std::endl;
        std::cout << std::endl;
    }
}

Genetic::BaseIndividual& Genetic::BaseGeneticAlgorithm::get_best_individual()
{
    return population[0];
}

void Genetic::BaseGeneticAlgorithm::print_individuals()
{
    for(int i =0; i < population_size; i++) {
        std::cout << population[i] << std::endl;
    }
}

void Genetic::BaseGeneticAlgorithm::print_solution(BooleanMatrix::BooleanMatrix& M)
{
    if(n > 100) {
        std::cout << "WARNING: Solution output can be too huge." << std::endl;
    }

    std::cout << "Best individual: " << get_best_individual() << std::endl;
    std::cout << "Coverage: " << std::endl;
    std::vector<bool> best = get_best_individual().genotype;
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            if(!best[j])
                continue;
            std::cout << M[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void Genetic::BaseGeneticAlgorithm::analyze_solution(BooleanMatrix::BooleanMatrix& M)
{
    std::vector<int> scores;
    int genotype_len = population[0].size();

    std::cout << "Analyzing..." << std::endl;
    for(int j = 0; j < population_size; j++) {
        int fitness = population[j].fitness(M);
        std::cout << j + 1 << ") ";
        std::cout << "Fitness: " << fitness << ',';
        if(fitness < n + 1) {
            std::cout << " columns not included: " << n - fitness << std::endl;
        } else {
            std::cout << " not a covering" << std::endl;
        }
    }
}

void Genetic::BaseGeneticAlgorithm::print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename)
{
  std::ofstream f;
  f.open(filename, std::ofstream::app);
  f << fit_time << " " << population[0].fitness(M) << '\n';
  f.close();
}
