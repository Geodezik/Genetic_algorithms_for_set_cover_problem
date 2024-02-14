#include "BooleanMatrix.hpp"
#include <numeric>
#include <utility>
#include <fstream>
#include <stdexcept>
#include <sstream>

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

void fill_matrix(BooleanMatrix::BooleanMatrix &M, std::vector<std::pair<int, std::vector<bool>>> &csv_data, std::vector<int> &groups_idx) {
    int j = 0;
    int max_group = -1;
    int m = M.get_m();
    int n = M.get_n();
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

    // check matrix is correct
    for(int i = 0; i < m; i++) {
        int row_sum = 0;
        for(int k = 0; k < n; k++)
            row_sum += M.get(i, k);
        if(row_sum == 0)
            throw std::runtime_error("Incorrect matrix or csv format");
    }
}

void read_ranks(std::string filename, std::vector<double> &ranks)
{
    std::ifstream myFile(filename);
    if(!myFile.is_open()) throw std::runtime_error("Could not open ranks file");

    std::string line;
    double val;

    std::getline(myFile, line);
    std::stringstream ss(line);
    int colIdx = 0;

    while(ss >> val) {
        ranks.push_back(val);
        if(ss.peek() == ',') ss.ignore();
        colIdx++;
    }

    //for(auto elem: ranks)
        //std::cout << elem << std::endl;

    myFile.close();
}
