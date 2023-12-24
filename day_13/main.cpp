#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0
#define PART_1 false

uint64_t sum_mirror(const vector<string> &input_mirror)
{
    auto mirror = input_mirror;
    // Check for a horizontal line
    // We check each line as a possible candidate
    for (int line = 1; line < mirror.size(); ++line)
    {
        int rows_to_check = min(line, (int)(mirror.size() - line), [](int a, int b){return a < b;});
        bool match = true;
        for (int offset = 0; offset < rows_to_check; ++offset)
        {
            if (mirror.at(line - offset - 1) != mirror.at(line + offset))
            {
                match = false;
                break;
            }
        }
        if (!match)
            continue;
        return 100 * line;
    }

    // To make this easy we simply transpose the grid, so we can operate on a row by row basis
    vector<string> temp {};
    for (int i = 0; i < mirror.at(0).length(); ++i)
        temp.emplace_back("");

    for (auto line : mirror)
        for (int i = 0; i < line.length(); ++i)
            temp.at(i) += line.at(i);

    mirror = temp;
    // Check for a vertical line
    // We check each line as a possible candidate
    for (int line = 1; line < mirror.size(); ++line)
    {
        int rows_to_check = min(line, (int)(mirror.size() - line), [](int a, int b){return a < b;});
        bool match = true;
        for (int offset = 0; offset < rows_to_check; ++offset)
        {
            if (mirror.at(line - offset - 1) != mirror.at(line + offset))
            {
                match = false;
                break;
            }
        }
        if (!match)
            continue;
        return line;
    }
}

uint64_t sum_mirrors(vector<vector<string>> &mirrors)
{
    uint64_t sum {};

    for (auto &mirror : mirrors)
        sum += sum_mirror(mirror);

    return sum;
}

int difference(const string &a, const string &b)
{
    int count {};
    int diff {-1};
    for (int i = 0; i < a.length(); ++i)
    {
        if (a.at(i) != b.at(i))
        {
            diff = i;
            count++;
        }
        if (count > 1)
            return -2;
    }
    return diff;
}

uint64_t correct_mirror(const vector<string> &input_mirror)
{
    vector<string> mirror = input_mirror;

    // Check for a horizontal line
    // We check each line as a possible candidate
    for (int line = 1; line < mirror.size(); ++line)
    {
        int rows_to_check = min(line, (int)(mirror.size() - line), [](int a, int b){return a < b;});
        bool match = false;
        int diffs {};
        int diff {};
        int pos {};
        int of {};
        for (int offset = 0; offset < rows_to_check; ++offset)
        {
            if ((diff = difference(mirror.at(line - offset - 1), mirror.at(line + offset))) >= 0)
            {
                diffs++;
                if (diffs == 1)
                {
                    of = offset;
                    match = true;
                }
                if (diffs > 1)
                {
                    match = false;
                    break;
                }
                pos = diff;
            }
            else if (diff == -2)
            {
                match = false;
                break;
            }
        }
        if (!match)
            continue;

        if (mirror.at(line - of - 1).at(pos) == '#')
            mirror.at(line - of - 1).at(pos) = '.';
        else
            mirror.at(line - of - 1).at(pos) = '#';

        return 100 * line;
    }

    // To make this easy we simply transpose the grid, so we can operate on a row by row basis
    vector<string> temp {};
    for (int i = 0; i < mirror.at(0).length(); ++i)
        temp.emplace_back("");

    for (auto line : mirror)
        for (int i = 0; i < line.length(); ++i)
            temp.at(i) += line.at(i);

    mirror = temp;
    // Check for a vertical line
    // We check each line as a possible candidate
    for (int line = 1; line < mirror.size(); ++line)
    {
        int rows_to_check = min(line, (int)(mirror.size() - line), [](int a, int b){return a < b;});
        bool match = false;
        int diffs {};
        int diff {};
        int pos {};
        int of {};
        for (int offset = 0; offset < rows_to_check; ++offset)
        {
            if ((diff = difference(mirror.at(line - offset - 1), mirror.at(line + offset))) >= 0)
            {
                diffs++;
                if (diffs == 1)
                {
                    of = offset;
                    match = true;
                }
                if (diffs > 1)
                {
                    match = false;
                    break;
                }
                pos = diff;
            }
            else if (diff == -2)
            {
                match = false;
                break;
            }
        }
        if (!match)
            continue;

        if (mirror.at(line - of - 1).at(pos) == '#')
            mirror.at(line - of - 1).at(pos) = '.';
        else
            mirror.at(line - of - 1).at(pos) = '#';

        return line;
    }

    cout << "Should find a line" << endl;
    exit(1);
}

uint64_t sum_corrected_mirror(const vector<string> &input_mirror, int prev_line)
{
    auto mirror = input_mirror;
    // Check for a horizontal line
    // We check each line as a possible candidate
    for (int line = 1; line < mirror.size(); ++line)
    {
        if (prev_line == line)
            continue; // Skip the previous line

        int rows_to_check = min(line, (int)(mirror.size() - line), [](int a, int b){return a < b;});
        bool match = true;
        for (int offset = 0; offset < rows_to_check; ++offset)
        {
            if (mirror.at(line - offset - 1) != mirror.at(line + offset))
            {
                match = false;
                break;
            }
        }
        if (!match)
            continue;
        return 100 * line;
    }

    // To make this easy we simply transpose the grid, so we can operate on a row by row basis
    vector<string> temp {};
    for (int i = 0; i < mirror.at(0).length(); ++i)
        temp.emplace_back("");

    for (auto line : mirror)
        for (int i = 0; i < line.length(); ++i)
            temp.at(i) += line.at(i);

    mirror = temp;
    // Check for a vertical line
    // We check each line as a possible candidate
    for (int line = 1; line < mirror.size(); ++line)
    {
        if (prev_line == -line)
            continue; // Skip previous line

        int rows_to_check = min(line, (int)(mirror.size() - line), [](int a, int b){return a < b;});
        bool match = true;
        for (int offset = 0; offset < rows_to_check; ++offset)
        {
            if (mirror.at(line - offset - 1) != mirror.at(line + offset))
            {
                match = false;
                break;
            }
        }
        if (!match)
            continue;
        return line;
    }
}

uint64_t sum_corrected_mirrors(vector<vector<string>> &mirrors)
{
    uint64_t sum {};

    vector<int> corrections {};

    for (int i = 0; i < mirrors.size(); ++i)
    {
        int sum = (int)sum_mirror(mirrors.at(i));
        if (sum % 100 == 0)
            sum /= -100;
        corrections.emplace_back(sum);
    }

    for (int i = 0; i < mirrors.size(); ++i)
        sum += correct_mirror(mirrors.at(i)), corrections.at(i);

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
    vector<vector<string>> mirrors;
    vector<string> mirror;
    while (!file.eof())
    {
        getline(file, temp);
        if (temp.empty())
        {
            mirrors.emplace_back(mirror);
            mirror = {};
        }
        else
            mirror.emplace_back(temp);
    }
    mirrors.emplace_back(mirror);
    
    file.close();
    
    // Part 1
    if (PART_1)
        cout << "Sum from lines is: " << sum_mirrors(mirrors) << endl;
    // Part 2
    else
        cout << "Sum from lines after correction: " << sum_corrected_mirrors(mirrors) << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
