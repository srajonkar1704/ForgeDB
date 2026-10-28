#include "querypro.hpp"
#include <sstream>
#include<iostream>
#include <filesystem>

querypro::querypro(const std::string& query)
{
    tokenize(query);
}

void querypro::tokenize(const std::string& query)
{
    std::vector<std::string> tokens;
    std::istringstream iss(query);
    std::string token;
    while(iss>>token)
    {
        tokens.push_back(token);
    }

    if(tokens.empty())
    {
        std::cout<<"Empty Query\n";
        return;
    }
    if(tokens[0]=="INSERT")
    {
        Insert(tokens);
    }

    else if(tokens[0]=="SELECT")
    {
        Select(tokens);
    }


    else if(tokens[0]=="CREATE")
    {
        Create(tokens);
    }

    else
    {
        std::cout<<"Unknown Commands\n";
    }
}


void querypro::Insert(std::vector<std::string> tokens)
{
    std::vector<std::string> values(tokens.begin()+4,tokens.end());
    std::string table_name = tokens[2];
    std::vector<std::string> ipt;
    HeapFile heap(table_name+".heap");
    value_parse(values,ipt);

   int id = stoi(ipt[0]);
   Row row(ipt[1],id,ipt[2]);
   std::cout << "ID: " << id << '\n';
std::cout << "Username: " << ipt[1] << '\n';
std::cout << "Email: " << ipt[2] << '\n';


   heap.insert(row);
}

void querypro::value_parse(const std::vector<std::string>& values,std::vector<std::string>& ipt)
{
    bool inside_quotes = false;
    std:: string value;

    for(const std::string& token : values)
    {
        for(char ch: token)
        {
            if(ch=='"')
            {
                inside_quotes =!inside_quotes;
            }

            else if(ch==','&&!inside_quotes)
            {
                ipt.push_back(value);
                value.clear();
            }

             else if ((ch == '(' || ch == ')' || ch == ';') && !inside_quotes)
            {
                // SQL syntax — ignore it
            }
            
            else
            {
                value.push_back(ch);
            }
        }
    }
    if(!value.empty())
    {
        ipt.push_back(value);
    }
}

void querypro::Select(std::vector<std::string> tokens)
{
    if(tokens.size() < 4)
    {
        std::cout << "Invalid SELECT command\n";
        return;
    }

    std::string table_name = tokens[3];

    // Remove ';'
    if(!table_name.empty() && table_name.back() == ';')
    {
        table_name.pop_back();
    }

    std::string filename = table_name + ".heap";

    // Don't let SELECT accidentally create a new heap file
    if(!std::filesystem::exists(filename))
    {
        std::cout << "Error: Table does not exist\n";
        return;
    }

    HeapFile heap(filename);

    std::vector<Row> rows = heap.scan();

    std::cout << "\n";
    std::cout << "ID\tUsername\tEmail\n";
    std::cout << "---------------------------------------------\n";

    for(const Row& row : rows)
    {
        std::cout << row.id << "\t"
             << row.username << "\t"
             << row.email << "\n";
    }
}

void querypro::Create(std::vector<std::string> tokens)
{
    if(tokens.size() < 3)
    {
        std::cout << "Invalid CREATE command\n";
        return;
    }

    std::string table_name = tokens[2];

    // Remove ';' from table name
    if(!table_name.empty() && table_name.back() == ';')
    {
        table_name.pop_back();
    }

    std::string filename = table_name + ".heap";

    // Don't allow duplicate table names
    if(std::filesystem::exists(filename))
    {
        std::cout<< "Error: Table already exists\n";
        return;
    }

    // HeapFile -> Pager creates the physical file
    HeapFile heap(filename);

    std::cout<< "Table " << table_name << " created successfully\n";
}