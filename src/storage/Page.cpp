#include<cstring>
#include<string>
#include<stdexcept>
#include "Page.hpp"

using namespace std;

Page:: Page()
{
    memset(data,0,db::PAGE_SIZE);
}


void Page::insert_row(const Row& row)
{
    if(is_full())
    {
        throw invalid_argument(" PAGE FULL");
    }
    int32_t x = row_used();
    int32_t add = x*db::ROW_SIZE;
    row.serialize(data+db::PAGE_METADATA__SIZE+add);
    
    set_row_used(x+1);
}

 Row Page::read_row(const int32_t& idx) const
{

    valid_row();
    int x = row_used();
    if(idx<0 || idx>MAX_ROWS-1||idx>=x)
    {
        throw invalid_argument("Invalid Index");
    }


    Row row ;
    int32_t add = idx*db::ROW_SIZE;
    row = row.deserialize(data+db::PAGE_METADATA__SIZE+add);
    return row;
}

bool Page::is_full() const 
{
    valid_row();
    int32_t x =row_used();
    return (x==MAX_ROWS);
}

int32_t Page:: row_used() const 
{
    int32_t x;
    memcpy(&x,data,db::PAGE_METADATA__SIZE);
    return x; 
}

void Page:: set_row_used(const int32_t& row_used)
{
    memcpy(data,&row_used,db::PAGE_METADATA__SIZE);
}


void Page:: serialize(char* buffer) const
{
    memcpy(buffer,data,db::PAGE_SIZE);
    return;
}

void Page::deserialize(const char* buffer)
{
    memcpy(data,buffer,db::PAGE_SIZE);
    return;
}

int32_t Page:: row_count() const
{
     valid_row();
    int32_t x =row_used();
    return x;
}

void Page:: valid_row() const
{
    int32_t x = row_used();
    if(x>MAX_ROWS ||x<0)
    {
        throw runtime_error(" METADATA IS CORRUPTED");
    }
}