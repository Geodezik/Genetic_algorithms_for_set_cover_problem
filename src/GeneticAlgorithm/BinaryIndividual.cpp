#include "BCGA.hpp"

BCGA::BinaryIndividual::BinaryIndividual(std::vector<bool>& genotype, bool zero_gen)
{
    this->zero_gen = zero_gen;
    this->genotype = genotype;
}

int BCGA::BinaryIndividual::size()
{
    return genotype.size();
}

bool BCGA::BinaryIndividual::is_from_zero_gen()
{
    return zero_gen;
}

std::ostream& BCGA::operator<<(std::ostream& os, const BinaryIndividual& individual)
{
    for(int i = 0; i < individual.genotype.size(); i++) {
        os << individual.genotype[i] << " ";
    }
    return os;
}
