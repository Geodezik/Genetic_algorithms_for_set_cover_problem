#include "BCGA.hpp"

BCGA::CODE3::CODE3(int population_size, std::vector<int> groups_idx, Fitness optimize, int K, double C, int max_iter, int seed, OutputMode verbose): BCGA::GENCODE(
                                                 population_size, groups_idx, optimize, K, C, max_iter, seed, verbose)
{
    // fill ???
}

double BCGA::CODE3::fitness(BooleanMatrix::BooleanMatrix& M, BinaryIndividual& individual)
{
    double s = 0;
    int counter = 0;

    for(int j = 0; j < individual.size(); j++) {
        if((individual.genotype[j] == 0) || (column_scores[j] == 0))
            continue;
        counter++;
        s += 1.0 / column_scores[j];
    }
    return -s / counter;
}

void BCGA::CODE3::optimize_covering(BooleanMatrix::BooleanMatrix& M, boost::dynamic_bitset<>& columns)
{
    fill_counters(M, columns);
    std::vector<int> row_scores(m);

    boost::dynamic_bitset<> bad_rows = boost::dynamic_bitset<>(m, 0);
    for(int i = 0; i < m; i++) {
        row_scores[i] = (M.rows[i] & columns).count();
        bad_rows[i] = (row_scores[i] <= 1);
    }

    std::vector<int> queue = apriori_queue;

    for(int i = 0; i < n; i++) {
        if(!columns[queue[i]])
            continue;

        if(group_counters[columns_groups[queue[i]]] == 1)
            continue;

        if(column_scores[queue[i]] == 0) {
            columns[queue[i]] = false;
            group_counters[columns_groups[queue[i]]]--;
            continue;
        }

        if((bad_rows & M.columns[queue[i]]).any())
            continue;

        columns[queue[i]] = false;
        for(int j = 0; j < m; j++) {
            row_scores[j] -= M.get(j, queue[i]);
            bad_rows[j] = (row_scores[j] <= 1);
        }
    }
}

