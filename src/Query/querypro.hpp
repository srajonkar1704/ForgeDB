#pragma once 
#include "storage/HeapFile.hpp"
#include<string>
#include<vector>

class querypro
{
    public:
    explicit querypro(const std:: string& query);

    private:
    void Insert(std::vector<std:: string> tokens);

    void Create(std::vector<std::string> tokens);

    void Select(std::vector<std:: string> tokens);

    void tokenize(const std::string& tokens);

    void value_parse(const std::vector<std::string>& values, std::vector<std::string>& ipt);
};