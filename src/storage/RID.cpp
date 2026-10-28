#include "RID.hpp"
#include <stdexcept>
RID :: RID(const std::int32_t page_no, std::int32_t row_no)
{
    if(page_no<0|| row_no<0)
    {
        throw  std::invalid_argument("Invalid RID");
    }

    page_id = page_no;
    slot_id = row_no;
}

std::int32_t RID::get_page_id() const
{
    return page_id;

}

std::int32_t RID::get_slot_id() const
{
        return slot_id;
}