#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0
#define PART_1 false

vector<string> split(const string &line, const char delimiter)
{
    vector<string> res {};

    string temp = line;
    while (temp.find(delimiter) != -1)
    {
        res.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + 1);
    }
    res.emplace_back(temp);

    return res;
}

uint8_t hash_code(const string &code)
{
    uint8_t res {};

    for (auto c : code)
    {
        res += c;
        res *= 17;
    }

    return res;
}

uint64_t sum_hash(const vector<string> &codes)
{
    uint64_t sum {};

    for (auto &c : codes)
        sum += hash_code(c);

    return sum;
}

void insert_lens(vector<vector<pair<string, uint8_t>>> &boxes, const string &code)
{
    string label;
    string focal_length_str;
    char operation;
    bool reading_label {true};
    for (auto c : code)
    {
        if (c == '=' || c == '-')
        {
            operation = c;
            reading_label = false;
            continue;
        }
        if (reading_label)
            label += c;
        else
            focal_length_str += c;
    }
    uint64_t focal_length;
    if (!focal_length_str.empty())
        focal_length = stoi(focal_length_str);

    uint8_t box_index = hash_code(label);

    auto &box = boxes.at(box_index);
    if (operation == '=')
    {
        for (auto & itr : box)
        {
            if (itr.first == label)
            {
                itr.second = focal_length;
                return;
            }
        }

        box.emplace_back(label, focal_length);
        return;
    }
    else if (operation == '-')
    {
        for (auto itr = box.begin(); itr != box.end(); itr++)
        {

            if ((*itr).first == label)
            {
                box.erase(itr);
                return;
            }
        }
    }
    else
    {
        cout << "Should have read an operation while parsing the code" << endl;
        exit(1);
    }
}

uint64_t sum_lenses(const vector<string> &codes)
{
    vector<vector<pair<string, uint8_t>>> boxes(256);
    for (const auto& c : codes)
        insert_lens(boxes, c);

    uint64_t focus_power {};

    for (int i = 0; i < boxes.size(); ++i)
        for (int j = 0; j < boxes.at(i).size(); ++j)
            focus_power += (1 + i) * (j + 1) * boxes.at(i).at(j).second;

    return focus_power;
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
    vector<string> codes {};
    while (!file.eof())
    {
        getline(file, temp);
        codes = split(temp, ',');
    }
    
    file.close();

    // Part 1
    if (PART_1)
        cout << "Sum of hash code is: " << sum_hash(codes) << endl;
    // Part 2
    else
        cout << "Sum of installed lenses: " << sum_lenses(codes) << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
