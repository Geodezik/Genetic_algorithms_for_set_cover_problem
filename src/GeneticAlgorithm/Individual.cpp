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
