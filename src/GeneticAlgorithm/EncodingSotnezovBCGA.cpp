#include "BCGA.hpp"

BCGA::EncodingSotnezovBCGA::EncodingSotnezovBCGA(int population_size, std::vector<int> groups_idx, Fitness optimize, int K, float C, int max_iter, int seed, OutputMode verbose): BCGA::SotnezovBCGA(
                                                 population_size, K, C, max_iter, seed, verbose)
{
    this->groups_idx = groups_idx;
    this->fit_function = optimize;
};

void BCGA::EncodingSotnezovBCGA::check_compatibility()
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
            throw std::out_of_range("Found wrond index (idx <= 0) or (idx >= n)");
        max_idx = idx;
    }
};

double BCGA::EncodingSotnezovBCGA::covlen_fitness(BooleanMatrix::BooleanMatrix& M, BCGA::BinaryIndividual& individual)
{
    int ones_counter = 0;
    for(int i = 0; i < individual.size(); i++)
        if(individual.genotype[i])
            ones_counter++;
    return ones_counter;
};

double BCGA::EncodingSotnezovBCGA::maxbinsnum_fitness(BooleanMatrix::BooleanMatrix& M, BCGA::BinaryIndividual& individual)
{
    int ones_counter = 0;
    for(int i = 0; i < individual.size(); i++)
        if(individual.genotype[i])
            ones_counter++;
    return ones_counter;
};

double BCGA::EncodingSotnezovBCGA::fitness(BooleanMatrix::BooleanMatrix& M, BCGA::BinaryIndividual& individual)
{
    switch(fit_function) {
    case Fitness::CovLen:
        return covlen_fitness(M, individual);
    case Fitness::MaxBinsNum:
        return maxbinsnum_fitness(M, individual);
    default:
        throw std::invalid_argument("Unknown fitness funtion in fitness call");
    }

    throw std::invalid_argument("Unknown fitness funtion in fitness call");
    return 0;
}
