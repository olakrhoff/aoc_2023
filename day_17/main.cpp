#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>

using namespace std;

#define DEBUG 0

typedef enum DIRECTIONS
{
    UNKNOWN,
    NORTH,
    WEST,
    SOUTH,
    EAST
} DIRECTIONS_T;

typedef struct coord
{
    int row {}, col {};
    DIRECTIONS_T dir {UNKNOWN};
    int moves_left {};

    coord() = default;

    coord(int row, int col, DIRECTIONS_T dir, int moves_left) : row(row), col(col), dir(dir), moves_left(moves_left)
    {}

    bool operator<(const coord &rhs) const
    {
        if (row != rhs.row)
            return row < rhs.row;
        if (col != rhs.col)
            return col < rhs.col;
        if (dir != rhs.dir)
            return dir < rhs.dir;
        return moves_left < rhs.moves_left;
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
               col == rhs.col &&
               dir == rhs.dir &&
               moves_left == rhs.moves_left;
    }

    bool operator!=(const coord &rhs) const
    {
        return !(*this == rhs);
    }
} coord_t;

vector<coord_t> get_moves(pair<uint64_t, coord_t> state, int max_row, int max_col)
{
    vector<coord_t> moves {};

    coord_t current = state.second;

    int current_moves_left = current.moves_left;
    DIRECTIONS_T current_dir = current.dir;
    // First we find all neighbours of the grid cell, that does not move in the direction we came from
    if (current.dir != SOUTH)
        moves.emplace_back(current.row - 1, current.col, NORTH, NORTH == current_dir ? current_moves_left - 1 : 2);
    if (current.dir != EAST)
        moves.emplace_back(current.row, current.col - 1, WEST, WEST == current_dir ? current_moves_left - 1 : 2);
    if (current.dir != NORTH)
        moves.emplace_back(current.row + 1, current.col, SOUTH, SOUTH == current_dir ? current_moves_left - 1 : 2);
    if (current.dir != WEST)
        moves.emplace_back(current.row, current.col + 1, EAST, EAST == current_dir ? current_moves_left - 1 : 2);

    // Filter out those out of bound
    vector<coord_t> filtered_moves {};
    std::copy_if(moves.begin(), moves.end(), back_inserter(filtered_moves), [max_row, max_col](coord_t coord) {
        return coord.row >= 0 && coord.row < max_row && coord.col >= 0 && coord.col < max_col && coord.moves_left >= 0;
    });

    return filtered_moves;
}

class Compare
{
public:
    bool operator()(pair<coord_t, uint64_t> below, pair<coord_t, uint64_t> above)
    {
        return below.second > above.second;
    }
};

// We want a BFS, from the top left to the bottom right, that has a
// maximum of three consecutive moves in the same direction
uint64_t minimise_heat_loss(const vector<string> &grid)
{
    coord_t start {0, 0, UNKNOWN, 3};
    coord_t end {(int) (grid.size() - 1), (int) (grid.at(0).size() - 1), UNKNOWN, 0};
    int max_row = grid.size();
    int max_col = grid.at(0).size();

    priority_queue<pair<uint64_t, coord_t>, vector<pair<uint64_t, coord_t>>, greater<>> coords {};
    coords.emplace(0, start);

    set<coord_t> visited {};
    while (!coords.empty())
    {
        pair<uint64_t, coord_t> current = coords.top();
        coords.pop();

        if (current.second.row == end.row && current.second.col == end.col)
            return current.first;
        if (visited.contains(current.second)) // We have a difference in state
            continue;
        visited.insert(current.second);


        uint64_t heat_loss = current.first;

        vector<coord_t> next_moves = get_moves(current, max_row, max_col);

        for (auto move: next_moves)
        {
            uint64_t heat_loss_at_move = grid.at(move.row).at(move.col) - '0'; // Here we offset the char number value by the ASCII '0' to get correct number
            coords.emplace(heat_loss + heat_loss_at_move, move);
        }
    }

    cout << "We should find a value" << endl;
    exit(EXIT_FAILURE);
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
    cout << "The minimised heat loss is: " << minimise_heat_loss(grid) << endl;

    // Part 2

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
