#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0
#define PART_1 false

vector<uint64_t> get_numbers(const string &prev, const string &curr, const string &next)
{
    vector<uint64_t> numbers {};
    
    for (int i = 0; i < curr.size(); ++i)
    {
        if (curr.at(i) >= '0' && curr.at(i) <= '9')
        {
            int end_of_number = i;
            while (end_of_number < curr.size() && curr.at(end_of_number + 1) >= '0' && curr.at(end_of_number + 1) <= '9')
            {
                end_of_number++;
            }
            bool symbol_found = false;
            //Check if there is a symbol above or below
            for (int idx = i - 1; idx <= end_of_number + 1; ++idx)
            {
                if (prev.at(idx) != '.' || next.at(idx) != '.')
                {
                    symbol_found = true;
                    break;
                }
            }
            // The number must be added to the list
            if (symbol_found || curr.at(i - 1) != '.' || curr.at(end_of_number + 1) != '.')
            {
                uint64_t number = stoi(curr.substr(i, end_of_number - i + 1));
                numbers.emplace_back(number);
            }
            i = end_of_number;
        }
    }
    
    return numbers;
}

bool is_digit(const char c)
{
    return c >= '0' && c <= '9';
}

uint64_t get_number(const string &line, int index)
{
    uint64_t number {};
    int first_index = index;
    while (is_digit(line.at(first_index - 1)))
        first_index--;
    
    int last_index = index;
    while (is_digit(line.at(last_index + 1)))
        last_index++;
    
    return stoi(line.substr(first_index, last_index - first_index + 1));
}

vector<uint64_t> get_gear_ratios(const string &prev, const string &curr, const string &next)
{
    vector<uint64_t> gear_ratios {};
    for (int i = 1; i < curr.size() - 1; ++i)
    {
        if (curr.at(i) == '*') //We found a gear
        {
            uint64_t first_number {0}, second_number {0};
            int surrounding_numbers {0};
            
            // Check to the left
            if (is_digit(curr.at(i - 1)))
            {
                surrounding_numbers++;
                if (first_number == 0)
                {
                    first_number = get_number(curr, i - 1);
                }
                else
                {
                    second_number = get_number(curr, i - 1);
                }
            }
            // Check to the right
            if (is_digit(curr.at(i + 1)))
            {
                surrounding_numbers++;
                if (first_number == 0)
                {
                    first_number = get_number(curr, i + 1);
                }
                else
                {
                    second_number = get_number(curr, i + 1);
                }
            }
            //Check above
            bool same_number = false;
            for (int idx = i - 1; idx <= i + 1; ++idx)
            {
                if (is_digit(prev.at(idx)) && !same_number)
                {
                    surrounding_numbers++;
                    same_number = true;
                    if (first_number == 0)
                    {
                        first_number = get_number(prev, idx);
                    }
                    else
                    {
                        second_number = get_number(prev, idx);
                    }
                }
                else if (!is_digit(prev.at(idx)))
                {
                    same_number = false;
                }
            }
            //Check below
            same_number = false;
            for (int idx = i - 1; idx <= i + 1; ++idx)
            {
                if (is_digit(next.at(idx)) && !same_number)
                {
                    surrounding_numbers++;
                    same_number = true;
                    if (first_number == 0)
                    {
                        first_number = get_number(next, idx);
                    }
                    else
                    {
                        second_number = get_number(next, idx);
                    }
                }
                else if (!is_digit(next.at(idx)))
                {
                    same_number = false;
                }
            }
            
            if (surrounding_numbers == 2)
            {
                gear_ratios.emplace_back(first_number * second_number);
            }
        }
    }
    
    return gear_ratios;
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
    vector<string> lines {};
    while (!file.eof())
    {
        getline(file, temp);
        lines.emplace_back(temp);
    }
    
    file.close();
    
    // Part 1
    
    //Add a '.' to the start and end of each line to pad the ends so we avoid the edge cases of our of bounds.
    for (int i = 0; i < lines.size(); ++i)
    {
        lines.at(i) = "." + lines.at(i) + ".";
    }
    
    string empty_line = "";
    for (int i = 0; i < lines.at(0).size(); ++i)
        empty_line += ".";
    // We want to add an empty line to the top and bottom of the manual such that we don't have
    // to handle these edge cases, and can just run a generic check for each actual line
    lines.insert(lines.begin(), empty_line);
    lines.emplace_back(empty_line);
    
    uint64_t sum {};
    if (PART_1)
    {
        for (int line = 1; line < lines.size() - 1; ++line)
        {
            vector<uint64_t> numbers_in_line = get_numbers(lines.at(line - 1), lines.at(line), lines.at(line + 1));
            for (auto num: numbers_in_line)
                sum += num;
        }
        cout << "The sum of the part numbers is: " << sum << endl;
    }
    // Part 2
    else
    {
        for (int line = 1; line < lines.size() - 1; ++line)
        {
            vector<uint64_t> gear_ratios = get_gear_ratios(lines.at(line - 1), lines.at(line), lines.at(line + 1));
            for (auto num : gear_ratios)
                sum += num;
        }
        cout << "The sum of the gear ratios are: " << sum << endl;
    }
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
