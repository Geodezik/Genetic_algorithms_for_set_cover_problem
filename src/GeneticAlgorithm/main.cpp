#include "BCGA.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>
#include <random>
#include <numeric>

#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <stdexcept>
#include <sstream>

#include <boost/dynamic_bitset.hpp>

using namespace BCGA;

std::vector<std::pair<int, std::vector<bool>>> read_csv(std::string filename){
    // Reads a CSV file into a vector of <string, vector<int>> pairs where
    // each pair represents <column name, column values>

    std::vector<std::pair<int, std::vector<bool>>> result;

    std::ifstream myFile(filename);

    if(!myFile.is_open()) throw std::runtime_error("Could not open file");

    std::string line, colname;
    int val;

    if(myFile.good())
    {
        std::getline(myFile, line);
        std::stringstream ss(line);

        while(std::getline(ss, colname, ',')){
            int int_colname = std::stoi(colname);
            result.push_back({int_colname, std::vector<bool> {}});
        }
    }

    while(std::getline(myFile, line))
    {
        std::stringstream ss(line);
        int colIdx = 0;

        while(ss >> val){
            result.at(colIdx).second.push_back(val);
            if(ss.peek() == ',') ss.ignore();
            colIdx++;
        }
    }

    myFile.close();

    return result;
}

int main()
{
    int m = 10260;
    int n = 1724;

    int K = n / 10;
    float C = 0.1;
    int population_size = 25;
    int max_iter = 100;
    int seed = 42;

    BooleanMatrix::BooleanMatrix M(m, n);
    std::vector<int> groups_idx = {};
    //std::vector<float> ranks = {10, 5, 0, 0, -10, 0, 0, 0, 0, 0};

    int j = 0;
    int max_group = -1;
    std::vector<std::pair<int, std::vector<bool>>> csv_data = read_csv("./data/titanic_bool.csv");
    for(auto pair: csv_data) {
        int cur_group = pair.first;
        if(cur_group > max_group) {
            max_group = cur_group;
            groups_idx.push_back(j);
        }
        std::vector<bool> column = pair.second;
        for(int i = 0; i < m; i++)
            M.set(i, j, column[i]);
        j++;
    }

    for(int i = 0; i < m; i++) {
        int row_sum = 0;
        for(int k = 0; k < n; k++)
            row_sum += M.get(i, k);
        if(row_sum == 0)
            throw 228;
    }

    //SotnezovBCGA A = SotnezovBCGA(population_size, K, C, max_iter, seed, OutputMode::Normal);
    //REncSotnezovBCGA A = REncSotnezovBCGA(population_size, groups_idx, ranks, RankType::GroupWise, Fitness::MaxBinsNum, K, C, max_iter, seed, OutputMode::Max);
    EncSotnezovBCGA A = EncSotnezovBCGA(population_size, groups_idx, Fitness::MaxBinsNum, K, C, max_iter, seed, OutputMode::Max);
    A.fit(M);
    A.analyze_solution(M);

    return 0;
}