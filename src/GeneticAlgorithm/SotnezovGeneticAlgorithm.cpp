#include "Genetic.hpp"

Genetic::SotnezovGeneticAlgorithm::SotnezovGeneticAlgorithm(int population_size, int extended_population_size, double mutation_proba,
                                                            int max_iter): Genetic::BaseGeneticAlgorithm(population_size, extended_population_size,
                                                                                                         mutation_proba, max_iter) {}

void Genetic::SotnezovGeneticAlgorithm::optimize_covering(BooleanMatrix::BooleanMatrix& M, std::vector<bool>& columns)
{
    // get score for each row (how many columns from covering cover it)
    // count covered rows for each column, argsort columns by this
    // go through argsort, remove columns that cover only rows with scores >= 2, decrease rows scores

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
            // if i covers this row and this row has score >=2 => OK. Otherwise drop flag and break.
            if(M[j][queue[i]]) {
                if(row_scores[j] >= 2) {
                    continue;
                } else {
                    flag = false;
                    break;
                }
            }
        }

        //if flag is true => can be kicked (and decrease scores in a loop)
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
    this->m = M.get_m();
    this->n = M.get_n();

    if(!best_score) {
        std::cout << "Default best score " << n << std::endl;
        best_score = n;
    }

    population.clear();
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
        optimize_covering(M, population[i].genotype);

        int f = fitness(M, population[i]);
        //std::cout << f << ' ';
        if(f < best_score) {
            //std::cout << "New best score " << f << std::endl;
            best_score = f;
        }
    }
    //std::cout << std::endl;
}

Genetic::Individual Genetic::SotnezovGeneticAlgorithm::crossover(Individual& parent1, Individual& parent2)
{
    std::vector<bool> new_genotype;
    // count relative scores (don't forget +1)
    // find sum of all relatives
    // create list of probabilities (1/fi) / sum
    // sample two parents from distribution (different?)
    // go through genes (get parents' relatives before) and choose FIRST gen with p = f2/(f1+f2) second otherwise
    // create an Individual from this vector.

    // find rel_scores and rel_sum
    int min_score = n + 1;
    for(int i = 0; i < scores.size(); i++) {
        if(scores[i] < min_score)
            min_score = scores[i];
    }
    min_score--;
    double rel_sum = 0;
    std::vector<int> rel_scores = scores;
    for(int i = 0; i < scores.size(); i++) {
        rel_scores[i] -= min_score;
        rel_sum += rel_scores[i];
    }

    //build probs
    std::vector<double> probs;
    for(int i = 0; i < scores.size(); i++) {
        probs.push_back((1.0 / scores[i]) / rel_sum);
    }
    // std::cout << "SIZE" << probs.size() << ' ' << std::endl;

    // sample two parents
    std::discrete_distribution<> distr(probs.begin(), probs.end());
    int p1 = distr(rng);
    int p2 = distr(rng);
    int f1 = rel_scores[p1];
    int f2 = rel_scores[p2];
    double p = f2 / (f1 + f2);
    //std::cout << rel_scores[0] << rel_scores[1] << std::endl;
    std::bernoulli_distribution bd(p);

    // create set of genes
    for(int i = 0; i < n; i++) {
        if(population[p1].genotype[i] == population[p2].genotype[i]) {
            new_genotype.push_back(population[p1].genotype[i]);
            continue;
        }
        //std::cout << p1 << ' ';
        //std::cout << population[p1].genotype[i];
        //new_genotype.push_back(population[p1].genotype[i]);
        if(bd(rng)) {
            new_genotype.push_back(population[p1].genotype[i]);
        } else {
            new_genotype.push_back(population[p2].genotype[i]);
        }
    }
    // std::cout << std::endl;

    // std::cout << p1 << p2 << std::endl;
    // new_genotype = population[0].genotype;

    return Individual(new_genotype);
}

void Genetic::SotnezovGeneticAlgorithm::mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba, int parameter)
{
    double K = 500.0;
    // double C = 0.00001;
    double C = 0.00001;
    int number_of_mutations = K * (1.0 - 1.0 / (C * parameter + 1.0));
    std::cout << "Mutations: " << number_of_mutations << std::endl;
    int genotype_len = population[0].size();
    int child_idx = population_size;
    for(int mut_iter = 0; mut_iter < number_of_mutations; mut_iter++) {
        std::uniform_int_distribution<> genes_d(0, genotype_len - 1);
        int random_gen = genes_d(rng);
        population[child_idx].genotype[random_gen] = !population[child_idx].genotype[random_gen];
    }

    //recovering
    std::vector<bool> covered_rows = get_covered_rows(M, population[child_idx].genotype);
    for(int j = 0; j < covered_rows.size(); j++) {
        if(covered_rows[j])
            continue;
        //cover this and many (works with side effect)
        int argmax_score = get_maxscore_column(M, covered_rows, population[child_idx].genotype, j);
    }
    // + make irreducible
    optimize_covering(M, population[child_idx].genotype);
    //std::cout << "Child fitness: " << fitness(M, population[child_idx]) << std::endl;
    // std::cout << M.is_covered_by(population[child_idx].genotype) << std::endl;
}

double Genetic::SotnezovGeneticAlgorithm::fitness(BooleanMatrix::BooleanMatrix& M, Genetic::Individual& individual)
{
    // Cheap!
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

    // check if child (the last one) is in the population
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

    std::cout << "Generation: " << iteration << ", best score: " << best_score << ", already in population: " << child_in_population << ", hit by child: " << hit_by_child << ", replacements: ";

    if(child_in_population) {
        std::cout << 0 << ", child score: " << child_score << std::endl;
        unluck_counter++;
        if(unluck_counter >= 10) {
            unluck_counter = 0;
            // recreate half population
            
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
        std::cout << 0 << ", child score: " << child_score << std::endl;
        unluck_counter++;
        if(unluck_counter >= 10) {
            unluck_counter = 0;
            // recreate half population
            
        }
        return;
    }

    //replace one of bad
    std::cout << 1 << ", child score: " << child_score << std::endl;
    std::uniform_int_distribution<> uid(0, worse.size() - 1);
    int random_individual = uid(rng);
    scores[worse[random_individual]] = child_score;
    population[worse[random_individual]] = population[population_size];
    population.pop_back();
}
