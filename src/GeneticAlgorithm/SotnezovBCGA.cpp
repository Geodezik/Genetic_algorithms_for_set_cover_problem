#include "BCGA.hpp"

std::vector<int> BCGA::SotnezovBCGA::argsort(const std::vector<int>& v) {
    //Argsort for any type of elem-comparable vectors
    std::vector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::stable_sort(idx.begin(), idx.end(),
        [&v](int i1, int i2) {return v[i1] < v[i2];});

    return idx;
}

BCGA::SotnezovBCGA::SotnezovBCGA(int population_size, int K, float C, int max_iter, int seed, OutputMode verbose): BCGA::BaseBCGA(population_size,
                                 population_size + 1, 1.0, max_iter, seed, verbose)
{
    if((K <= 0) || (C <= 0.0))
        throw std::invalid_argument("Wrong value for K or C parameter(s)");
    this->K = K;
    this->C = C;
}

void BCGA::SotnezovBCGA::optimize_covering(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns)
{
    // possible upgrade: sort not all the indices but covering indices
    std::vector<int> row_scores(m);
    for(int j = 0; j < n; j++) {
        if(!columns[j])
            continue;
        for(int i = 0; i < m; i++)
            row_scores[i] += M.get(i, j);
    }

    // iterate through columns (from worst to best), exclude if can
    std::vector<int> queue = apriori_queue;
    for(int j = 0; j < n; j++) {
        if(!columns[queue[j]])
            continue;

        //zero rows covered or not in covering
        if(column_scores[queue[j]] == 0) {
            columns[queue[j]] = false;
            continue;
        }

        bool flag = true; // can be kicked?
        for(int i = 0; i < m; i++)
            if(M.get(i, queue[j]) && (row_scores[i] < GlobalSettings::SotnezovThreshold)) {
                // column is important
                flag = false;
                break;
            }

        if(!flag)
            continue;

        columns[queue[j]] = false;
        for(int i = 0; i < m; i++) {
            // Decrease scores
            if(M.get(i, queue[j]))
                row_scores[i]--;
        }
    }
}

std::vector<bool> BCGA::SotnezovBCGA::get_covered_rows(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns)
{
    std::vector<bool> covered_rows;
    for(int i = 0; i < m; i++) {
        bool flag = false;
        for(int j = 0; j < n; j++)
            if(M.get(i, j) & columns[j]) {
                flag = true;
                break;
            }
        covered_rows.push_back(flag);
    }

    return covered_rows;
}

void BCGA::SotnezovBCGA::add_maxscore_column(BooleanMatrix::BooleanMatrix& M, std::vector<bool>& covered_rows, boost::dynamic_bitset<>& columns, int row, int from, int to)
{
    int max_score = -1;
    int argmax_score = from;
    for(int i = from; i < to; i++) {
        //i doesn't cover this row or already added
        if(!M.get(row, i) || columns[i])
            continue;
        int score = 1;
        for(int j = row + 1; j < m; j++)
            score += (M.get(j, i) && !covered_rows[j]);
        if(score > max_score) {
            max_score = score;
            argmax_score = i;
        }
    }

    //covers some others
    for(int i = 0; i < m; i++)
        if(M.get(i, argmax_score))
            covered_rows[i] = true;

    //!!!
    columns[argmax_score] = true;
}

void BCGA::SotnezovBCGA::restore_solution(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns)
{
    std::vector<bool> covered_rows = get_covered_rows(M, columns);
    for(int j = 0; j < covered_rows.size(); j++) {
        if(covered_rows[j])
            continue;
        //cover this and many others (works with side effect)
        add_maxscore_column(M, covered_rows, columns, j, 0, n);
    }
}

void BCGA::SotnezovBCGA::create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len)
{
    population.clear();
    this->m = M.get_m();
    this->n = M.get_n();
    scores_sum = 0;
    best_score = n;

    double p = GlobalSettings::SotnezovInitProbability;
    std::bernoulli_distribution bd(p);
    for(int i = 0; i < population_size; i++) {
        boost::dynamic_bitset<> new_genes;
        for(int j = 0; j < genotype_len; j++)
            new_genes.push_back(bd(rng));

        restore_solution(M, new_genes);
        population.push_back(BinaryIndividual(new_genes));
        //optimize_covering(M, population[i].genotype);

        scores_sum += fitness(M, population[i]); // meh
    }

    //count apriori_column_scores
    column_scores = std::vector<int>();
    for(int j = 0; j < n; j++) {
        int s = 0;
        for(int i = 0; i < m; i++)
            s += M.get(i, j);
        column_scores.push_back(s);
    }
    apriori_queue = argsort(column_scores);
}

void BCGA::SotnezovBCGA::get_parent_indices(int& p1, int& p2)
{
    // does nothing but called in base class
}

BCGA::BinaryIndividual BCGA::SotnezovBCGA::crossover(BinaryIndividual& parent1, BinaryIndividual& parent2)
{
    boost::dynamic_bitset<> new_genotype;

    //build probs
    double rel_sum = scores_sum - (best_score - 1) * population_size;
    if (rel_sum <= 0) {
        std::cout << "WARNING: REL_SUM = 1" << std::endl;
        rel_sum = 1;
    }
    std::vector<double> probs;
    for(int i = 0; i < population_size; i++) {
        int rel_score = scores[i] - best_score + 1;
        probs.push_back((1.0 / rel_score) / rel_sum);
    }

    // sample two parents
    std::discrete_distribution<> distr(probs.begin(), probs.end());
    int p1 = distr(rng);
    int p2 = distr(rng);
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
        if(bd(rng))
            new_genotype.push_back(population[p1].genotype[i]);
        else
            new_genotype.push_back(population[p2].genotype[i]);
    }

    return BinaryIndividual(new_genotype);
}

void BCGA::SotnezovBCGA::mutate(BooleanMatrix::BooleanMatrix& M, double mutation_proba, int parameter)
{
    int number_of_mutations = K * (1.0 - 1.0 / (C * parameter + 1.0));

    int genotype_len = M.get_n();
    int child_idx = population_size;
    std::uniform_int_distribution<> genes_d(0, genotype_len - 1);

    for(int mut_iter = 0; mut_iter < number_of_mutations; mut_iter++) {
        int random_gen = genes_d(rng);
        population[child_idx].genotype[random_gen] = !population[child_idx].genotype[random_gen];
    }

    restore_solution(M, population[child_idx].genotype);
    optimize_covering(M, population[child_idx].genotype);
}

int BCGA::SotnezovBCGA::fitness(BooleanMatrix::BooleanMatrix& M, BCGA::BinaryIndividual& individual)
{
    int ones_counter = 0;
    for(int i = 0; i < individual.size(); i++)
        ones_counter += individual.genotype[i];

    return ones_counter;
}

void BCGA::SotnezovBCGA::selection(int iteration)
{
    int child_score = scores[population_size];
    bool child_in_population = false;
    bool hit_by_child = (child_score < best_score);
    if(hit_by_child) 
        best_score = child_score;
    else {
        for(int i = 0; i < population_size; i++) {
            bool flag = true;
            for(int j = 0; j < n; j++)
                if(population[population_size].genotype[j] != population[i].genotype[j]) {
                    flag = false;
                    break;
                }

            if(!flag)
                continue;

            child_in_population = true;
            break;
        }
    }

    switch(verbose) {
        case OutputMode::Normal:
            std::cout << "Generation: " << iteration << ", best score: " << best_score << std::endl;
            break;
        case OutputMode::Max:
            std::cout << "Generation: " << iteration << ", best score: " << best_score;
            std::cout << ", already in this population: " << child_in_population << ", hit by child: " << hit_by_child;
    }

    // find all the worse individuals indices
    std::vector<int> worse;
    for(int i = 0; i < population_size; i++)
        if(child_score < scores[i])
            worse.push_back(i);

    // track cases when child should NOT be included
    bool bad_child = (worse.size() == 0);
    if(child_in_population || bad_child) {
        if(verbose == OutputMode::Max)
            std::cout << ", replaced: " << "None" << ", child score: " << child_score << std::endl;
        population.pop_back();
        return;
    }

    // replace with % in future?
    std::uniform_int_distribution<> uid(0, worse.size() - 1);
    int random_individual = uid(rng);
    scores_sum -= (scores[worse[random_individual]] - child_score);
    scores[worse[random_individual]] = child_score;
    population[worse[random_individual]] = population[population_size];
    if(hit_by_child)
        best_index = worse[random_individual];
    if(verbose == OutputMode::Max)
        std::cout << ", replaced: " << worse[random_individual] << ", child score: " << child_score << std::endl;
    population.pop_back();
}

void BCGA::SotnezovBCGA::print_columns_to_file(std::string filename) {
    if(!is_fitted)
        throw std::runtime_error("NotFittedError");
    std::ofstream f;
    f.open(filename, std::ofstream::app);
    boost::dynamic_bitset<> best_solution = get_best_individual().genotype;
    for(int i = 0; i < best_solution.size(); i++) {
        if(best_solution[i] == 0)
            continue;
        f << i << '\n';
    }
    f.close();
}
