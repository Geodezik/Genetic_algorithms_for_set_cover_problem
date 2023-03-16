#include "Genetic.hpp"

Genetic::Individual::Individual(std::vector<bool>& genotype, bool first_gen, double p)
{
    this->first_gen = first_gen;
    this->genotype = genotype;
}

int Genetic::Individual::size()
{
    return genotype.size();
}

bool Genetic::Individual::is_coverage(BooleanMatrix::BooleanMatrix& M)
{
    int m = M.get_m();
    int n = M.get_n();

    for(int i = 0; i < m; i++) {
        bool flag = false;
        for(int j = 0; j < n; j++) {
            if(!genotype[j])
                continue;
            if(M[i][j]) {
                flag = true;
                break;
            }
        }
        if(!flag)
            return false;
    }

    return true;
}

double Genetic::Individual::fitness(BooleanMatrix::BooleanMatrix& M)
{
    int m = M.get_m();
    int n = M.get_n();

    if(first_gen)
        return 0;

    for(int i = 0; i < m; i++) {
        bool flag = false;
        for(int j = 0; j < n; j++) {
            // gene in set and elem in M is 1
            if(genotype[j] && M[i][j]) {
                flag = true;
                break;
            }
        }
        if(!flag)
            return n + 1;
    }

    // cheap???
    int ones_counter = 0;
    for(int i = 0; i < size(); i++) {
        if(genotype[i])
            ones_counter++;
    }

    return ones_counter;
}

std::ostream& Genetic::operator<<(std::ostream& os, const Individual& I)
{
    for(int i = 0; i < I.genotype.size(); i++) {
        os << I.genotype[i] << " ";
    }
    return os;
};

template <typename T>
std::vector<int> Genetic::GeneticAlgorithm::argsort(const std::vector<T> &v) {
    //Argsort for any type of elem-comparable vectors
    std::vector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::stable_sort(idx.begin(), idx.end(),
        [&v](int i1, int i2) {return v[i1] < v[i2];});

    return idx;
}

Genetic::GeneticAlgorithm::GeneticAlgorithm(int population_size, int extended_population_size, int genotype_len,
                    double mutation_proba, int max_iter, std::string task)
{
    this->population_size = population_size;
    this->extended_population_size = extended_population_size;
    this->max_iter = max_iter;
    this->mutation_proba =  mutation_proba;

    create_zero_generation(genotype_len);
}

void Genetic::GeneticAlgorithm::create_zero_generation(int genotype_len)
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

Genetic::Individual Genetic::GeneticAlgorithm::crossover(Individual& parent1, Individual& parent2)
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

void Genetic::GeneticAlgorithm::mutate(std::vector<Genetic::Individual>& individual_vector, double mutation_proba, int parameter)
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

void Genetic::GeneticAlgorithm::fit(BooleanMatrix::BooleanMatrix& M, int verbose, bool finishing_message) {
    std::time_t start = std::time(nullptr);
    int m = M.get_m();
    int n = M.get_n();

    //CROSSOVER (creating extended population)
    int delta = extended_population_size - population_size;
    int genotype_len = population[0].genotype.size();
    std::uniform_int_distribution<> parents_d(0, population_size - 1);

    for(int i = 0; i < max_iter; i++) {
        //std::cout << population[0].fitness(M) << std::endl;
        std::vector<Individual> extended_population = population;
        //std::cout << extended_population[0].fitness(M) << std::endl;
        for(int j = 0; j < delta; j++) {
            int p1, p2;
            do {
                p1 = parents_d(rng);
                p2 = parents_d(rng);
            } while (p1 == p2);
            Individual child = crossover(population[p1], population[p2]);
            extended_population.push_back(child);
        }

        //Mutate
        int mutations = (n / 5) * (max_iter - i) / max_iter;
        mutate(extended_population, 1.0, mutations);

        //Get scores
        std::vector<double> scores;
        for(int j = 0; j < extended_population_size; j++) {
            scores.push_back(extended_population[j].fitness(M));
            //std::cout << scores[j] << " ";
        }

        //Take K best
        std::vector<int> argbest = argsort(scores);

        switch(verbose) {
            case 0:
                break;
            case 1:
                std::cout << scores[argbest[0]] << std::endl;
                break;
            case 2:
                std::cout << "Generation: " << i << std::endl;
                std::cout << "Best individual fitness: " << scores[argbest[0]] << std::endl;
                std::cout << std::endl;
                break;
        }

        std::vector<Individual> best;

        // first include non-firstgen that are coverings
        for(int j = 0; j < extended_population_size; j++) {
            if(best.size() == population_size)
                break;
            if(extended_population[argbest[j]].first_gen || (scores[j] > n))
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

    std::time_t finish = std::time(nullptr);
    fit_time =  finish - start;

    if(finishing_message) {
        std::cout << "Learning finished in " << fit_time << " s" << std::endl;
        std::cout << std::endl;
    }
}

Genetic::Individual& Genetic::GeneticAlgorithm::get_best_individual()
{
    return population[0];
}

void Genetic::GeneticAlgorithm::print_individuals()
{
    for(int i =0; i < population_size; i++) {
        std::cout << population[i] << std::endl;
    }
}

void Genetic::GeneticAlgorithm::print_solution(BooleanMatrix::BooleanMatrix& M)
{
    int m = M.get_m();
    int n = M.get_n();
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

void Genetic::GeneticAlgorithm::analyze_solution(BooleanMatrix::BooleanMatrix& M)
{
    int m = M.get_m();
    int n = M.get_n();

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

void Genetic::GeneticAlgorithm::print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename)
{
  std::ofstream f;
  f.open(filename, std::ofstream::app);
  f << fit_time << " " << population[0].fitness(M) << '\n';
  f.close();
}
