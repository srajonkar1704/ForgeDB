#include <iostream>
#include <vector>
#include <string>

using namespace std;

void value_parse(
    const vector<string>& values,
    vector<string>& input
)
{
    bool inside_quotes = false;
    string value;

    for (const string& token : values)
    {
        for (char ch : token)
        {
            if (ch == '"')
            {
                inside_quotes = !inside_quotes;
            }
            else if (ch == ',' && !inside_quotes)
            {
                input.push_back(value);
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

    if (!value.empty())
    {
        input.push_back(value);
    }
}

int main()
{
    vector<string> values = {
        "(1,",
        "\"Sambhav\",",
        "\"abc@gmail.com\");"
    };

    vector<string> input;

    value_parse(values, input);

    for (const string& value : input)
    {
        cout << "[" << value << "]" << endl;
    }

    return 0;
}