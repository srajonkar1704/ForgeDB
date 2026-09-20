#pragma once 
#include<cstdint>
#include<cstddef>
#include<string>
#include<fstream>
#include "Page.hpp"
#include "Constants.hpp"

class Pager
{
    private:
    std:: fstream file;

    bool is_CheckvalPage(const int32_t pageNO) ;

    size_t get_Pages() ;

    public:
    Pager(const std::string& filename);

    Page read_Page(int32_t page_No ) ;

    void write_Page(const int32_t idx , const Page& page);
};