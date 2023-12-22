#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <queue>

using namespace std;

#define DEBUG 0
#define PART_1 false

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

    coord operator-(const coord &rhs) const
    {
        return {row - rhs.row, col - rhs.col};
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
                case 'S':
                    start = {row, col};
                    break;
                case 'I': //Temp ignore
                    break;
                default:
                    cout << "Did not recognise the char: " << grid.at(row).at(col) << endl;
                    exit(1);
            }

            // Add the edges to the adjacent structure
            for (auto neighbour: neighbours)
                if (neighbour.row >= 0 && neighbour.row < grid.size() &&
                    neighbour.col >= 0 && neighbour.col < grid.at(row).length())
                {
                    auto itr = adjacent.find({row, col});
                    if (itr == adjacent.end())
                        adjacent.insert({{row, col},
                                         {}});
                    adjacent.at({row, col}).emplace_back(neighbour);
                }
        }
    }

    // Now we need to add the edges from the start point
    for (auto [node, list]: adjacent)
        for (auto to_node: list)
            if (to_node == start)
            {
                auto itr = adjacent.find(start);
                if (itr == adjacent.end())
                    adjacent.insert({start, {}});
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

        for (auto next: adjacent.at(current))
        {
            auto itr = distances.find(next);
            if (itr == distances.end())
            {
                distances.insert({next, distances.at(current) + 1});
                dijkstra_queue.push(next);
                if (result.second < distances.at(next))
                    result = {next, distances.at(next)};
            }
        }
    }

    return result.second;
}

typedef enum DIRECTIONS
{
    NORTH,
    EAST,
    SOUTH,
    WEST
} DIRECTIONS_T;

DIRECTIONS_T get_direction(const coord_t from, const coord_t to)
{
    coord_t diff = to - from;

    if (diff.row > 0)
        return SOUTH;
    if (diff.row < 0)
        return NORTH;
    if (diff.col > 0)
        return EAST;
    if (diff.col < 0)
        return WEST;

    cout << "We should get a direction" << endl;
    exit(1);
}

uint64_t find_area(const vector<string> &grid)
{
    // Construct the main loop
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
                case 'S':
                    start = {row, col};
                    break;
                case 'I': // Temp ignore
                    break;
                default:
                    cout << "Did not recognise the char" << endl;
                    exit(1);
            }

            // Add the edges to the adjacent structure
            for (auto neighbour: neighbours)
                if (neighbour.row >= 0 && neighbour.row < grid.size() &&
                    neighbour.col >= 0 && neighbour.col < grid.at(row).length())
                {
                    auto itr = adjacent.find({row, col});
                    if (itr == adjacent.end())
                        adjacent.insert({{row, col},
                                         {}});
                    adjacent.at({row, col}).emplace_back(neighbour);
                }
        }
    }

    // Now we need to add the edges from the start point
    for (auto [node, list]: adjacent)
        for (auto to_node: list)
            if (to_node == start)
            {
                auto itr = adjacent.find(start);
                if (itr == adjacent.end())
                    adjacent.insert({start, {}});
                adjacent.at(start).emplace_back(node);
            }

    // We need to clean the graph such that we only have the main loop
    bool changed = true;
    while (changed)
    {
        changed = false;

        for (auto itr = adjacent.begin(); itr != adjacent.end();)
        {
            if ((*itr).second.size() < 2)
            {
                itr = adjacent.erase(itr);
                changed = true;
            }
            else
                itr++;
        }

        // Loop over all nodes in the maze
        for (auto itr = adjacent.begin(); itr != adjacent.end(); itr++)
        {

            coord_t current = (*itr).first;

            // Loop over each neighbour
            for (auto neighbour: (*itr).second)
            {
                bool found = false;
                auto node_itr = adjacent.find(neighbour);
                // Check if the neighbour is a GROUND point
                if (node_itr != adjacent.end())
                {
                    for (auto node: adjacent.at(neighbour))
                    {
                        // If our neighbour points to us, we are happy
                        if (node == current)
                        {
                            found = true;
                            break;
                        }
                    }
                }

                // If the neighbour points tous we are happy and can check our next neighbour
                if (found)
                    continue;

                // If our neighbour does not recognise (point to) us we must obviously cease to exist
                if (!found)
                {
                    // Remove all occurrences of us
                    vector<coord_t> possible_neighbours = {{current.row + 1, current.col},
                                                           {current.row - 1, current.col},
                                                           {current.row,     current.col + 1},
                                                           {current.row,     current.col - 1}};

                    vector<coord_t> actual_neighbours {};
                    // We must remove any possible neighbour that is not actually a point in the maze
                    for (auto & possible_neighbour : possible_neighbours)
                        if (adjacent.find(possible_neighbour) != adjacent.end())
                            actual_neighbours.emplace_back(possible_neighbour);

                    // Loop over all possible neighbours
                    for (auto an: actual_neighbours)
                    {
                        vector<coord_t> new_list {};
                        // Loop over all nodes a neighbour points to
                        for (auto node: adjacent.at(an))
                            // If it points to our current node
                            if (node != current)
                                new_list.emplace_back(node);
                        adjacent.at(an) = new_list;
                    }

                    // Lastly remove the entry of the current node and all its points
                    itr = adjacent.erase(itr);
                    if (itr == adjacent.end())
                        itr--;
                    changed = true;
                    break;
                }
            }
        }
    }

    // Walk along the main loop
    coord_t current = start, prev = start;

    // Find the point on the main loop which is the highest up
    coord_t max_coord = start;
    do
    {
        if (current.row < max_coord.row)
            max_coord = current;
        else if (current.row == max_coord.row && current.col > max_coord.col)
            max_coord = current;

        bool found = false;
        for (auto next: adjacent.at(current))
            if (next != prev)
            {
                prev = current;
                current = next;
                found = true;
                break;
            }
        if (!found)
        {
            cout << "We should always find a next point in a loop" << endl;
            exit(1);
        }
    } while (current != start);

    // Walk the main loop and find all interior points
    set<coord_t> interior {};
    current = max_coord, prev = max_coord;

    if (adjacent.at(current).size() != 2)
    {
        cout << "Should only have two neighbours" << endl;
        exit(1);
    }

    // Ensure that we explore in clockwise order such that we use the
    // left hand rule and check for interior points to the right
    if (get_direction(current, adjacent.at(current).at(0)) == SOUTH)
        prev = adjacent.at(current).at(1);
    else
        prev = adjacent.at(current).at(0);

    do
    {
        DIRECTIONS_T prev_direction;
        //cout << "current: " << current.row << ", " << current.col << endl;
        // Using left hand rule, add all '.' to the right to a set
        switch (prev_direction = get_direction(prev, current))
        {
            case NORTH:
                for (int col = current.col + 1; adjacent.find({current.row, col}) == adjacent.end(); ++col)
                    interior.insert({current.row, col});
                break;
            case EAST:
                for (int row = current.row + 1; adjacent.find({row, current.col}) == adjacent.end(); ++row)
                    interior.insert({row, current.col});
                break;
            case SOUTH:
                for (int col = current.col - 1; adjacent.find({current.row, col}) == adjacent.end(); --col)
                    interior.insert({current.row, col});
                break;
            case WEST:
                for (int row = current.row - 1; adjacent.find({row, current.col}) == adjacent.end(); --row)
                    interior.insert({row, current.col});
                break;
        }
        bool found = false;
        for (auto next: adjacent.at(current))
            if (next != prev)
            {
                // If we round an interior corner we need to check both directions from the corner
                DIRECTIONS_T next_direction = get_direction(current, next);
                // Checks if we don't have south and north, or west and east
                // in other words if we have a corner
                if ((next_direction + prev_direction) % 2 != 0)
                {
                    switch (next_direction)
                    {
                        case NORTH:
                            for (int col = current.col + 1; adjacent.find({current.row, col}) == adjacent.end(); ++col)
                                interior.insert({current.row, col});
                            break;
                        case EAST:
                            for (int row = current.row + 1; adjacent.find({row, current.col}) == adjacent.end(); ++row)
                                interior.insert({row, current.col});
                            break;
                        case SOUTH:
                            for (int col = current.col - 1; adjacent.find({current.row, col}) == adjacent.end(); --col)
                                interior.insert({current.row, col});
                            break;
                        case WEST:
                            for (int row = current.row - 1; adjacent.find({row, current.col}) == adjacent.end(); --row)
                                interior.insert({row, current.col});
                            break;
                    }
                }
                prev = current;
                current = next;
                found = true;
                break;
            }
        if (!found)
        {
            cout << "We should always find a next point in a loop" << endl;
            exit(1);
        }


    } while (current != max_coord);

    // Print the grid
    cout << endl;
    for (int row = 0; row < grid.size(); ++row)
    {
        for (int col = 0; col < grid.at(row).length(); ++col)
        {
            auto itr = interior.find({row, col});
            if (itr != interior.end())
                cout << 'I';
            else if (adjacent.find({row, col}) != adjacent.end())
                cout << grid.at(row).at(col);
            else
                cout << '.';
        }
        cout << endl;
    }


    // Return size of set
    return interior.size();
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
    if (PART_1)
        cout << "The furthest distance is: " << find_furthest_distance(grid) << endl;
        // Part 2
    else
        cout << "The area eclosed by the main loop is: " << find_area(grid) << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
