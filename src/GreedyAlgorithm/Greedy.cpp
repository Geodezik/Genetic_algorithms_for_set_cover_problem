#include "Greedy.hpp"

Greedy::GreedyAlgorithm::GreedyAlgorithm()
{
    this->columns = std::vector<int>();
}

int Greedy::GreedyAlgorithm::get_column_score(BooleanMatrix::BooleanMatrix& M, int j)
{
    int m = M.get_m();
    int n = M.get_n();
    int score = 0;

    for(int i = 0; i < m; i++) {
        if M[i][j]
            score++;
    }

    return score;
}

int Greedy::GreedyAlgorithm::get_argmax_score(int *scores, int n)
{
    int max_score = 0;
    int argmax_score = 0;
    for(int i = 0; i < n; i++) {
        if(scores[i] > max_score) {
            max_scores = scores[i];
            argmax_score = i;
        }
    }

    return argmax_score;
}

void Greedy::GreedyAlgorithm::fit(BooleanMatrix::BooleanMatrix& M)
{
    int m = M.get_m();
    int n = M.get_n();

    bool row_is_covered[m] = {};
    int scores[n] = {};
    int not_covered_counter = m;

    while(!not_covered_counter) {
        for(int j = 0; j < n; j++)
            scores[j] = get_column_score(M, j);
        argmax_score = get_argmax_score(scores, n);
    }
}