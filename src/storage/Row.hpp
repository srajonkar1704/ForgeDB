#pragma once
#include<cstdint>
#include<cstddef>
#include<string>

class Row
{
    public:
     static constexpr size_t NAME_SIZE =32;
     static constexpr size_t EMAIL_SIZE =64;
     static constexpr size_t ROW_SIZE = NAME_SIZE + sizeof(int32_t) + EMAIL_SIZE;     int32_t id;
     char username[32];
     char email[64];
     
     Row();
     Row(const std:: string& username, int32_t id, const std:: string& email);
     void serialize(char* destination) const;
     static Row deserialize(const char* source);
     
     
    static constexpr size_t SerializedSize()
    {
        return ROW_SIZE;
    }

};