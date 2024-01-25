#include "BCGA.hpp"

BCGA::REncSotnezovBCGA::REncSotnezovBCGA(int population_size, std::vector<int> groups_idx, std::vector<float> ranks, RankType rank_type, Fitness optimize, int K, float C, int max_iter, int seed, OutputMode verbose):
                                         BCGA::EncSotnezovBCGA(population_size, groups_idx, optimize, K, C, max_iter, seed, verbose)
{
    this->columns_ranks = ranks;
    this->rank_type = rank_type;
    this->individual_ranks = std::vector<float>(extended_population_size);
}

float BCGA::REncSotnezovBCGA::rank(BinaryIndividual& individual)
{
    switch(rank_type) {
        case RankType::ElementWise: {
            float ind_rank = 0.0;
            int cov_len = 0;
            for(int i = 0; i < n; i++) {
                if(!individual.genotype[i])
                    continue;
                ind_rank += columns_ranks[i];
                cov_len++;
            }
            return ind_rank / cov_len;
        } case RankType::GroupWise: {
            float ind_rank = 0.0;
            for(int group = 0; group < groups_idx.size() - 1; group++) {
                float group_sum_rank = 0.0;
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
        float cur_rank = rank(population[i]);

        scores_sum += cur_fitness; // meh but ok
        individual_ranks[i] = cur_rank;

        if((cur_fitness < best_score) || ((cur_fitness == best_score) && (cur_rank > best_rank))) {
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
    apriori_queue = argsort(column_scores);
}

void BCGA::REncSotnezovBCGA::update_scores(BooleanMatrix::BooleanMatrix& M)
{
    for(int j = population_size; j < extended_population_size; j++) {
        scores[j] = fitness(M, population[j]);
        individual_ranks[j] = rank(population[j]);
    }
}

void BCGA::REncSotnezovBCGA::selection(int iteration)
{
    int child_score = scores[population_size];
    float child_rank = individual_ranks[population_size];
    bool child_in_population = false;
    bool hit_by_child = ((child_score < best_score) || ((child_score == best_score) && (child_rank > best_rank)));

    //if(((child_score == best_score) && (child_rank > best_rank)))
        //std::cout << "!!!!!!!!!!!!!!!!!!!" << std::endl;

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
    for(int i = 0; i < population_size; i++)
        if((child_score < scores[i]) || ((child_score == scores[i]) && (child_rank > individual_ranks[i])))
            worse.push_back(i);

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
