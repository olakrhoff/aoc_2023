#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0
#define PART_1 false

uint64_t parse_number(const string& str)
{
    uint64_t first, last;
    
    for (auto c : str)
        if (c >= '0' && c <= '9')
        {
            first = c - '0';
            break;
        }
    
    for (int i = str.size() - 1; i >= 0; --i)
    {
        char c = str[i];
        if (c >= '0' && c <= '9')
        {
            last = c - '0';
            return first * 10 + last;
        }
    }
    
    cout << "Should not get here, all lines should have two numbers in them" << endl;
    exit(2);
}

bool starts_with(const string &str, const string &keyword)
{
    if (str.length() < keyword.length())
        return false;
    
    for (int i = 0 ; i < keyword.size(); ++i)
        if (str[i] != keyword[i])
            return false;
    
    return true;
}

string reverse_string(const string &str)
{
    string reverse = str;
    for (int i = 0; i < str.size(); ++i)
    {
        reverse[reverse.size() - 1 - i] = str[i];
    }
    
    return reverse;
}

#define NUMS 9
const string NUMBERS[NUMS] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

uint64_t parse_number_and_text(const string &str)
{
    uint64_t first, last;
    string temp = str;
    while (!temp.empty())
    {
        char c = temp[0];
        if (c >= '0' && c <= '9')
        {
            first = c - '0';
            break;
        }
        else
        {
            for (int i = 0; i < NUMS; ++i)
            {
                if (starts_with(temp, NUMBERS[i]))
                {
                    first = i + 1;
                    goto phase_two; // Easier to explicitly exit the nested loops, then changing the condition to force an exit
                }
            }
        }
        temp = {temp.begin() + 1, temp.end()};
    }
    
    phase_two:
    while (!temp.empty())
    {
        char c = temp[temp.size() - 1];
        if (c >= '0' && c <= '9')
        {
            last = c - '0';
            return first * 10 + last;
        }
        else
        {
            for (int i = 0; i < NUMS; ++i)
            {
                if (starts_with(reverse_string(temp), reverse_string(NUMBERS[i])))
                {
                    last = i + 1;
                    return first * 10 + last;
                }
            }
        }
        temp = {temp.begin(), temp.end() - 1};
    }
}

int main()
{
    auto start_time = chrono::high_resolution_clock::now();
    //Reading data from file
    string filename = "../data.txt";
    if (DEBUG)
        filename = "../test_data.txt";
    ifstream file;
    
    file.open(filename);
    if (!file.is_open())
        return EXIT_FAILURE;
    
    string temp {};
    uint64_t sum {0};
    while (!file.eof())
    {
        getline(file, temp);
        if (PART_1)
            sum += parse_number(temp);
        else
            sum += parse_number_and_text(temp);
    }
    
    file.close();
    
    cout << "Sum of calibration values: " << sum << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
