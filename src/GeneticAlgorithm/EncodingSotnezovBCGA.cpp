#include "BCGA.hpp"

BCGA::EncodingSotnezovBCGA::EncodingSotnezovBCGA(int population_size, std::vector<int> features, Fitness optimize, int K, float C, int max_iter, int seed, OutputMode verbose): BCGA::SotnezovBCGA(
                                                 population_size, K, C, max_iter, seed, verbose)
{
    this->features = features;
    this->fit_function = optimize;
};

void BCGA::EncodingSotnezovBCGA::check_compatibility()
{
    if(features.size() != n)
        throw std::out_of_range("Feature vector is invalid for the given matrix");
};
