#pragma once
#include<cstddef>
 namespace db
 {
    constexpr std::size_t PAGE_SIZE = 4096;
    constexpr std::size_t ROW_SIZE= 100;
    constexpr std::size_t EMAIL_SIZE= 64;
    constexpr std::size_t NAME_SIZE= 32;
    constexpr std::size_t PAGE_METADATA__SIZE= 4;
 }