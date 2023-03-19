#include "Genetic.hpp"

Genetic::SotnezovGeneticAlgorithm::SotnezovGeneticAlgorithm(int population_size, int extended_population_size, double mutation_proba,
                                                            int max_iter): Genetic::BaseGeneticAlgorithm(population_size, extended_population_size,
                                                                                                         mutation_proba, max_iter) {}

std::vector<bool> Genetic::SotnezovGeneticAlgorithm::get_covered_rows(std::vector<bool> columns)
{
    std::vector<bool> covered_rows;
    for(int i = 0; i < m; i++) {
        bool flag = true;
        for(int j = 0; j < n; j++) {
            if(columns[j] & M[i][j]) {
                covered_rows.push_back(true);
                flag = false;
                break;
            }
        }
        if(flag)
            covered_rows.push_back(false);
    }

    return covered_rows;
}

int Genetic::SotnezovGeneticAlgorithm::get_maxscore_column(std::vector<bool>& covered_rows, std::vector<bool>& columns, int row)
{
    int max_score = 0;
    int argmax_score = 0;
    for(int i = 0; i < n; i++) {
        //i doesn't cover this row
        if(!M[row][i] || columns[i])
            continue;
        int score = 1;
        for(int j = row + 1; j < m; j++) {
            score += (M[j][i] && !covered_rows[j]);
        }
        if(score > max_score) {
            max_score = score;
            argmax_score = i;
        }
    }

    //covers some others
    for(int i = 0; i < m; i++) {
        if(M[i][argmax_score])
            covered_rows[i] = true;
    }

    return argmax_score;
}

void Genetic::SotnezovGeneticAlgorithm::create_zero_generation(int genotype_len)
{
    population.clear();
    double p = 0.5;
    std::bernoulli_distribution bd(p);
    for(int i = 0; i < population_size; i++) {
        std::vector<bool> new_genes;
        for(int j = 0; j < genotype_len; j++) {
            new_genes.push_back(bd(rng));
        }
        std::vector<bool> covered_rows = get_covered_rows(new_genes);
        for(int j = 0; j < covered_rows.size(); j++) {
            if(covered_rows[j])
                continue;
            //cover this and many (works with side effect)
            int argmax_score = get_maxscore_column(covered_rows, new_genes, j);
        }
        population.push_back(Individual(new_genes));
    }
}

Genetic::Individual Genetic::SotnezovGeneticAlgorithm::crossover(Individual& parent1, Individual& parent2)
{
    std::vector<bool> new_genotype;
    return Individual(new_genotype);
}

void Genetic::SotnezovGeneticAlgorithm::mutate(double mutation_proba, int parameter)
{

}

double Genetic::SotnezovGeneticAlgorithm::fitness(Genetic::Individual& individual)
{
    return 0.0;
}

void Genetic::SotnezovGeneticAlgorithm::selection(int iteration, int verbose)
{

}
