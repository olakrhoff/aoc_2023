#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0
#define PART_1 false

string trim(string line)
{
    while (line.at(0) == ' ')
        line = line.substr(1);
    while (line.at(line.size() - 1) == ' ')
        line = line.substr(0, line.size() - 1);
    
    return line;
}

vector<string> split(const string &line, const char delimiter)
{
    vector<string> res {};
    
    string temp = line;
    while (temp.find(delimiter) != -1)
    {
        res.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + 1);
        temp = trim(temp);
    }
    res.emplace_back(temp);
    
    return res;
}

vector<uint64_t> get_numbers_from_list(const string &line)
{
    vector<uint64_t> nums {};
    
    vector<string> string_nums = split(line, ' ');
    
    for (auto s : string_nums)
        nums.emplace_back(stoll(s));
    
    return nums;
}

typedef struct race
{
    uint64_t time;
    uint64_t distance;
    
    race(uint64_t time, uint64_t distance) : time(time), distance(distance)
    {}
    
} race_t;

uint64_t get_distance(uint64_t hold_time, const race_t &race)
{
    if (hold_time >= race.time || hold_time == 0)
        return 0;
    
    return (race.time - hold_time) * hold_time;
}

uint64_t get_number_of_wins(const race_t &race)
{
    uint64_t shortest_hold {}, longest_hold {};
    
    while (get_distance(shortest_hold, race) <= race.distance)
        shortest_hold++;
    longest_hold = shortest_hold;
    while(get_distance(longest_hold + 1, race) > race.distance)
        longest_hold++;
    
    return longest_hold - shortest_hold + 1;
}

uint64_t find_number_of_wins(const vector<race_t> &races)
{
    uint64_t prod {1};
    
    for (auto race : races)
        prod *= get_number_of_wins(race);
    
    return prod;
}

uint64_t decimals_in(uint64_t num)
{
    string temp = to_string(num);
    return temp.size();
}

uint64_t concat_numbers(const vector<uint64_t> &nums)
{
    uint64_t number {};
    uint64_t decimals {};
    
    for (int i = nums.size() - 1; i >= 0; --i)
    {
        number += (nums.at(i) * (uint64_t)pow(10, decimals));
        decimals += decimals_in(nums.at(i));
    }
    
    return number;
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
    vector<uint64_t> times {};
    vector<uint64_t> distances {};
    while (!file.eof())
    {
        getline(file, temp);
        times = get_numbers_from_list(trim(temp.substr(temp.find(':') + 1)));
        getline(file, temp);
        distances = get_numbers_from_list(trim(temp.substr(temp.find(':') + 1)));
    }
    
    file.close();
    
    vector<race_t> races {};
    for (int i = 0; i < times.size(); ++i)
        races.emplace_back(race_t(times.at(i), distances.at(i)));
    
    // Part 1
    if (PART_1)
        cout << "The product of number of ways to win is: " << find_number_of_wins(races) << endl;
    // Part 2
    else
    {
        uint64_t collective_time = concat_numbers(times);
        uint64_t collective_distance = concat_numbers(distances);
        race_t the_race(collective_time, collective_distance);
        cout << "The number of ways to win is: " << get_number_of_wins(the_race) << endl;
    }
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
