#include "BCGA.hpp"

BCGA::EncodingSotnezovBCGA::EncodingSotnezovBCGA(int population_size, std::vector<int> features, Fitness optimize, int K, float C, int max_iter, int seed, OutputMode verbose): BCGA::SotnezovBCGA(
                                                 population_size, K, C, max_iter, seed, verbose)
{

};
