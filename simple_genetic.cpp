#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <numeric>
#include <algorithm>

/*
TODO
Chromosome is a vector => very bad (try class template?)
*/

class GeneticAlgorithm;

class Individual {
    std::vector<bool> chromosome;

public:
    Individual(int chromosome_size)
    {
        double p = 0.5;

        std::random_device rd{};
        std::mt19937 rng{rd()};
        std::bernoulli_distribution d(p);

        //creating chromosome from random bits
        chromosome = std::vector<bool>();
        for(int i=0; i < chromosome_size; i++) {
            chromosome.push_back(d(rng));
        }
    };

    Individual(std::vector<bool> chromosome)
    {
        this->chromosome = chromosome;
    };


    template<std::size_t n, std::size_t m>
    bool is_coverage(std::array<std::array<bool, n>, m>& M)
    {
        //Sum of all taken columns (by rows, we sum rows) shouldn't include zeros
        std::array<int, m> rows_sum = {};
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                if(!chromosome[j])
                    continue;
                rows_sum[i] += M[i][j];
            }
        }

        for(int i = 0; i < m; i++) {
            //std::cout << rows_sum[i] << std::endl;
            if(!rows_sum[i])
                return false;
        }

        return true;
    };

    template<std::size_t n, std::size_t m>
    double fitness(std::array<std::array<bool, n>, m>& M)
    {
        if(!is_coverage(M))
            return n + 1;

        int sum = 0;
        for(auto elem: chromosome) {
            sum += elem;
        }
        return sum;
    };

    friend GeneticAlgorithm;
    friend std::ostream& operator<<(std::ostream& os, const Individual& I);
};

std::ostream& operator<<(std::ostream& os, const Individual& I)
{
    for(int i = 0; i < I.chromosome.size(); i++) {
        os << I.chromosome[i] << " ";
    }
    return os;
};

class GeneticAlgorithm {
    std::vector<Individual> population;
    std::vector<int> scores;

    std::mt19937 rng;

    int max_iter;
    int population_size;
    int extended_population_size;
    double mutation_proba;

    template <typename T>
    std::vector<int> argsort(const std::vector<T> &v) {
        //Argsort for any type of elem-comparable vectors
        std::vector<int> idx(v.size());
        std::iota(idx.begin(), idx.end(), 0);

        std::stable_sort(idx.begin(), idx.end(),
            [&v](int i1, int i2) {return v[i1] < v[i2];});

        return idx;
    }

public:
    GeneticAlgorithm(int population_size, int extended_population_size, int chromosome_len,
                     double mutation_proba, int max_iter = 100)
    {
        this->population_size = population_size;
        this->extended_population_size = extended_population_size;
        this->max_iter = max_iter;
        this->mutation_proba =  mutation_proba;

        double p = 0.5;

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
    };

    Individual one_point_crossover(Individual s1, Individual s2)
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
    };

    template<std::size_t n, std::size_t m>
    void fit(std::array<std::array<bool, n>, m>& M, int verbose=2, bool finishing_message=true) {
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
            std::bernoulli_distribution bernoulli_d(mutation_proba);
            std::uniform_int_distribution<> genes_d(0, chromosome_len - 1);
            for(int j = 0; j < extended_population_size; j++) {
                bool mutate = bernoulli_d(rng);
                if(!mutate)
                    continue;

                int random_gen = genes_d(rng);
                extended_population[j].chromosome[random_gen] = !extended_population[j].chromosome[random_gen];
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
            for(int j = 0; j < population_size; j++) {
                best.push_back(extended_population[argbest[j]]);
            }

            population = best;
        }
        if(finishing_message)
            std::cout << "Learning finished!" << std::endl;
    };

    std::vector<bool> get_best_chromosome()
    {
        return population[0].chromosome;
    };

    template<std::size_t n, std::size_t m>
    void print_solution(std::array<std::array<bool, n>, m>& M)
    {
        if(n > 25) {
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
    };
};

int main()
{
    //Create data
    const int m = 500;
    const int n = m;
    std::array<std::array<bool, n>, m> M = {};

    //int population_size, int extended_population_size, int chromosome_len, double mutation_proba, int max_iter = 100
    GeneticAlgorithm A = GeneticAlgorithm(50, 150, n, 0.15, 100);

    double p = 0.5;
    std::random_device rd{};
    std::mt19937 rng{rd()};
    std::bernoulli_distribution d(p);

    
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            M[i][j] = static_cast<bool> (d(rng));
        }
    }


    A.fit(M, 0, false);
    //A.print_solution(M);

    return 0;
}