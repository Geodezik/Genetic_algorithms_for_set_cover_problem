#include "BCGA.hpp"

std::vector<int> BCGA::GENCODE::conditional_argsort(const std::vector<int> &v, const std::vector<double> &c) {
    //Argsort, but for equal values: bigger c => earlier
    std::vector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::stable_sort(idx.begin(), idx.end(),
        [&v, &c](int i1, int i2) {return (v[i1] < v[i2]) || ((v[i1] == v[i2]) && (c[i1] > c[i2]));});

    return idx;
}

std::vector<int> BCGA::GENCODE::enc_conditional_argsort(const std::vector<int> &v) {
    //Argsort, but for equal values: bigger c => earlier
    std::vector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::vector<int> &a = columns_groups;
    std::vector<int> &b = group_counters;

    std::stable_sort(idx.begin(), idx.end(),
        [&v, &a, &b](int i1, int i2) {return (v[i1] < v[i2]) || ((v[i1] == v[i2]) && (b[a[i1]] > b[a[i2]]));});

    return idx;
}

BCGA::GENCODE::GENCODE(int population_size, std::vector<int> groups_idx, Fitness optimize, int K, double C, int max_iter, int seed, OutputMode verbose): BCGA::SotnezovBCGA(
                                                 population_size, K, C, max_iter, seed, verbose)
{
    this->groups_idx = groups_idx;
    this->group_counters = std::vector<int>(groups_idx.size());
    this->fit_function = optimize;
}

void BCGA::GENCODE::check_compatibility()
{
    if((groups_idx.size() == 0) || (groups_idx.size() > n))
        throw std::out_of_range("Wrong number of group's indices was given");
    if(groups_idx[0] != 0)
        throw std::invalid_argument("First group index must be zero");
    int max_idx = -1;
    for(auto idx: groups_idx) {
        if(idx <= max_idx)
            throw std::invalid_argument("Some of group indices are given in a wrong order");
        else if((idx < 0) || (idx >= n))
            throw std::out_of_range("Found wrong index (idx <= 0) or (idx >= n)");
        max_idx = idx;
    }

    // add fictive idx
    groups_idx.push_back(n);

    columns_groups = std::vector<int>(n);
    for(int group = 0; group < groups_idx.size() - 1; group++)
        for(int j = groups_idx[group]; j < groups_idx[group + 1]; j++)
            columns_groups[j] = group;  
}

void BCGA::GENCODE::fill_counters(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns)
{
    for(int i = 0; i < group_counters.size(); i++)
        group_counters[i] = 0;

    for(int group = 0; group < groups_idx.size() - 1; group++)
        for(int j = groups_idx[group]; j < groups_idx[group + 1]; j++)
            group_counters[group] += columns[j];
}

std::vector<double> BCGA::GENCODE::build_decreasing_counters(boost::dynamic_bitset<>& columns, std::vector<int>& columns_argsort)
{
    std::vector<int> group_counters_copy = group_counters;
    std::vector<double> decreasing_group_counters(n);

    for(auto idx: columns_argsort) {
        group_counters_copy[columns_groups[idx]] -= columns[idx];
        decreasing_group_counters[idx] = group_counters_copy[columns_groups[idx]];
    }
    
    return decreasing_group_counters;
}

void BCGA::GENCODE::optimize_covering(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns)
{
    fill_counters(M, columns);
    std::vector<int> row_scores(m);

    boost::dynamic_bitset<> bad_rows = boost::dynamic_bitset<>(m, 0);
    for(int i = 0; i < m; i++) {
        row_scores[i] = (M.rows[i] & columns).count();
        bad_rows[i] = (row_scores[i] <= 1);
    }

    // iterate through columns (from worst to best), exclude if can
    std::vector<int> queue;
    std::vector<double> decreasing_group_counters;

    switch(fit_function) {
        case Fitness::CovLen:
            queue = apriori_queue;
            break;
        case Fitness::MaxBinsNum:
        case Fitness::Mixed:
            //queue = enc_conditional_argsort(column_scores);
            queue = apriori_queue;
            decreasing_group_counters = build_decreasing_counters(columns, queue);
            queue = conditional_argsort(column_scores, decreasing_group_counters);
            break;
        default:
            throw std::invalid_argument("Unknown fitness function in optimization call");
    }

    for(int i = 0; i < n; i++) {
        if(!columns[queue[i]])
            continue;

        if(group_counters[columns_groups[queue[i]]] == 1)
            continue;

        //zero rows covered
        if(column_scores[queue[i]] == 0) {
            columns[queue[i]] = false;
            group_counters[columns_groups[queue[i]]]--;
            continue;
        }

        if((bad_rows & M.columns[queue[i]]).any())
            continue;

        columns[queue[i]] = false;
        group_counters[columns_groups[queue[i]]]--;

        // Decrease scores
        for(int j = 0; j < m; j++) {
            row_scores[j] -= M.get(j, queue[i]);
            bad_rows[j] = (row_scores[j] <= 1);
        }
    }
}

void BCGA::GENCODE::restore_solution(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns)
{
    std::vector<bool> covered_rows = get_covered_rows(M, columns);

    fill_counters(M, columns);
    for(int group = 0; group < groups_idx.size() - 1; group++) {
        if(group_counters[group] > 0)
            continue;

        // add random column from group
        std::uniform_int_distribution<> columns_d(groups_idx[group], groups_idx[group + 1] - 1);
        int random_column = columns_d(rng);
        columns[random_column] = true;
        for(int i = 0; i < m; i++)
            covered_rows[i] = (covered_rows[i] || M.get(i, random_column));
    }

    for(int i = 0; i < covered_rows.size(); i++) {
        if(covered_rows[i])
            continue;
        add_maxscore_column(M, covered_rows, columns, i, 0, n);
    }
}

double BCGA::GENCODE::covlen_fitness(BooleanMatrix::BooleanMatrix& M, BCGA::BinaryIndividual& individual)
{
    int ones_counter = 0;
    for(int i = 0; i < individual.size(); i++)
        ones_counter += individual.genotype[i];
    if(ones_counter < groups_idx.size() - 1)
        throw "A bug was detected";
    return ones_counter;
}

double BCGA::GENCODE::maxbinsnum_fitness(BooleanMatrix::BooleanMatrix& M, BCGA::BinaryIndividual& individual)
{
    fill_counters(M, individual.genotype);
    int max_bin_num = 0;
    for(auto bin_num: group_counters)
        if(bin_num > max_bin_num)
            max_bin_num = bin_num;
    return max_bin_num + 1;
}

double BCGA::GENCODE::mixed_fitness(BooleanMatrix::BooleanMatrix& M, BCGA::BinaryIndividual& individual)
{
    int maxbinsnum = maxbinsnum_fitness(M, individual);
    int covlen = covlen_fitness(M, individual);
    return maxbinsnum + covlen / (groups_idx.size() - 1);
}

double BCGA::GENCODE::fitness(BooleanMatrix::BooleanMatrix& M, BCGA::BinaryIndividual& individual)
{
    switch(fit_function) {
        case Fitness::CovLen:
            return covlen_fitness(M, individual);
        case Fitness::MaxBinsNum:
            return maxbinsnum_fitness(M, individual);
        case Fitness::Mixed:
            return mixed_fitness(M, individual);
        default:
            throw std::invalid_argument("Unknown fitness function in fitness call");
    }

    throw std::invalid_argument("Unknown fitness function in fitness call");
    return 0;
}

void BCGA::GENCODE::analyze_solution(BooleanMatrix::BooleanMatrix& M)
{
    if(!is_fitted)
        throw std::runtime_error("NotFittedError");

    int best_covlen = n;
    int best_maxbinsnum = n;
    int best_mixed = n;

    for(auto individual: population) {
        int score = n;
        score = covlen_fitness(M, individual);
        best_covlen = (best_covlen > score) ? score : best_covlen;
        score = maxbinsnum_fitness(M, individual);
        best_maxbinsnum = (best_maxbinsnum > score) ? score : best_maxbinsnum;
    }

    switch(verbose) {
        case OutputMode::Silent:
            break;
        case OutputMode::Normal:
            std::cout << best_covlen << ' ' << best_maxbinsnum << std::endl;
            break;
        case OutputMode::Max:
            std::cout << "Best CovLen: " << best_covlen << std::endl;
            std::cout << "Best MaxBinsNum " << best_maxbinsnum << std::endl;
            std::cout << std::endl;
            break;
    }
}
