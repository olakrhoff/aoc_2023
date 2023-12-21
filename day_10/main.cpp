#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>

using namespace std;

#define DEBUG 0

typedef struct coord
{
    int64_t row {}, col {};

    coord(int64_t row, int64_t col) : row(row), col(col)
    {}

    coord() = default;

    bool operator<(const coord &rhs) const
    {
        if (row < rhs.row)
            return true;
        if (rhs.row < row)
            return false;
        return col < rhs.col;
    }

    bool operator>(const coord &rhs) const
    {
        return rhs < *this;
    }

    bool operator<=(const coord &rhs) const
    {
        return !(rhs < *this);
    }

    bool operator>=(const coord &rhs) const
    {
        return !(*this < rhs);
    }

    bool operator==(const coord &rhs) const
    {
        return row == rhs.row &&
               col == rhs.col;
    }

    bool operator!=(const coord &rhs) const
    {
        return !(rhs == *this);
    }

} coord_t;

uint64_t find_furthest_distance(const vector<string> &grid)
{
    coord_t start;
    map<coord_t, vector<coord_t>> adjacent {};
    for (int row = 0; row < grid.size(); ++row)
    {
        for (int col = 0; col < grid.at(row).length(); ++col)
        {
            // Find the neighbours based on the type of pipe
            vector<coord_t> neighbours {};
            switch (grid.at(row).at(col))
            {
                case '|':
                    neighbours.emplace_back(row + 1, col);
                    neighbours.emplace_back(row - 1, col);
                    break;
                case '-':
                    neighbours.emplace_back(row, col - 1);
                    neighbours.emplace_back(row, col + 1);
                    break;
                case 'L':
                    neighbours.emplace_back(row - 1, col);
                    neighbours.emplace_back(row, col + 1);
                    break;
                case 'J':
                    neighbours.emplace_back(row - 1, col);
                    neighbours.emplace_back(row, col - 1);
                    break;
                case '7':
                    neighbours.emplace_back(row, col - 1);
                    neighbours.emplace_back(row + 1, col);
                    break;
                case 'F':
                    neighbours.emplace_back(row, col + 1);
                    neighbours.emplace_back(row + 1, col);
                    break;
                case '.':
                    continue;
                    break;
                case 'S':
                    start = {row, col};
                    break;
                default:
                    cout << "Did not recognise the char" << endl;
                    exit(1);
            }

            // Add the edges to the adjacent structure
            for (auto neighbour : neighbours)
                if (neighbour.row >= 0 && neighbour.row < grid.size() &&
                    neighbour.col >= 0 && neighbour.col < grid.at(row).length())
                {
                    auto itr = adjacent.find({row, col});
                    if (itr == adjacent.end())
                        adjacent.insert({{row, col},{}});
                    adjacent.at({row, col}).emplace_back(neighbour);
                }
        }
    }

    // Now we need to add the edges from the start point
    for (auto [node, list] : adjacent)
        for (auto to_node : list)
            if (to_node == start)
            {
                auto itr = adjacent.find(start);
                if (itr == adjacent.end())
                    adjacent.insert({start,{}});
                adjacent.at(start).emplace_back(node);
            }

    // Now we run Dijkstra to find the shortest distance to all points, then find the furthest point of these
    queue<coord_t> dijkstra_queue;

    dijkstra_queue.push(start);

    pair<coord_t, uint64_t> result = {start, 0};
    map<coord_t, uint64_t> distances {{start, 0}};
    while (!dijkstra_queue.empty())
    {
        auto current = dijkstra_queue.front();
        dijkstra_queue.pop();

        for (auto next : adjacent.at(current))
        {
            auto itr = distances.find(next);
            if (itr == distances.end())
            {
                distances.insert({next,distances.at(current) + 1});
                dijkstra_queue.push(next);
                if (result.second < distances.at(next))
                    result = {next, distances.at(next)};
            }
        }
    }

    return result.second;
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
    vector<string> grid {};
    while (!file.eof())
    {
        getline(file, temp);
        grid.emplace_back(temp);
    }

    file.close();

    // Part 1
    cout << "The furthest distance is: " << find_furthest_distance(grid) << endl;

    // Part 2

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
