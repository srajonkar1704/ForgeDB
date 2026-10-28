#pragma once 
#include <cstdint>

class RID
{
    private:
    std::int32_t page_id;
    std::int32_t slot_id;

    public:
    RID(const std::int32_t page_no , const std:: int32_t row_no);
    

    int32_t get_page_id() const;
    int32_t get_slot_id() const;
};