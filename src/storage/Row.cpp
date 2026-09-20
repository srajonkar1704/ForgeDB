#include<cstring>
#include<string>
#include "Row.hpp"
#include<stdexcept>
using namespace std;
Row::Row() : id(0)
{
    username[0]='\0';
    email[0] = '\0';
}

Row:: Row(const string& Username,int32_t i_d , const string& Email) : id(i_d)
{
    if(Username.length()>= NAME_SIZE)
    {
        throw invalid_argument("Username is too long");
    }

    if(Email.length()>= EMAIL_SIZE)
    {
        throw invalid_argument("Email is too long");
    }

    memset(this->username , 0, NAME_SIZE);
    memset(this->email,0,EMAIL_SIZE);
    memcpy(this->username,Username.c_str(),Username.length()-1);
    memcpy(this->email , Email.c_str(), Email.length()-1);
}

void Row :: serialize(char* destination) const
{
    size_t offset =0;
    memcpy(destination+offset , &id, sizeof(id));
    offset += sizeof(id); 
    memcpy(destination+offset , username, NAME_SIZE);
    offset+= NAME_SIZE;
    memcpy(destination+offset , email, EMAIL_SIZE);
}

Row Row :: deserialize(const char* source)
{
    Row row ;
    size_t offset =0;

    memcpy(&row.id, source+offset, sizeof(row.id));
    offset += sizeof(row.id);

    memcpy(row.username , source+offset , NAME_SIZE);
    offset += NAME_SIZE;

    memcpy(row.email, source+offset , EMAIL_SIZE);
    return row;
}