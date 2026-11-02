#include "Query/querypro.hpp"

#include <iostream>
#include <string>

int main()
{
    std::string query;

    // Read one SQL query from standard input.
    if (!std::getline(std::cin, query))
    {
        std::cerr << "No query received.\n";
        return 1;
    }

    if (query.empty())
    {
        std::cerr << "Empty query.\n";
        return 1;
    }

    // Execute using the existing ForgeDB query processor.
    querypro q(query);

    return 0;
}

