#pragma once 
#include<string>
#include<vector>
#include "Constants.hpp"
#include "Row.hpp"
#include "Pager.hpp"
#include "RID.hpp"

class HeapFile
{
    private:
    Pager pager;

    public:
     explicit HeapFile(const std:: string& filename);
    RID insert(const Row& row);
    std::vector<Row> scan(); 
    Row get(const RID& rid) ;
};