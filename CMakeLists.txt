cmake_minimum_required(VERSION 3.16)

project(mini_rdbms
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(mini_rdbms
    
    src/storage/Constants.hpp
    src/storage/Row.hpp
    src/storage/Row.cpp
    src/storage/Page.hpp
    src/storage/Page.cpp
    src/storage/Pager.hpp
    src/storage/Pager.cpp
    src/storage/HeapFile.hpp
    src/storage/HeapFile.cpp
    src/Query/querypro.hpp
    src/Query/querypro.cpp
    src/demo/ForgeDBDemo.cpp  
)

target_include_directories(mini_rdbms PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

