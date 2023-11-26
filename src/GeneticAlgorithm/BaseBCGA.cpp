#include "BCGA.hpp"

template <typename T>
std::vector<int> BCGA::BaseBCGA::argsort(const std::vector<T> &v) {
    //Argsort for any type of elem-comparable vectors
    std::vector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::stable_sort(idx.begin(), idx.end(),
        [&v](int i1, int i2) {return v[i1] < v[i2];});

    return idx;
}

template <typename T>
std::vector<int> BCGA::BaseBCGA::conditional_argsort(const std::vector<T> &v, const std::vector<T> &c, int &counter_of_cases) {
    //Argsort, but for equal values: bigger c => earlier
    std::vector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::stable_sort(idx.begin(), idx.end(),
        [&v, &c, &counter_of_cases](int i1, int i2) {if((v[i1] == v[i2]) && (c[i1] > c[i2])) counter_of_cases++; return (v[i1] < v[i2]) || ((v[i1] == v[i2]) && (c[i1] > c[i2]));});

    return idx;
}

template <typename T>
std::vector<int> BCGA::BaseBCGA::special_conditional_argsort(const std::vector<T> &v, const std::vector<T> &a, const std::vector<T> &b) {
    //Argsort, but for equal values: bigger c => earlier
    std::vector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::stable_sort(idx.begin(), idx.end(),
        [&v, &a, &b](int i1, int i2) {return (v[i1] < v[i2]) || ((v[i1] == v[i2]) && (b[a[i1]] > b[a[i2]]));});

    return idx;
}

BCGA::BaseBCGA::BaseBCGA(int population_size, int extended_population_size, double mutation_proba, int max_iter, int seed, OutputMode verbose)
{
    this->population_size = population_size;
    this->extended_population_size = extended_population_size;
    this->max_iter = max_iter;
    this->mutation_proba =  mutation_proba;
    this->verbose = verbose;
    if(seed >= 0)
        this->rng = std::mt19937(seed);
    else
        throw std::invalid_argument("Wrong seed value");
}

void BCGA::BaseBCGA::print_stats(std::vector<int>& argbest, int iteration)
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

void BCGA::BaseBCGA::fit(BooleanMatrix::BooleanMatrix& M) {
    std::time_t start = std::time(nullptr);
    this->m = M.get_m();
    this->n = M.get_n();

    check_compatibility();

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
        selection(i);
    }

    std::time_t finish = std::time(nullptr);
    fit_time =  finish - start;

    if(verbose == OutputMode::Max) {
        std::cout << "Learning finished in " << fit_time << " s" << std::endl;
        std::cout << std::endl;
    }
}

BCGA::BinaryIndividual& BCGA::BaseBCGA::get_best_individual()
{
    if ((best_index >= population_size) || (best_index == -1))
        throw std::out_of_range("Solution index (best_index) either wasn't defined or out of range");
    return population[best_index];
}

void BCGA::BaseBCGA::print_individuals()
{
    for(int i = 0; i < population_size; i++)
        std::cout << population[i] << std::endl;
}

void BCGA::BaseBCGA::print_solution(BooleanMatrix::BooleanMatrix& M)
{
    if(n > GlobalSettings::MaxOutputLength)
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

void BCGA::BaseBCGA::analyze_solution(BooleanMatrix::BooleanMatrix& M)
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

void BCGA::BaseBCGA::analyze_alikeness(int t)
{
    for(int i = 0; i < t; i++) {
        int alike_counter = 0;
        int example = -1;

        for(int j = 0; j < population_size; j++) {
            if(i == j)
                continue;

            bool flag = true;
            for(int k = 0; k < n; k++)
                if(population[i].genotype[k] != population[j].genotype[k]) {
                    flag = false;
                    break;
                }

            if(flag)
                example = j;
            alike_counter += flag;
        }

        std::cout << i << "th individual is like " << alike_counter << " others, " << "for example: " << example << std::endl;
    }
}

void BCGA::BaseBCGA::print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename)
{
  std::ofstream f;
  f.open(filename, std::ofstream::app);
  f << fit_time << " " << fitness(M, get_best_individual()) << '\n';
  f.close();
}
