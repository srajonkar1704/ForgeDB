#pragma once
#include<cstdint>
#include<cstddef>
#include<string>
#include "Constants.hpp"
#include "Row.hpp"



class Page
{
    
    private:

    static constexpr size_t MAX_ROWS  = (db::PAGE_SIZE - db::PAGE_METADATA__SIZE)/db::ROW_SIZE ;
    char data[db::PAGE_SIZE];
     
    int32_t row_used() const;
    void set_row_used(const int32_t& row_used);

    void valid_row() const;

    public:
    Page();

    void insert_row(const Row& row);

    int32_t row_count() const;

    Row read_row(const int32_t& idx) const;
    
    bool is_full()const;

    void serialize(char* buffer) const ;

    void deserialize(const char* buffer);
};