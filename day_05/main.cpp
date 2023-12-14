#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

using namespace std;

#define DEBUG 0
#define PART_1 false

string &trim(string &line)
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
        trim(temp);
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

typedef struct
{
    uint64_t dest {};
    uint64_t src {};
    uint64_t range {};
} map_t;

map_t parse_line_to_map(const string &line)
{
    map_t map {};
    
    vector<uint64_t> nums = get_numbers_from_list(line);
    
    if (nums.size() != 3)
    {
        cout << "This list of numbers is expected to be exactly 3" << endl;
        exit(1);
    }
    
    map.dest = nums.at(0);
    map.src = nums.at(1);
    map.range = nums.at(2);
    
    return map;
}

uint64_t map_location(uint64_t seed, vector<map_t> &map)
{
    for (auto m : map)
    {
        if (seed >= m.src && seed <= (m.src + m.range - 1))
        {
            return m.dest + seed - m.src;
        }
    }
    
    return seed;
}

uint64_t find_location(uint64_t seed, vector<vector<map_t>> &maps)
{
    for (auto &m : maps)
        seed = map_location(seed, m);
    
    return seed;
}

uint64_t find_min_location(const vector<uint64_t> &seeds, vector<vector<map_t>> &maps)
{
    vector<uint64_t> locations {};
    
    for (auto s : seeds)
        locations.emplace_back(find_location(s, maps));
    
    return *std::min_element(locations.begin(), locations.end());
}

uint64_t find_offset(uint64_t left, uint64_t right, vector<vector<map_t>> &maps)
{
    uint64_t prev = find_location(left, maps);
    uint64_t start = left;
    uint64_t mid = ((right) - left) / 2 + left;
    
    while (left + 1 < right)
    {
        mid = ((right) - left) / 2 + left;
        if (find_location(mid, maps) == prev + (mid - start))
            left = mid;
        else
            right = mid;
    }
    if (left == mid)
        return mid - start + 1;
    return mid - start;
}

uint64_t find_min_location_interval(const vector<uint64_t> &seeds, vector<vector<map_t>> &maps)
{
    vector<uint64_t> locations {};
    
    for (int i = 0; i < seeds.size(); i += 2)
    {
        uint64_t start = seeds.at(i);
        uint64_t range = seeds.at(i + 1);
        for (uint64_t s = start; s < start + range;)
        {
            locations.emplace_back(find_location(s, maps));
            
            // Calculating the offset
            // We know that there are some range of values that will map to the same value
            // We assume with intuition and no proof that these intervals give unique values
            uint64_t left = s;
            uint64_t right = start + range - 1;
            
            s += find_offset(left, right, maps);
        }
    }
    
    return *std::min_element(locations.begin(), locations.end());
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
    vector<uint64_t> seeds {};
    vector<vector<map_t>> maps {};
    vector<map_t> temp_map {};
    bool found_seeds {false};
    while (!file.eof())
    {
        getline(file, temp);
        if (!found_seeds)
        {
            seeds = get_numbers_from_list(temp.substr(7));
            found_seeds = true;
        }
        else
        {
            if (temp.empty())
            {
                getline(file, temp);
                if (temp_map.empty())
                    continue;
                else
                {
                    maps.emplace_back(temp_map);
                    temp_map = {};
                }
            }
            else
            {
                temp_map.emplace_back(parse_line_to_map(temp));
            }
        }
    }
    maps.emplace_back(temp_map);
    
    file.close();
    
    // Part 1
    if (PART_1)
        cout << "The closest location is: " << find_min_location(seeds, maps) << endl;
    // Part 2
    else
        cout << "The closest location is: " << find_min_location_interval(seeds, maps) << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
