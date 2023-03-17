#include "Genetic.hpp"

Genetic::BaseIndividual::BaseIndividual(std::vector<bool>& genotype, bool zero_gen, double p)
{
    this->zero_gen = zero_gen;
    this->genotype = genotype;
}

int Genetic::BaseIndividual::size()
{
    return genotype.size();
}

bool Genetic::BaseIndividual::is_coverage(BooleanMatrix::BooleanMatrix& M)
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

bool Genetic::BaseIndividual::is_from_zero_gen()
{
    return zero_gen;
}

double Genetic::BaseIndividual::fitness(BooleanMatrix::BooleanMatrix& M)
{
    int m = M.get_m();
    int n = M.get_n();

    if(zero_gen)
        return 0;

    for(int i = 0; i < m; i++) {
        bool flag = false;
        for(int j = 0; j < n; j++) {
            // gene in set and elem in M is 1
            if(genotype[j] && M[i][j]) {
                flag = true;
                break;
            }
        }
        if(!flag)
            return n + 1;
    }

    // cheap???
    int ones_counter = 0;
    for(int i = 0; i < size(); i++) {
        if(genotype[i])
            ones_counter++;
    }

    return ones_counter;
}

std::ostream& Genetic::operator<<(std::ostream& os, const BaseIndividual& I)
{
    for(int i = 0; i < I.genotype.size(); i++) {
        os << I.genotype[i] << " ";
    }
    return os;
};

Genetic::CoverageIndividual::CoverageIndividual(std::vector<bool>& genotype, bool first_gen, double p): BaseIndividual(genotype, first_gen, p) {}
