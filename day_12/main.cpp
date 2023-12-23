#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>

using namespace std;

#define DEBUG 0
#define PART_1 false

string trim(string line, char delimiter)
{
    while (line.at(0) == delimiter)
        line = line.substr(1);
    while (line.at(line.size() - 1) == delimiter)
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
        temp = trim(temp, delimiter);
    }
    res.emplace_back(temp);

    return res;
}

vector<uint64_t> get_numbers_from_list(const string &line)
{
    vector<uint64_t> nums {};

    vector<string> string_nums = split(line, ',');

    for (auto s: string_nums)
        nums.emplace_back(stoll(s));

    return nums;
}

typedef struct record
{
    string springs {};
    vector<uint64_t> contiguous_springs {};

    bool operator<(const record &rhs) const
    {
        if (springs < rhs.springs)
            return true;
        if (rhs.springs < springs)
            return false;
        return contiguous_springs < rhs.contiguous_springs;
    }

    bool operator>(const record &rhs) const
    {
        return rhs < *this;
    }

    bool operator<=(const record &rhs) const
    {
        return !(rhs < *this);
    }

    bool operator>=(const record &rhs) const
    {
        return !(*this < rhs);
    }

    bool operator==(const record &rhs) const
    {
        return contiguous_springs == rhs.contiguous_springs && springs == rhs.springs;
    }

    bool operator!=(const record &rhs) const
    {
        return !(rhs == *this);
    }
} record_t;

record_t parse_record(const string &record)
{
    record_t rec;

    rec.springs = record.substr(0, record.find(' '));
    rec.contiguous_springs = get_numbers_from_list(record.substr(record.find(' ') + 1));

    return rec;
}

bool valid(const record_t &record)
{
    int broken_expected {-1};
    int broken_index = 0;
    for (int i = 0; i < record.springs.size(); ++i)
    {
        if (broken_expected > 0 && record.springs.at(i) != '#')
            return false;
        if (broken_expected == 0 && record.springs.at(i) == '#')
            return false;
        if (broken_expected < 0 && record.springs.at(i) == '#')
        {
            if (broken_index >= record.contiguous_springs.size())
                return false;
            broken_expected = record.contiguous_springs.at(broken_index);
            broken_index++;
        }
        broken_expected--;
    }

    if (broken_index < record.contiguous_springs.size() || broken_expected > 0)
        return false;
    return true;
}

 set<record_t> generate_permutations(const record_t &record)
 {
    set<record_t> perms {};
    perms.insert(record);
    set<record_t> temp {};

    for (int group = 0; group < record.contiguous_springs.size(); ++group)
    {
        auto length = record.contiguous_springs.at(group);
        for (auto per : perms)
        {
            // Firstly we want to account for the previous groups that they are in order
            int index {};
            for (int prev_group = 0; prev_group < group; ++prev_group)
            {
                auto len = record.contiguous_springs.at(prev_group);
                for (int i = index; i < per.springs.length(); ++i)
                {
                    if (per.springs.at(i) == '#')
                    {
                        index = i + len + 1;
                        break;
                    }
                }
            }
            for (int i = index; i < per.springs.length() - length + 1; ++i)
            {
                bool match = true;
                int miss {-1};
                for (int j = i; j < per.springs.length() && j < i + length; ++j)
                {
                    if (per.springs.at(j) == '.')
                    {
                        match = false;
                        miss = j;
                        break;
                    }
                }
                if (!match)
                {
                    i = miss; // Skip as far ahead as you can
                    continue;
                }
                if (i + length < per.springs.length() - 1)
                    if (per.springs.at(i + length) == '#')
                        continue;
                if (i > 0 && per.springs.at(i - 1) == '#')
                    continue;

                record_t rec = per;
                for (int j = i; j < per.springs.length() && j < i + length; ++j)
                    rec.springs.at(j) = '#';

                if (i + length < per.springs.length() - 1 && rec.springs.at(i + length) == '?')
                    rec.springs.at(i + length) = '.';

                for (int j = 0; j < i; ++j)
                    if (rec.springs.at(j) == '?')
                        rec.springs.at(j) = '.';

                temp.insert(rec);
            }
        }
        perms = temp;
        temp = {};
    }
    for (auto per : perms)
    {
        replace(per.springs.begin(), per.springs.end(), '?', '.');
        temp.insert(per);
    }

    return temp;
 }

uint64_t combinations(const record_t &record)
{
    set<record_t> permutations = generate_permutations(record);

    uint64_t sum {};
    for (auto per : permutations)
        if (valid(per))
            sum++;

    return sum;
}

uint64_t find_combinations(const string &record)
{
    record_t rec = parse_record(record);

    return combinations(rec);
}

uint64_t find_all_combinations(const vector<string> &records)
{
    uint64_t combinations {};

    for (auto &record: records)
        combinations += find_combinations(record);

    return combinations;
}

vector<string> unfold(const vector<string> &records)
{
    vector<string> result {};
    for (auto record : records)
    {
        record_t rec = parse_record(record);
        string temp;
        for (int i = 0; i < 4; ++i)
            temp += rec.springs + '?';
        temp += rec.springs + ' ';

        string nums;
        for (auto num : rec.contiguous_springs)
            nums += to_string(num) + ',';
        nums = nums.substr(0, nums.length() - 1);
        for (int i = 0; i < 4; ++i)
            temp += nums + ',';
        temp += nums;

        result.emplace_back(temp);
    }

    return result;
}

vector<record_t> generate_permutations_for_first_spring(record_t record)
{
    vector<record_t> perms {};

    auto length = record.contiguous_springs.at(0);
    for (int i = 0; i < record.springs.length() - length + 1 && i <= record.springs.find('#'); ++i)
    {
        bool match = true;
        for (int j = i; j < i + length; ++j)
        {
            if (record.springs.at(j) == '.')
            {
                match = false;
                break;
            }
        }
        if (!match)
            continue;
        if (i > 0)
            if (record.springs.at(i - 1) == '#')
                continue;
        if (i + length < record.springs.length())
            if (record.springs.at(i + length) == '#')
                continue;

        record_t new_rec;

        int offset = 0;
        if (i + length < record.springs.length())
            offset = 1;
        new_rec.springs = record.springs.substr(i + length + offset);
        new_rec.contiguous_springs = record.contiguous_springs;
        new_rec.contiguous_springs.erase(new_rec.contiguous_springs.begin());

        perms.emplace_back(new_rec);
    }

    return perms;
}

uint64_t length_needed(const vector<uint64_t> &nums)
{
    uint64_t sum {};
    for (auto num : nums)
        sum += num;

    return sum + nums.size() - 1;
}

uint64_t find_combinations_recursive(record_t record, map<record_t, uint64_t> &cache)
{
    // If we have solved the problem before we just return the cached value
    if (cache.find(record) != cache.end())
        return cache.at(record);


    if (record.springs.empty())
    {
        if (record.contiguous_springs.empty())
        {
            cache.insert({record, 1});
            return 1;
        }
        else
        {
            cache.insert({record, 0});
            return 0;
        }
    }
    else if (record.contiguous_springs.empty())
    {
        for (auto spring : record.springs)
        {
            if (spring == '#')
            {
                cache.insert({record, 0});
                return 0;
            }
        }
        cache.insert({record, 1});
        return 1;
    }
    else if (record.springs.length() < length_needed(record.contiguous_springs))
    {
        cache.insert({record, 0});
        return 0;
    }

    // If it was not cached we need to calculate the value
    vector<record_t> perms = generate_permutations_for_first_spring(record);
    uint64_t sum {};

    for (auto perm : perms)
        sum += find_combinations_recursive(perm, cache);

    // Add the calculated value to the cache
    cache.insert({record, sum});

    return sum;
}

uint64_t find_all_combinations_recursive(const vector<string> &records)
{
    uint64_t combinations {};

    for (auto &record: records)
    {
        map<record_t, uint64_t> cache;
        combinations += find_combinations_recursive(parse_record(record), cache);
    }

    return combinations;
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
    vector<string> records {};
    while (!file.eof())
    {
        getline(file, temp);
        records.emplace_back(temp);
    }

    file.close();

    if (!PART_1)
        records = unfold(records);

    // Part 1
    if (PART_1)
        // This line is kept as a memory, so wonder at the improvement made in the recursive memoisation attempt
        //cout << "The number of combinations of springs are: " << find_all_combinations(records) << endl;
        cout << "The number of combinations of springs are: " << find_all_combinations_recursive(records) << endl;
    // Part 2
    else
        cout << "The number of combinations of the unfolded springs are: " << find_all_combinations_recursive(records) << endl;



    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
