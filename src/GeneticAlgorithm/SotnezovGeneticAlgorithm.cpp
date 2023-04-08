#include "Genetic.hpp"

Genetic::SotnezovGeneticAlgorithm::SotnezovGeneticAlgorithm(int population_size, int max_iter): Genetic::BaseGeneticAlgorithm(population_size,
                                                                                                population_size + 1, 1.0, max_iter) {}

void Genetic::SotnezovGeneticAlgorithm::optimize_covering(BooleanMatrix::BooleanMatrix& M, std::vector<bool>& columns)
{
    int row_scores[m] = {};
    std::vector<int> column_scores(n);
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            if(M[i][j] & columns[j]) {
                row_scores[i]++;
                column_scores[j]++;
            }
        }
    }

    std::vector<int> queue = argsort(column_scores);
    for(int i = 0; i < n; i++) {
        //zero rows covered or not in covering
        if(!column_scores[queue[i]]) {
            if(columns[queue[i]])
                columns[queue[i]] = false;
            continue;
        }

        bool flag = true;
        for(int j = 0; j < m; j++) {
            if(M[j][queue[i]]) {
                if(row_scores[j] >= 2) {
                    continue;
                } else {
                    flag = false;
                    break;
                }
            }
        }

        if(flag) {
            columns[queue[i]] = false;
            for(int j = 0; j < m; j++) {
                // Decrease scores
                if(M[j][queue[i]])
                        row_scores[j]--;
            }
        }
    }
}

std::vector<bool> Genetic::SotnezovGeneticAlgorithm::get_covered_rows(BooleanMatrix::BooleanMatrix& M, std::vector<bool> columns)
{
    std::vector<bool> covered_rows;
    for(int i = 0; i < m; i++) {
        bool flag = true;
        for(int j = 0; j < n; j++) {
            if(M[i][j] & columns[j]) {
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

int Genetic::SotnezovGeneticAlgorithm::get_maxscore_column(BooleanMatrix::BooleanMatrix& M, std::vector<bool>& covered_rows, std::vector<bool>& columns, int row)
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

    //!!!
    columns[argmax_score] = true;

    return argmax_score;
}

void Genetic::SotnezovGeneticAlgorithm::create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len)
{
    population.clear();
    this->m = M.get_m();
    this->n = M.get_n();
    scores_sum = 0;
    best_score = n;

    double p = 0.5;
    std::bernoulli_distribution bd(p);
    for(int i = 0; i < population_size; i++) {
        std::vector<bool> new_genes;
        for(int j = 0; j < genotype_len; j++) {
            new_genes.push_back(bd(rng));
        }
        std::vector<bool> covered_rows = get_covered_rows(M, new_genes);
        for(int j = 0; j < covered_rows.size(); j++) {
            if(covered_rows[j])
                continue;
            //cover this and many (works with side effect)
            int argmax_score = get_maxscore_column(M, covered_rows, new_genes, j);
        }

        population.push_back(Individual(new_genes));
        //optimize_covering(M, population[i].genotype);

        int f = fitness(M, population[i]);
        scores_sum += f;
        if(f < best_score)
            best_score = f;
    }
}

void Genetic::SotnezovGeneticAlgorithm::get_parent_indices(int& p1, int& p2)
{
    // does nothing but called in base class
}

Genetic::Individual Genetic::SotnezovGeneticAlgorithm::crossover(Individual& parent1, Individual& parent2)
{
    std::vector<bool> new_genotype;

    //build probs
    double rel_sum = scores_sum - (best_score - 1) * population_size;
    if (rel_sum <= 0) {
        std::cout << "WARNING: REL_SUM = 1" << std::endl;
        rel_sum = 1;
    }
    std::vector<double> probs;
    for(int i = 0; i < scores.size(); i++) {
        int rel_score = scores[i] - best_score + 1;
        probs.push_back((1.0 / rel_score) / rel_sum);
    }

    // sample two parents
    std::discrete_distribution<> distr(probs.begin(), probs.end());
    int p1 = distr(rng);
    int p2 = distr(rng);
    std::cout << "Parents " << p1 << ", " << p2 << " with probs " << probs[p1] << ", " << probs[p2] << std::endl;
    int f1 = scores[p1] - best_score + 1;
    int f2 = scores[p2] - best_score + 1;
    double p = f2 / (f1 + f2);
    std::bernoulli_distribution bd(p);

    // create set of genes
    for(int i = 0; i < n; i++) {
        if(population[p1].genotype[i] == population[p2].genotype[i]) {
            new_genotype.push_back(population[p1].genotype[i]);
            continue;
        }
        if(bd(rng)) {
            new_genotype.push_back(population[p1].genotype[i]);
        } else {
            new_genotype.push_back(population[p2].genotype[i]);
        }
    }

    return Individual(new_genotype);
}

void Genetic::SotnezovGeneticAlgorithm::mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba, int parameter)
{
    double K = 1000.0;
    double C = 0.001;
    int number_of_mutations = K * (1.0 - 1.0 / (C * parameter + 1.0));
    std::cout << "Mutations: " << number_of_mutations << std::endl;
    int genotype_len = population[0].size();
    int child_idx = population_size;
    for(int mut_iter = 0; mut_iter < number_of_mutations; mut_iter++) {
        std::uniform_int_distribution<> genes_d(0, genotype_len - 1);
        int random_gen = genes_d(rng);
        population[child_idx].genotype[random_gen] = !population[child_idx].genotype[random_gen];
    }

    //reCOVERING (haha)
    std::vector<bool> covered_rows = get_covered_rows(M, population[child_idx].genotype);
    for(int j = 0; j < covered_rows.size(); j++) {
        if(covered_rows[j])
            continue;
        int argmax_score = get_maxscore_column(M, covered_rows, population[child_idx].genotype, j);
    }
    optimize_covering(M, population[child_idx].genotype);
}

double Genetic::SotnezovGeneticAlgorithm::fitness(BooleanMatrix::BooleanMatrix& M, Genetic::Individual& individual)
{
    int ones_counter = 0;
    for(int i = 0; i < individual.size(); i++) {
        if(individual.genotype[i])
            ones_counter++;
    }

    return ones_counter;
}

void Genetic::SotnezovGeneticAlgorithm::selection(int iteration, int verbose)
{
    int child_score = scores[population_size];

    bool child_in_population = false;
    for(int i = 0; i < population_size; i++) {
        bool flag = true;
        for(int j = 0; j < n; j++) {
            if(population[population_size].genotype[j] != population[i].genotype[j]) {
                flag = false;
                break;
            }
        }
        if(flag) {
            child_in_population = true;
            break;
        }
    }

    bool hit_by_child = (child_score < best_score);
    if(hit_by_child) {
        //std::cout << "New best score " << child_score << std::endl;
        best_score = child_score;
    }

    std::cout << "Generation: " << iteration << ", best score: " << best_score << ", already in this population: " << child_in_population << ", hit by child: " << hit_by_child;

    if(child_in_population) {
        std::cout << ", replaced: " << "-" << ", child score: " << child_score << std::endl;
        unluck_counter++;
        if(unluck_counter >= 10) {
            unluck_counter = 0;
            // recreate half population ?
            
        }
        population.pop_back();
        return;
    }

    std::vector<int> worse;
    for(int i = 0; i < population_size; i++) {
        if(child_score < scores[i])
            worse.push_back(i);
    }

    if(!worse.size()) {
        std::cout << ", replaced: " << "-" << ", child score: " << child_score << std::endl;
        unluck_counter++;
        if(unluck_counter >= 10) {
            unluck_counter = 0;
            // recreate half population ?
            
        }
        return;
    }

    std::uniform_int_distribution<> uid(0, worse.size() - 1);
    int random_individual = uid(rng);
    scores_sum -= (scores[worse[random_individual]] - child_score);
    scores[worse[random_individual]] = child_score;
    population[worse[random_individual]] = population[population_size];
    std::cout << ", replaced: " << worse[random_individual] << ", child score: " << child_score << std::endl;
    population.pop_back();
}
