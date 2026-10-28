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
    if(Username.length()>= db::NAME_SIZE)
    {
        throw invalid_argument("Username is too long");
    }

    if(Email.length()>= db::EMAIL_SIZE)
    {
        throw invalid_argument("Email is too long");
    }

    memset(this->username , 0, db::NAME_SIZE);
    memset(this->email,0,db::EMAIL_SIZE);
    memcpy(this->username,Username.c_str(),Username.length());
    memcpy(this->email , Email.c_str(), Email.length());
}

void Row :: serialize(char* destination) const
{
    size_t offset =0;
    memcpy(destination+offset , &id, sizeof(id));
    offset += sizeof(id); 
    memcpy(destination+offset , username, db::NAME_SIZE);
    offset+= db::NAME_SIZE;
    memcpy(destination+offset , email, db::EMAIL_SIZE);
}

Row Row :: deserialize(const char* source)
{
    Row row ;
    size_t offset =0;

    memcpy(&row.id, source+offset, sizeof(row.id));
    offset += sizeof(row.id);

    memcpy(row.username , source+offset , db::NAME_SIZE);
    offset += db::NAME_SIZE;

    memcpy(row.email, source+offset , db::EMAIL_SIZE);
    return row;
}