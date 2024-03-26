#include "BCGA.hpp"

BCGA::BaseBCGA::BaseBCGA(int population_size, int extended_population_size, double mutation_proba, int max_iter, int seed, OutputMode verbose)
{
    this->population_size = population_size;
    this->extended_population_size = extended_population_size;
    this->max_iter = max_iter;
    this->mutation_proba =  mutation_proba;
    this->verbose = verbose;
    this->scores = std::vector<double>(extended_population_size);

    if(seed >= 0)
        this->rng = std::mt19937(seed);
    else
        throw std::invalid_argument("Wrong seed value");
}

void BCGA::BaseBCGA::print_stats(std::vector<int>& argbest)
{
    switch(verbose) {
        case OutputMode::Silent:
            break;
        case OutputMode::Normal:
            std::cout << scores[argbest[0]] << std::endl;
            break;
        case OutputMode::Max:
            std::cout << "Generation: " << iteration + 1 << std::endl;
            std::cout << "Best individual fitness: " << scores[argbest[0]] << std::endl;
            std::cout << std::endl;
            break;
    }
}

void BCGA::BaseBCGA::update_scores(BooleanMatrix::BooleanMatrix& M)
{
    for(int j = population_size; j < extended_population_size; j++)
        scores[j] = fitness(M, population[j]);
}

void BCGA::BaseBCGA::fit(BooleanMatrix::BooleanMatrix& M) {
    if(is_fitted)
        throw std::runtime_error("IsAlreadyFittedError");

    std::time_t start = std::time(nullptr);
    this->m = M.get_m();
    this->n = M.get_n();

    check_compatibility();
    population.clear();

    create_zero_generation(M, n);

    scores.clear();
    for(int j = 0; j < population_size; j++) {
        double f = fitness(M, population[j]);
        if(f < best_score) {
            best_score = f;
            best_index = j;
        }
        scores[j] = f;
    }

    //CROSSOVER (creating extended population)
    int delta = extended_population_size - population_size;
    int genotype_len = population[0].genotype.size();

    for(int i = 0; i < max_iter; i++) {
        iteration = i;

        // choose parents
        for(int j = 0; j < delta; j++) {
            int p1 = 0, p2 = 0;
            get_parent_indices(p1, p2);
            auto child = crossover(population[p1], population[p2]);
            population.push_back(child);
        }

        //Mutate
        mutate(M, mutation_proba);

        //Get scores for new individuals
        update_scores(M);

        //Selection
        selection();
    }

    std::time_t finish = std::time(nullptr);
    fit_time =  finish - start;

    if(verbose == OutputMode::Max) {
        std::cout << "Learning finished in " << fit_time << " s" << std::endl;
        std::cout << std::endl;
    }

    is_fitted = true;
}

BCGA::BinaryIndividual& BCGA::BaseBCGA::get_best_individual()
{
    if(!is_fitted)
        throw std::runtime_error("NotFittedError");
    if ((best_index >= population_size) || (best_index == -1))
        throw std::out_of_range("Solution index (best_index) either wasn't defined or out of range");
    return population[best_index];
}

void BCGA::BaseBCGA::print_solution(std::string filename)
{
    if(!is_fitted)
        throw std::runtime_error("NotFittedError");
    auto columns = get_best_individual().genotype;
    std::ofstream f;
    f.open(filename);
    for(int i = 0; i < n; i++)
        if(columns[i])
            f << i << ' ';
    f.close();
}
