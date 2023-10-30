#include "Greedy.hpp"

int Greedy::GreedyAlgorithm::get_column_score(BooleanMatrix::BooleanMatrix& M, int j, std::vector<bool> &row_is_covered)
{
    int m = M.get_m();
    int n = M.get_n();
    int score = 0;

    for(int i = 0; i < m; i++) {
        if(M[i][j] && !row_is_covered[i])
            score++;
    }

    return score;
}

int Greedy::GreedyAlgorithm::get_argmax_score(std::vector<int> &scores, int n)
{
    int max_score = 0;
    int argmax_score = 0;
    for(int i = 0; i < n; i++) {
        if(scores[i] > max_score) {
            max_score = scores[i];
            argmax_score = i;
        }
    }

    return argmax_score;
}

void Greedy::GreedyAlgorithm::update(BooleanMatrix::BooleanMatrix& M, int j, std::vector<bool> &row_is_covered, int &not_covered_counter)
{
    int m = M.get_m();
    for(int i = 0; i < m; i++)
        if(M[i][j] and !row_is_covered[i]) {
            row_is_covered[i] = true;
            not_covered_counter--;
        }
}

void Greedy::GreedyAlgorithm::fit(BooleanMatrix::BooleanMatrix& M)
{
    std::time_t start = std::time(nullptr);
    int m = M.get_m();
    int n = M.get_n();
    columns = std::vector<bool>(n);
    std::vector<bool> row_is_covered(m);
    std::vector<int> scores(n);
    int not_covered_counter = m;

    while(not_covered_counter) {
        for(int j = 0; j < n; j++)
            scores[j] = get_column_score(M, j, row_is_covered);
        int argmax_score = get_argmax_score(scores, n);
        update(M, argmax_score, row_is_covered, not_covered_counter);
        columns[argmax_score] = true;
    }
    std::time_t finish = std::time(nullptr);
    fit_time = finish - start;
}

void Greedy::GreedyAlgorithm::print_solution(BooleanMatrix::BooleanMatrix& M)
{
    int m = M.get_m();
    int n = M.get_n();
    if(n > 100)
        std::cout << "WARNING: Solution output can be too huge." << std::endl;

    std::cout << "Coverage: " << std::endl;
    std::cout << columns.size() << std::endl;
    for(auto elem: columns)
        std::cout << elem << " ";
    std::cout << std::endl;
}

void Greedy::GreedyAlgorithm::analyze()
{
    std::cout << "Analyzing..." << std::endl;

    int s = 0;
    for(auto elem: columns)
        s += elem;
    std::cout << "Covering length: " << s << std::endl;
}

void Greedy::GreedyAlgorithm::print_fit_stats(BooleanMatrix::BooleanMatrix& M, std::string filename)
{
  std::ofstream f;
  f.open(filename, std::ofstream::app);
  f << fit_time << " " << columns.size() << '\n';
  f.close();
}

Greedy::EncodingGreedyAlgorithm::EncodingGreedyAlgorithm(std::vector<int> features)
{
    this->features = features;
    clear_counters();
}

void Greedy::EncodingGreedyAlgorithm::clear_counters()
{
    for(auto elem: features)
        group_counters[elem] = 0;
}

template <typename T>
std::vector<int> Greedy::EncodingGreedyAlgorithm::argsort(const std::vector<T> &v) {
    //Argsort for any type of elem-comparable vectors
    std::vector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::stable_sort(idx.begin(), idx.end(),
        [&v](int i1, int i2) {return v[i1] < v[i2];});

    return idx;
}

void Greedy::EncodingGreedyAlgorithm::optimize_covering(BooleanMatrix::BooleanMatrix& M)
{
    int m = M.get_m();
    int n = M.get_n();

    std::vector<int> row_scores(m);
    std::vector<int> column_scores(n);
    for(int i = 0; i < m; i++)
        for(int j = 0; j < n; j++)
            if(M[i][j] & columns[j]) {
                row_scores[i]++;
                column_scores[j]++;
            }

    std::vector<int> queue = argsort(column_scores);
    for(auto col: queue) {
        //unique for group => can't delete
        if(group_counters[features[col]] == 1)
            continue;

        //zero rows covered or not in covering
        if(!column_scores[col]) {
            if(columns[col]) {
                columns[col] = false;
                group_counters[features[col]]--;
            }
            continue;
        }

        bool flag = true;
        for(int j = 0; j < m; j++)
            if(M[j][col] && (row_scores[j] < 2)) {
                flag = false;
                break;
            }

        if(!flag)
            continue;

        columns[col] = false;
        group_counters[features[col]]--;
        for(int j = 0; j < m; j++) {
            // Decrease scores
            if(M[j][col])
                    row_scores[j]--;
        }
    }
}

void Greedy::EncodingGreedyAlgorithm::fit(BooleanMatrix::BooleanMatrix& M)
{
    std::time_t start = std::time(nullptr);
    int m = M.get_m();
    int n = M.get_n();

    if(features.size() != n)
        throw std::out_of_range("Feature vector is invalid for the given matrix");

    clear_counters();

    columns = std::vector<bool>(n);
    std::vector<bool> row_is_covered(m);
    std::vector<int> scores(n);
    int not_covered_counter = m;

    while(not_covered_counter) {
        for(int j = 0; j < n; j++)
            scores[j] = get_column_score(M, j, row_is_covered);
        int argmax_score = get_argmax_score(scores, n);
        update(M, argmax_score, row_is_covered, not_covered_counter);
        columns[argmax_score] = true;
        group_counters[features[argmax_score]]++;
    }

    // add columns to get encoding, then find irreducible
    for(auto it = group_counters.begin(); it != group_counters.end(); it++) {
        if(it->second)
            continue;
        for(int j = 0; j < n; j++)
            if(features[j] == it->first) {
                columns[j] = true;
                group_counters[it->first]++;
                break;
            }
    }
    optimize_covering(M);

    std::time_t finish = std::time(nullptr);
    fit_time = finish - start;
}

void Greedy::EncodingGreedyAlgorithm::analyze()
{
    std::cout << "Analyzing..." << std::endl;

    int s = 0;
    for(auto elem: columns)
        s += elem;
    std::cout << "Covering length: " << s << std::endl;

    int enc_rank = 0;
    for(auto it = group_counters.begin(); it != group_counters.end(); it++)
        if(it->second > enc_rank)
            enc_rank = it->second;
    std::cout << "Encoding rank: " << enc_rank << std::endl;
}
