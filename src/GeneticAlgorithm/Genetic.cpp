#include "Genetic.hpp"

Genetic::Individual::Individual(int chromosome_size, bool first_gen)
{
    this->first_gen = first_gen;
    double p = 0.5;

    std::random_device rd{};
    std::mt19937 rng{rd()};
    std::bernoulli_distribution d(p);

    //creating chromosome from random bits
    chromosome = std::vector<bool>();
    for(int i=0; i < chromosome_size; i++) {
        chromosome.push_back(d(rng));
    }
}

Genetic::Individual::Individual(std::vector<bool> chromosome, bool first_gen)
{
    this->first_gen = first_gen;
    this->chromosome = chromosome;
}

int Genetic::Individual::size()
{
    return chromosome.size();
}

bool Genetic::Individual::is_coverage(BooleanMatrix::BooleanMatrix& M)
{
    int m = M.get_m();
    int n = M.get_n();

    for(int i = 0; i < m; i++) {
        bool flag = false;
        for(int j = 0; j < n; j++) {
            if(!chromosome[j])
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
            if(chromosome[j] && M[i][j]) {
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
        if(chromosome[i])
            ones_counter++;
    }

    return ones_counter;
}

std::ostream& Genetic::operator<<(std::ostream& os, const Individual& I)
{
    for(int i = 0; i < I.chromosome.size(); i++) {
        os << I.chromosome[i] << " ";
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

Genetic::GeneticAlgorithm::GeneticAlgorithm(int population_size, int extended_population_size, int chromosome_len,
                    double mutation_proba, int max_iter, std::string task)
{
    this->population_size = population_size;
    this->extended_population_size = extended_population_size;
    this->max_iter = max_iter;
    this->mutation_proba =  mutation_proba;

    double p = 1.0;

    std::random_device rd{};
    std::mt19937 random_engine{rd()};
    rng = random_engine;
    std::bernoulli_distribution d(p);

    //creating zero generation with random genes
    for(int i = 0; i < population_size; i++) {
        std::vector<bool> new_chromosome;
        for(int j = 0; j < chromosome_len; j++) {
            new_chromosome.push_back(d(rng));
        }
        population.push_back(Individual(new_chromosome));
    };
}

Genetic::Individual Genetic::GeneticAlgorithm::one_point_crossover(Individual s1, Individual s2)
{
    // pick a random point (but not at the end of a chromosome), then
    // join two parts from different parents
    std::vector<bool> new_chromosome;
    int length = s1.chromosome.size();
    std::uniform_int_distribution<> d(1, length - 1);
    int point = d(rng);

    //std::cout << "Picked point " << point << std::endl;

    for(int i = 0; i < point; i++) {
        new_chromosome.push_back(s1.chromosome[i]);
    }

    for(int i = point; i < length; i++) {
        new_chromosome.push_back(s2.chromosome[i]);
    }

    return Individual(new_chromosome);
}

void Genetic::GeneticAlgorithm::fit(BooleanMatrix::BooleanMatrix& M, int verbose, bool finishing_message) {
    int m = M.get_m();
    int n = M.get_n();

    //CROSSOVER (creating extended population)
    int delta = extended_population_size - population_size;
    int chromosome_len = population[0].chromosome.size();
    std::uniform_int_distribution<> parents_d(0, population_size - 1);

    for(int i = 0; i < max_iter; i++) {
        std::vector<Individual> extended_population = population;
        for(int j = 0; j < delta; j++) {
            int p1, p2;
            do {
                p1 = parents_d(rng);
                p2 = parents_d(rng);
            } while (p1 == p2);
            Individual child = one_point_crossover(population[p1], population[p2]);
            extended_population.push_back(child);
        }

        //Mutate
        int mutations = 1000;
        for(int mut_iter = 0; mut_iter < mutations; mut_iter++) {
            std::bernoulli_distribution bernoulli_d(mutation_proba);
            std::uniform_int_distribution<> genes_d(0, chromosome_len - 1);
            for(int j = population_size + 1; j < extended_population_size; j++) {
                bool mutate = bernoulli_d(rng);
                if(!mutate)
                    continue;

                int random_gen = genes_d(rng);
                extended_population[j].chromosome[random_gen] = !extended_population[j].chromosome[random_gen];
            }
        }

        //Get scores
        std::vector<double> scores;
        for(int j = 0; j < extended_population_size; j++) {
            scores.push_back(extended_population[j].fitness(M));
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
    if(finishing_message) {
        std::cout << "Learning finished!" << std::endl;
        std::cout << std::endl;
    }

}

std::vector<bool> Genetic::GeneticAlgorithm::get_best_chromosome()
{
    return population[0].chromosome;
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

    std::cout << "Best chromosome: " << get_best_chromosome() << std::endl;
    std::cout << "Coverage: " << std::endl;
    std::vector<bool> best = get_best_chromosome();
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
    int chromosome_len = population[0].size();

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
