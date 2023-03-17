#include "Genetic.hpp"

Genetic::Individual::Individual(std::vector<bool>& genotype, bool zero_gen)
{
    this->zero_gen = zero_gen;
    this->genotype = genotype;
}

int Genetic::Individual::size()
{
    return genotype.size();
}

bool Genetic::Individual::is_coverage(BooleanMatrix::BooleanMatrix& M)
{
    int m = M.get_m();
    int n = M.get_n();

    for(int i = 0; i < m; i++) {
        bool flag = false;
        for(int j = 0; j < n; j++) {
            if(!genotype[j])
                continue;
            if(M[i][j]) {
                flag = true;
                break;
            }
        }
        if(!flag)
            return false;
    }

    return true;
}

bool Genetic::Individual::is_from_zero_gen()
{
    return zero_gen;
}

std::ostream& Genetic::operator<<(std::ostream& os, const Individual& I)
{
    for(int i = 0; i < I.genotype.size(); i++) {
        os << I.genotype[i] << " ";
    }
    return os;
}
