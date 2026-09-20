#include <cstring>
#include <string>
#include<iostream>
#include<filesystem>
#include <stdexcept>
#include "Pager.hpp"

Pager::Pager(const std:: string& filename)
{
    //Deals with opening database file in all the modes 
    if(std::filesystem::exists(filename))
    {
        file.open(filename,ios::in|ios::out|ios::binary);
    }

    else
    {
        std::ofstream create(filename,ios::out|ios::binary);
        create.close();

        file.open(filename,ios::in|ios::out|ios::binary);
    }
    
    if(!file.is_open())
    {
        throw std::runtime_error("Failed to open database file");
    }

}

size_t Pager:: get_Pages()  
{
    std::streampos old_pos = file.tellg();
    file.seekg(0,std::ios::end);
    std::streampos end_pos = file.tellg();
    size_t num = end_pos/db::PAGE_SIZE;
    if(end_pos%db::PAGE_SIZE!=0)
    {throw runtime_error("Corrupt file");
    }
    file.clear();
    file.seekg(old_pos);
    return num;
}

bool Pager::is_CheckvalPage(const int32_t  pageNo) 
{
    size_t pNO = get_Pages();
    if(pNO<=pageNo||pageNo<0)
    {return false;}


    return true;
}


void Pager:: write_Page(const int32_t idx, const Page& page)
{
    if(idx>get_Pages()||idx<0)
    {
        throw runtime_error("Invalid Page NUmber");
    }
    char buffer[db::PAGE_SIZE] ={};
    

    page.serializer(buffer);
    file.clear();
    
    file.seekp(static_cast<std::streamoff>(idx)*db::PAGE_SIZE);

    
    file.write(buffer,db::PAGE_SIZE);

     
     file.flush();

    if(!file)
    {
        throw runtime_error("Write opertaion failed");
    }
    file.clear();
}

Page Pager:: read_Page(int32_t page_No) 
{
    if(!is_CheckvalPage(page_No))
    {throw runtime_error("Invalid Page number");}

    file.seekg(page_No*db::PAGE_SIZE);
    char buffer[db::PAGE_SIZE] = {};
    file.read(buffer,db::PAGE_SIZE);
    
    if(!file)
    {
        throw runtime_error("Read opertaion failed");
    }
    file.clear();
    Page page;
    page.deserializer(buffer);
    return page;

}

