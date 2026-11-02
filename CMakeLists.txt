cmake_minimum_required(VERSION 3.16)

project(mini_rdbms
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Shared storage engine and query processor source files
set(FORGEDB_CORE_SOURCES
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
    src/storage/RID.cpp
    src/storage/RID.hpp
)

# Existing terminal demo
add_executable(mini_rdbms
    ${FORGEDB_CORE_SOURCES}
    src/demo/ForgeDBDemo.cpp
)

# Backend CLI — one query per process
add_executable(forgedb_cli
    ${FORGEDB_CORE_SOURCES}
    src/cli.cpp
)

# Include paths for both executables
target_include_directories(mini_rdbms PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(forgedb_cli PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)