#pragma once
#include<cstdint>
#include<cstddef>
#include<string>
#include "Constants.hpp"
#include "Row.hpp"



class Page
{
    
    private:
    static constexpr size_t METADATA = 4;
    static constexpr size_t ROW_SIZE = 100;
    static constexpr size_t MAX_ROWS  = (db::PAGE_SIZE - METADATA)/ROW_SIZE ;
    char data[db::PAGE_SIZE];
     int32_t get_row_used() const;

    void set_row_used(const int32_t& row_used);


    public:
    Page();

    void insert_row(const Row& row);

    Row read_row(const int32_t& idx) const;
    
    bool isfull()const;

    void serializer(char* buffer) const ;

    void deserializer(const char* buffer);
};