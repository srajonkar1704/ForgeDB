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
    if(isfull())
    {
        throw invalid_argument(" PAGE FULL");
    }

    int32_t x = get_row_used();
    int32_t add = x*ROW_SIZE;
    row.serialize(data+METADATA+add);
    
    set_row_used(x+1);
}

 Row Page::read_row(const int32_t& idx) const
{
    int x = get_row_used();
    if(idx<0 || idx>MAX_ROWS-1||idx>=x)
    {
        throw invalid_argument("Invalid Index");
    }


    Row row ;
    int32_t add = idx*ROW_SIZE;
    row = row.deserialize(data+METADATA+add);
    return row;
}

bool Page::isfull() const 
{
    
    int32_t x =get_row_used();
    return (x==MAX_ROWS);
}

int32_t Page:: get_row_used() const 
{
    int32_t x;
    memcpy(&x,data,METADATA);
    return x; 
}

void Page:: set_row_used(const int32_t& row_used)
{
    memcpy(data,&row_used,METADATA);
}


void Page:: serializer(char* buffer) const
{
    memcpy(buffer,data,db::PAGE_SIZE);
    return;
}

void Page::deserializer(const char* buffer)
{
    memcpy(data,buffer,db::PAGE_SIZE);
    return;
}