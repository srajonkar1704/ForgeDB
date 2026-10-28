#pragma once
#include<cstdint>
#include<cstddef>
#include<string>
#include "Constants.hpp"

class Row
{
    public:
    int32_t id;
     char username[db::NAME_SIZE];
     char email[db::EMAIL_SIZE];
     
     Row();
     Row(const std:: string& username, int32_t id, const std:: string& email);
     void serialize(char* destination) const;
     static Row deserialize(const char* source);
     
     
    static constexpr size_t SerializedSize()
    {
        return db::ROW_SIZE;
    }

};