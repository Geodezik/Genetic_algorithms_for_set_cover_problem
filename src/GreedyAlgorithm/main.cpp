#include "Greedy.hpp"
#include "BooleanMatrix.hpp"
#include <iostream>

#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <stdexcept>
#include <sstream>

using namespace Greedy;

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
    int m = 40712;
    int n = 1967;

    BooleanMatrix::BooleanMatrix M(m, n);
    std::vector<int> groups_idx = {};

    int j = 0;
    int max_group = -1;
    std::vector<std::pair<int, std::vector<bool>>> csv_data = read_csv("./data/cameras_bool.csv");
    for(auto pair: csv_data) {
        int cur_group = pair.first;
        if(cur_group > max_group)
            max_group = cur_group;
        groups_idx.push_back(max_group);
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

    EncodingGreedyAlgorithm A = EncodingGreedyAlgorithm(groups_idx);
    A.fit(M);
    //A.print_solution(M);
    A.analyze();
    A.print_fit_stats(M);

    return 0;
}