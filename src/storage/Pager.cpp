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
        file.open(filename,std::ios::in|std::ios::out|std::ios::binary);
    }

    else
    {
        std::ofstream create(filename,std::ios::out|std::ios::binary);
        create.close();

        file.open(filename,std::ios::in|std::ios::out|std::ios::binary);
    }
    
    if(!file.is_open())
    {
        throw std::runtime_error("Failed to open database file");
    }

}

size_t Pager:: get_Pages()  
{

    file.clear();
    std::streampos old_pos = file.tellg();
    if(old_pos== std::streampos(-1))
    {
        throw std::runtime_error("Failed to save read position");
    }
    file.seekg(0,std::ios::end);

    if(!file)
    {
        file.clear();
        file.seekg(old_pos);
        throw std::runtime_error("Failed to seek to file end");
    }
    std::streampos end_pos = file.tellg();

    if(end_pos==std::streampos(-1))
    {
        throw std::runtime_error("Failed to determine file size");
    }

    size_t num = end_pos/db::PAGE_SIZE;
    if(end_pos%db::PAGE_SIZE!=0)
    {throw std::runtime_error("Corrupt file");
    }
    file.clear();
    file.seekg(old_pos);
    if(!file)
    {
        throw std::runtime_error("Failed to go to old read position");
    }
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
        throw std::runtime_error("Invalid Page NUmber");
    }
    char buffer[db::PAGE_SIZE] ={};
    

    page.serialize(buffer);
    file.clear();
    
    file.seekp(static_cast<std::streamoff>(idx) * db::PAGE_SIZE);

    if (!file)
    {
        throw std::runtime_error("Write seek failed");
    }

    
    file.write(buffer,db::PAGE_SIZE);

     
     file.flush();

    if(!file)
    {
        throw std::runtime_error("Write opertaion failed");
    }
    file.clear();
}

Page Pager:: read_Page(int32_t page_No) 
{
    if(!is_CheckvalPage(page_No))
    {throw std::runtime_error("Invalid Page number");}

    file.seekg(static_cast<std::streamoff>(page_No) * db::PAGE_SIZE);

    if (!file)
    {
        throw std::runtime_error("Read seek failed");
    }

    char buffer[db::PAGE_SIZE] = {};
    file.read(buffer,db::PAGE_SIZE);
    
    if(!file ||file.gcount() != db::PAGE_SIZE )
    {
        throw std::runtime_error("Read opertaion failed");
    }
    file.clear();
    Page page;
    page.deserialize(buffer);
    return page;

}

