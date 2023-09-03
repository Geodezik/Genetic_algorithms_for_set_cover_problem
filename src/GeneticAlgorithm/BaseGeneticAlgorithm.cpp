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

Genetic::BaseGeneticAlgorithm::BaseGeneticAlgorithm(int population_size, int extended_population_size, double mutation_proba, int max_iter)
{
    this->population_size = population_size;
    this->extended_population_size = extended_population_size;
    this->max_iter = max_iter;
    this->mutation_proba =  mutation_proba;
}

void Genetic::BaseGeneticAlgorithm::print_stats(std::vector<int>& argbest, int iteration, int verbose)
{
    switch(verbose) {
        case 0:
            break;
        case 1:
            std::cout << scores[argbest[0]] << std::endl;
            break;
        case 2:
            std::cout << "Generation: " << iteration + 1 << std::endl;
            std::cout << "Best individual fitness: " << scores[argbest[0]] << std::endl;
            std::cout << std::endl;
            break;
    }
}

void Genetic::BaseGeneticAlgorithm::fit(BooleanMatrix::BooleanMatrix& M, int verbose, bool finishing_message) {
    std::time_t start = std::time(nullptr);
    this->m = M.get_m();
    this->n = M.get_n();

    population.clear();
    create_zero_generation(M, n);

    scores.clear();
    for(int j = 0; j < population_size; j++)
        scores.push_back(fitness(M, population[j]));

    //CROSSOVER (creating extended population)
    int delta = extended_population_size - population_size;
    int genotype_len = population[0].genotype.size();

    // choose parents
    for(int i = 0; i < max_iter; i++) {
        for(int j = 0; j < delta; j++) {
            int p1 = 0, p2 = 0;
            get_parent_indices(p1, p2);
            auto child = crossover(population[p1], population[p2]);
            population.push_back(child);
        }

        //Mutate
        mutate(M, mutation_proba, i);

        //Get scores for new individuals
        for(int j = population_size; j < extended_population_size; j++)
            scores[j] = fitness(M, population[j]);

        //Selection
        selection(i, verbose);
    }

    std::time_t finish = std::time(nullptr);
    fit_time =  finish - start;

    if(finishing_message) {
        std::cout << "Learning finished in " << fit_time << " s" << std::endl;
        std::cout << std::endl;
    }
}

Genetic::Individual& Genetic::BaseGeneticAlgorithm::get_best_individual()
{
    if ((best_index >= population_size) || (best_index == -1))
        throw std::out_of_range("Solution index (best_index) either wasn't defined or out of range");
    return population[best_index];
}

void Genetic::BaseGeneticAlgorithm::print_individuals()
{
    for(int i = 0; i < population_size; i++)
        std::cout << population[i] << std::endl;
}

void Genetic::BaseGeneticAlgorithm::print_solution(BooleanMatrix::BooleanMatrix& M)
{
    if(n > 100)
        std::cout << "WARNING: Solution output can be too huge." << std::endl;

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
    for(int j = 0; j < population.size(); j++) {
        int f = fitness(M, population[j]);
        std::cout << j + 1 << ") ";
        std::cout << "Fitness: " << f << ',';
        if(f < n + 1)
            std::cout << " columns not included: " << n - f << std::endl;
        else
            std::cout << " not a covering" << std::endl;
    }
}

void Genetic::BaseGeneticAlgorithm::analyze_alikeness(int t)
{
    for(int i = 0; i < t; i++) {
        int alike_counter = 0;
        int example = -1;

        for(int j = 0; j < population_size; j++) {
            if(i == j)
                continue;

            bool flag = true;
            for(int k = 0; k < n; k++) {
                if(population[i].genotype[k] != population[j].genotype[k]) {
                    flag = false;
                    break;
                }
            }
            if(flag)
                example = j;
            alike_counter += flag;
        }

        std::cout << i << "th individual is like " << alike_counter << " others, " << "for example: " << example << std::endl;
    }
}

void Genetic::BaseGeneticAlgorithm::print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename)
{
  std::ofstream f;
  f.open(filename, std::ofstream::app);
  f << fit_time << " " << fitness(M, population[0]) << '\n';
  f.close();
}
