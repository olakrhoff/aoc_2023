#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>

using namespace std;

#define DEBUG 0
#define PART_1 false

vector<string> split(const string &line, const string &delimiter)
{
    vector<string> res {};

    string temp = line;
    while (temp.find(delimiter) != -1)
    {
        res.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + delimiter.length());
    }
    res.emplace_back(temp);

    return res;
}

typedef enum TYPE
{
    NONE,
    FLIP_FLOP,
    CONJUNCTION
} TYPE_T;

void parse_input(const string &line, map<string, tuple<vector<string>, TYPE_T, bool>> &adjacent)
{
    vector<string> nodes {};

    string from = line.substr(0, line.find(' '));
    TYPE_T type {NONE};
    if (from.at(0) == '%')
    {
        type = FLIP_FLOP;
        from = from.substr(1);
    }
    else if (from.at(0) == '&')
    {
        type = CONJUNCTION;
        from = from.substr(1);
    }

    string list = line.substr(line.find('>') + 2);

    for (auto n : split(list, ", "))
        nodes.emplace_back(n);

    if (!adjacent.contains(from))
        adjacent.insert({from, {}});
    for (auto n : nodes)
        get<0>(adjacent.at(from)).emplace_back(n);
    get<1>(adjacent.at(from)) = type;
    get<2>(adjacent.at(from)) = false;
}

pair<uint64_t, uint64_t> send_pulse(const string &start, map<string, tuple<vector<string>, TYPE_T, bool>> &adjacent,
                                    map<string, vector<pair<string, bool>>> &conjunction_map)
{
    uint64_t low_pulse_counter {1};
    uint64_t high_pulse_counter {0};
    queue<tuple<string, bool, string>> modules {};
    modules.push({start, false, start});

    while (!modules.empty())
    {
        string current;
        bool signal;
        string from;
        tie(current, signal, from) = modules.front();
        modules.pop();

        if (!adjacent.contains(current))
            continue;

        switch (get<1>(adjacent.at(current)))
        {
            case NONE:
                for (const auto &mod : get<0>(adjacent.at(current)))
                {
                    modules.push({mod, signal, current});
                    if (signal)
                        high_pulse_counter++;
                    else
                        low_pulse_counter++;
                }
                break;
            case FLIP_FLOP:
            {
                if (signal)
                    break;
                bool &state = get<2>(adjacent.at(current));
                state = !state;
                for (const auto &mod : get<0>(adjacent.at(current)))
                {
                    modules.push({mod, state, current});
                    if (state)
                        high_pulse_counter++;
                    else
                        low_pulse_counter++;
                }
                break;
            }
            case CONJUNCTION:
                for (auto &val : conjunction_map.at(current))
                    if (val.first == from)
                        val.second = signal;


                bool all_high {true};
                for (const auto &val : conjunction_map.at(current))
                    if (!val.second)
                    {
                        all_high = false;
                        break;
                    }

                for (const auto &val : get<0>(adjacent.at(current)))
                {
                    modules.push({val, !all_high, current});
                    if (!all_high)
                        high_pulse_counter++;
                    else
                        low_pulse_counter++;
                }
                break;
        }
    }
    return {low_pulse_counter, high_pulse_counter};
}

uint64_t find_pulses_sent(map<string, tuple<vector<string>, TYPE_T, bool>> &adjacent,
                          map<string, vector<pair<string, bool>>> conjunction_map,
                          uint64_t iterations, const string &start)
{
    uint64_t low_pulse_count {};
    uint64_t high_pulse_count {};

    for (int i = 0; i < iterations; ++i)
    {
        auto temp = send_pulse(start, adjacent, conjunction_map);
        low_pulse_count += temp.first;
        high_pulse_count += temp.second;
    }

    return low_pulse_count * high_pulse_count;
}

map<string, vector<pair<string, bool>>> create_conjunction_map(const map<string,
        tuple<vector<string>, TYPE_T, bool>> &adjacent)
{
    map<string, vector<pair<string, bool>>> conjunction_map {};

    for (auto [to, list] : adjacent)
    {
        for (const auto &from : get<0>(list))
        {
            if (!conjunction_map.contains(from))
                conjunction_map.insert({from, {}});
            conjunction_map.at(from).emplace_back(to, false);
        }
    }

    return conjunction_map;
}

bool send_pulse_and_check_end(const string &start, const string &end,
                              map<string, tuple<vector<string>, TYPE_T, bool>> &adjacent,
                              map<string, vector<pair<string, bool>>> &conjunction_map)
{
    queue<tuple<string, bool, string>> modules {};
    modules.push({start, false, start});

    while (!modules.empty())
    {
        string current;
        bool signal;
        string from;
        tie(current, signal, from) = modules.front();
        modules.pop();

        if (!signal && current == end)
            return true;

        if (!adjacent.contains(current))
            continue;

        switch (get<1>(adjacent.at(current)))
        {
            case NONE:
                for (const auto &mod : get<0>(adjacent.at(current)))
                    modules.push({mod, signal, current});
                break;
            case FLIP_FLOP:
            {
                if (signal)
                    break;
                bool &state = get<2>(adjacent.at(current));
                state = !state;
                for (const auto &mod : get<0>(adjacent.at(current)))
                    modules.push({mod, state, current});
                break;
            }
            case CONJUNCTION:
                for (auto &val : conjunction_map.at(current))
                    if (val.first == from)
                        val.second = signal;


                bool all_high {true};
                for (const auto &val : conjunction_map.at(current))
                    if (!val.second)
                    {
                        all_high = false;
                        break;
                    }

                for (const auto &val : get<0>(adjacent.at(current)))
                    modules.push({val, !all_high, current});
                break;
        }
    }
    return false;
}

uint64_t find_min(map<string, tuple<vector<string>, TYPE_T, bool>> &adjacent,
                        map<string, vector<pair<string, bool>>> &conjunction_map,
                        const string &current, bool state)
{
    TYPE_T type = get<1>(adjacent.at(current));
    switch (type)
    {
        case NONE:
            if (current == "rx")
            {
                cout << "Hmmm, we have a loop" << endl;
                exit(EXIT_FAILURE);
            }
            if (current == "broadcaster")
            {
                return 1;
            }
            else
            {
                cout << "What is this, it should have a type: " << current << endl;
                exit(EXIT_FAILURE);
            }
            break;
        case FLIP_FLOP:
        {

            break;
        }
        case CONJUNCTION:
        {

            break;
        }
    }
}

uint64_t find_min_steps(map<string, tuple<vector<string>, TYPE_T, bool>> &adjacent,
                        map<string, vector<pair<string, bool>>> &conjunction_map,
                        const string &current)
{
    auto conjunction = conjunction_map.at(current);
    uint64_t min_steps = numeric_limits<uint64_t>::max();
    for (auto [name, val] : conjunction)
    {
        auto steps = find_min(adjacent, conjunction_map, name, false);
        if (steps < min_steps)
            min_steps = steps;
    }
    return min_steps;
}

void create_graph(const map<string, tuple<vector<string>, TYPE_T, bool>> &adjacent, const string &filename)
{
    ofstream file;
    file.open(filename);
    string start = "digraph G \n{\n";
    file.write(start.data(), start.length());
    for (auto [name, val] : adjacent)
    {
        vector<string> list;
        TYPE_T type;
        bool state;
        tie(list,type, state) = val;

        string temp = "\t";
        switch (type)
        {
            case NONE:
                break;
            case FLIP_FLOP:
                temp += name + " [color=red, shape=triangle]\n\t";
                break;
            case CONJUNCTION:
                temp += name + " [color=blue, shape=diamond]\n\t";
                break;
        }
        temp += name + " -> ";

        temp += "{";
        for (int i = 0; i < list.size(); ++i)
        {
            temp += list.at(i);
            if (i < list.size() - 1)
                temp += ", ";
        }
        temp += "}\n";
        file.write(temp.data(), temp.length());
    }
    file.write("}", 1);
    file.close();
    string command = "dot -Tpng " + filename + " -o ../graph.png";
    system(command.data());
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
    map<string, tuple<vector<string>, TYPE_T, bool>> adjacent {};
    while (!file.eof())
    {
        getline(file, temp);
        parse_input(temp, adjacent);
    }

    file.close();

    create_graph(adjacent, "../graph.gv");

    auto conjunction_map = create_conjunction_map(adjacent);
    // Part 1
    if (PART_1)
        cout << "Total pulses sent: " << find_pulses_sent(adjacent, conjunction_map, 1000, "broadcaster") << endl;
    // Part 2
    else
        cout << "Minimum steps to activate rx: " << find_min_steps(adjacent, conjunction_map, "rx")
             << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
