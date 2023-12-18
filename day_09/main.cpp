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

vector<int64_t> parse_line_to_history(const string &line)
{
    vector<int64_t> nums {};
    
    vector<string> string_nums = split(line, ' ');
    
    for (auto s : string_nums)
        nums.emplace_back(stoll(s));
    
    return nums;
}

vector<int64_t> generate_differences(const vector<int64_t> &history)
{
    vector<int64_t> diffs {};
    
    for (int i = 0; i < history.size() - 1; ++i)
        diffs.emplace_back(history.at(i + 1) - history.at(i));
    
    return diffs;
}

int64_t find_predicted_point(const vector<int64_t> &history)
{
    bool end_condition = true;
    for (auto h : history)
        if (h != 0)
        {
            end_condition = false;
            break;
        }
    if (end_condition)
        return 0;
    if (PART_1)
        return history.at(history.size() - 1) + find_predicted_point(generate_differences(history));
    else
        return history.at(0) - find_predicted_point(generate_differences(history));
}

int64_t sum_predicted_points(const vector<vector<int64_t>> &histories)
{
    int64_t sum {};
    
    for (auto history : histories)
        sum += find_predicted_point(history);
    
    return sum;
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
    vector<vector<int64_t>> histories;
    while (!file.eof())
    {
        getline(file, temp);
        histories.emplace_back(parse_line_to_history(temp));
    }
    
    file.close();
    
    // Part 1
    if (PART_1)
        cout << "The sum of predicted points (back): " << sum_predicted_points(histories) << endl;
    // Part 2
    else
        cout << "The sum of predicted points (front): " << sum_predicted_points(histories) << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
