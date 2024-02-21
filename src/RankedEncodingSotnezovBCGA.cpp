#include "BCGA.hpp"

BCGA::REncSotnezovBCGA::REncSotnezovBCGA(int population_size, std::vector<int> groups_idx, std::vector<double> ranks, RankType rank_type, Fitness optimize, int K, double C, double alpha, int max_iter, int norank_iter, int seed, OutputMode verbose):
                                         BCGA::EncSotnezovBCGA(population_size, groups_idx, optimize, K, C, max_iter, seed, verbose)
{
    this->columns_ranks = ranks;
    this->rank_type = rank_type;
    this->norank_iter = norank_iter;
    this->alpha = alpha;
    if((alpha < 0.0) || (alpha > 1.0))
        throw std::out_of_range("Invalid alpha value");
    this->individual_ranks = std::vector<double>(extended_population_size);
}

double BCGA::REncSotnezovBCGA::rank(BinaryIndividual& individual)
{
    switch(rank_type) {
        case RankType::ElementWise: {
            double ind_rank = 0.0;
            int cov_len = 0;
            for(int i = 0; i < n; i++) {
                if(!individual.genotype[i])
                    continue;
                ind_rank += columns_ranks[i];
                cov_len++;
            }
            return ind_rank / cov_len;
        } case RankType::GroupWise: {
            double ind_rank = 0.0;
            for(int group = 0; group < groups_idx.size() - 1; group++) {
                double group_sum_rank = 0.0;
                int intersection_len = 0;
                for(int i = groups_idx[group]; i < groups_idx[group + 1]; i++) {
                    if(!individual.genotype[i])
                        continue;
                    group_sum_rank += columns_ranks[i];
                    intersection_len++;
                }
                ind_rank += group_sum_rank / intersection_len;
            }
            return ind_rank / (groups_idx.size() - 1);
        } case RankType::Sum: {
            double ind_rank = 0.0;
            for(int i = 0; i < n; i++) {
                if(!individual.genotype[i])
                    continue;
                ind_rank += columns_ranks[i];
            }
            return ind_rank;
        } default:
            throw std::invalid_argument("Unknown rank function in rank call");
    }

    throw std::invalid_argument("Unknown rank function in rank call");
    return 0;
}

void BCGA::REncSotnezovBCGA::create_zero_generation(BooleanMatrix::BooleanMatrix& M, int genotype_len)
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

        int cur_fitness = fitness(M, population[i]);
        double cur_rank = rank(population[i]);

        scores_sum += cur_fitness; // meh but ok
        individual_ranks[i] = cur_rank;

        if((cur_fitness < best_score) || ((cur_fitness == best_score) && (cur_rank < best_rank))) {
            best_score = cur_fitness;
            best_index = i;
            best_rank = cur_rank;
        }
    }

    //count apriori_column_scores
    column_scores = std::vector<int>();
    for(int j = 0; j < n; j++) {
        int s = 0;
        for(int i = 0; i < m; i++)
            s += M.get(i, j);
        column_scores.push_back(s);
    }

    // commented doesn't work well...
    //apriori_queue = conditional_argsort(column_scores, individual_ranks);
    apriori_queue = argsort(column_scores);
}

void BCGA::REncSotnezovBCGA::update_scores(BooleanMatrix::BooleanMatrix& M)
{
    for(int j = population_size; j < extended_population_size; j++) {
        scores[j] = fitness(M, population[j]);
        individual_ranks[j] = rank(population[j]);
    }
}

BCGA::BinaryIndividual BCGA::REncSotnezovBCGA::crossover(BinaryIndividual& parent1, BinaryIndividual& parent2)
{
    boost::dynamic_bitset<> new_genotype;
    double beta = alpha * (iteration >= norank_iter);

    //build probs
    std::vector<double> rank_probs;
    std::vector<double> scores_probs;
    std::vector<double> probs;
    double inv_score_sum = 0.0;
    double inv_rank_sum = 0.0;
    for(int i = 0; i < population_size; i++) {
        double inv_rel_rank = 1.0 / (individual_ranks[i]);
        double inv_rel_score = 1.0 / (scores[i] - best_score + 1);
        inv_rank_sum += inv_rel_rank;
        inv_score_sum += inv_rel_score;
        rank_probs.push_back(inv_rel_rank);
        scores_probs.push_back(inv_rel_score);
    }
    for(int i = 0; i < population_size; i++)
        probs.push_back(beta * rank_probs[i] / inv_rank_sum + (1 - beta) * scores_probs[i] / inv_score_sum);

    // sample two parents
    std::discrete_distribution<> distr(probs.begin(), probs.end());
    int p1 = distr(rng);
    int p2 = distr(rng);
    double r1 = individual_ranks[p1];
    double r2 = individual_ranks[p2];
    double f1 = scores[p1] - best_score + 1;
    double f2 = scores[p2] - best_score + 1;
    double prob1 = beta * (r2 / (r1 + r2));
    double prob2 = (1 - beta) * (f2 / (f1 + f2));
    double p = prob1 + prob2;
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

void BCGA::REncSotnezovBCGA::selection()
{
    int child_score = scores[population_size];
    double child_rank = individual_ranks[population_size];
    bool child_in_population = false;
    bool hit_by_child = ((child_score < best_score) || ((child_score == best_score) && (child_rank < best_rank)));

    if(hit_by_child) {
        best_score = child_score;
        best_rank = child_rank;
    } else {
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
            std::cout << "Generation: " << iteration << ", best score: " << best_score << ", best rank: " << best_rank << std::endl;
            break;
        case OutputMode::Max:
            std::cout << "Generation: " << iteration << ", best score: " << best_score << ", best rank: " << best_rank;
            std::cout << ", already in this population: " << child_in_population << ", hit by child: " << hit_by_child;
    }

    // find all the worse individuals indices
    std::vector<int> worse;
    if(iteration >= norank_iter) {
        for(int i = 0; i < population_size; i++)
            if((child_score < scores[i]) || ((child_score == scores[i]) && (child_rank < individual_ranks[i])))
                worse.push_back(i);
    } else {
        for(int i = 0; i < population_size; i++)
            if(child_score < scores[i])
                worse.push_back(i);
    }

    // track cases when child should NOT be included
    bool bad_child = (worse.size() == 0);
    if(child_in_population || bad_child) {
        if(verbose == OutputMode::Max)
            std::cout << ", replaced: " << "None" << ", child score: " << child_score << ", child rank: " << child_rank << std::endl;
        population.pop_back();
        return;
    }

    std::uniform_int_distribution<> uid(0, worse.size() - 1);
    int random_individual = uid(rng);
    scores_sum -= (scores[worse[random_individual]] - child_score);
    scores[worse[random_individual]] = child_score;
    population[worse[random_individual]] = population[population_size];

    if(hit_by_child)
        best_index = worse[random_individual];
    if(verbose == OutputMode::Max)
        std::cout << ", replaced: " << worse[random_individual] << ", child score: " << child_score << ", child rank: " << child_rank << std::endl;
    population.pop_back();
}
