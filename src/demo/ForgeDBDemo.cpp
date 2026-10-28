#include "../src/Query/querypro.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

void wait(int milliseconds)
{
    this_thread::sleep_for(
        chrono::milliseconds(milliseconds)
    );
}

void show_create()
{
    cout << "\n";
    cout << "[ CREATE TABLE ]\n";
    wait(300);

    cout << "        |\n";
    cout << "        v\n";

    cout << "[ HEAP FILE ]\n";
    wait(300);

    cout << "Creating heap file...\n";
    wait(500);

    cout << "        |\n";
    cout << "        v\n";

    cout << "[ PAGER ]\n";
    cout << "Opening database file...\n";
    wait(500);

    cout << "\n";
    cout << "Workflow ready.\n\n";
}

void show_insert()
{
    cout << "\n";
    cout << "[ INSERT ]\n";
    wait(300);

    cout << "        |\n";
    cout << "        v\n";

    cout << "[ QUERY PROCESSOR ]\n";
    cout << "Parsing INSERT query...\n";
    wait(500);

    cout << "        |\n";
    cout << "        v\n";

    cout << "[ HEAP FILE ]\n";
    cout << "Finding page with free space...\n";
    wait(500);

    cout << "        |\n";
    cout << "        v\n";

    cout << "[ PAGE ]\n";
    cout << "Finding available row slot...\n";
    wait(500);

    cout << "        |\n";
    cout << "        v\n";

    cout << "[ ROW ]\n";
    cout << "Preparing row for insertion...\n";
    wait(500);

    cout << "        |\n";
    cout << "        v\n";

    cout << "[ DISK ]\n";
    cout << "Writing page to heap file...\n";
    wait(500);

    cout << "\n";
}

void show_select()
{
    cout << "\n";
    cout << "[ SELECT ]\n";
    wait(300);

    cout << "        |\n";
    cout << "        v\n";

    cout << "[ QUERY PROCESSOR ]\n";
    cout << "Parsing SELECT query...\n";
    wait(500);

    cout << "        |\n";
    cout << "        v\n";

    cout << "[ HEAP FILE ]\n";
    cout << "Starting sequential scan...\n";
    wait(500);

    cout << "        |\n";
    cout << "        v\n";

    cout << "[ PAGE ]\n";
    cout << "Reading pages...\n";
    wait(500);

    cout << "        |\n";
    cout << "        v\n";

    cout << "[ ROW ]\n";
    cout << "Deserializing rows...\n";
    wait(500);

    cout << "\n";
}

int main()
{
    cout << "\n";
    cout << "============================================\n";
    cout << "              ForgeDB Terminal              \n";
    cout << "============================================\n";
    cout << "Type SQL queries or 'exit' to quit.\n\n";

    string query;

    while(true)
    {
        cout << "forge> ";

        getline(cin, query);

        if(query == "exit")
        {
            cout << "\nClosing ForgeDB...\n";
            break;
        }

        if(query.empty())
        {
            continue;
        }

        /*
         * Visualization only.
         * This does NOT modify the storage engine.
         */
        if(query.rfind("CREATE", 0) == 0)
        {
            show_create();
        }
        else if(query.rfind("INSERT", 0) == 0)
        {
            show_insert();
        }
        else if(query.rfind("SELECT", 0) == 0)
        {
            show_select();
        }
        else
        {
            cout << "Unknown command.\n";
        }

        /*
         * REAL QUERY EXECUTION
         *
         * This invokes your existing Query Processor.
         * Therefore the real .heap file is modified/read.
         */
        querypro q(query);

        cout << "\n";
    }

    return 0;
}