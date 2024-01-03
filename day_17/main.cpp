#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>

using namespace std;

#define DEBUG 0
#define PART_1 false

typedef enum DIRECTIONS
{
    NORTH,
    WEST,
    SOUTH,
    EAST,
    UNKNOWN
} DIRECTIONS_T;

typedef struct coord
{
    int16_t row {}, col {};
    DIRECTIONS_T dir {UNKNOWN};

    coord() = default;

    coord(int16_t row, int16_t col, DIRECTIONS_T dir) : row(row), col(col), dir(dir)
    {}

    bool operator<(const coord &rhs) const
    {
        if (row != rhs.row)
            return row < rhs.row;
        if (col != rhs.col)
            return col < rhs.col;
        return dir < rhs.dir;
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
               dir == rhs.dir;
    }

    bool operator!=(const coord &rhs) const
    {
        return !(*this == rhs);
    }

    DIRECTIONS_T get_direction(const coord &rhs) const
    {
        if (row < rhs.row)
            return SOUTH;
        if (row > rhs.row)
            return NORTH;
        if (col < rhs.col)
            return EAST;
        if (col > rhs.col)
            return WEST;

        cout << "Should not get here" << endl;
        exit(EXIT_FAILURE);
    }

} coord_t;

vector<coord_t> get_moves(coord_t state, int max_row, int max_col, int min_distance, int max_distance)
{
    vector<coord_t> moves {};


    switch (state.dir)
    {
        case NORTH:
            for (int left = min_distance; left <= max_distance; ++left)
                moves.emplace_back(state.row, state.col - left, WEST);
            for (int right = min_distance; right <= max_distance; ++right)
                moves.emplace_back(state.row, state.col + right, EAST);
            break;
        case WEST:
            for (int left = min_distance; left <= max_distance; ++left)
                moves.emplace_back(state.row + left, state.col, SOUTH);
            for (int right = min_distance; right <= max_distance; ++right)
                moves.emplace_back(state.row - right, state.col, NORTH);
            break;
        case SOUTH:
            for (int left = min_distance; left <= max_distance; ++left)
                moves.emplace_back(state.row, state.col + left, EAST);
            for (int right = min_distance; right <= max_distance; ++right)
                moves.emplace_back(state.row, state.col - right, WEST);
            break;
        case EAST:
            for (int left = min_distance; left <= max_distance; ++left)
                moves.emplace_back(state.row - left, state.col, NORTH);
            for (int right = min_distance; right <= max_distance; ++right)
                moves.emplace_back(state.row + right, state.col, SOUTH);
            break;
        case UNKNOWN:
            for (int up = min_distance; up <= max_distance; ++up)
                moves.emplace_back(state.row - up, state.col, NORTH);
            for (int left = min_distance; left <= max_distance; ++left)
                moves.emplace_back(state.row, state.col - left, WEST);
            for (int down = min_distance; down <= max_distance; ++down)
                moves.emplace_back(state.row + down, state.col, SOUTH);
            for (int right = min_distance; right <= max_distance; ++right)
                moves.emplace_back(state.row, state.col + right, EAST);
            break;
    }

    // Filter out those out of bound
    vector<coord_t> filtered_moves {};
    std::copy_if(moves.begin(), moves.end(), back_inserter(filtered_moves), [max_row, max_col](coord_t coord) {
        return coord.row >= 0 && coord.row < max_row && coord.col >= 0 && coord.col < max_col;
    });

    return filtered_moves;
}

// We want a BFS, from the top left to the bottom right, that has a
// maximum of three consecutive moves in the same direction.
// Adding A-Star will further increase our speed
uint16_t minimise_heat_loss(const vector<string> &grid, int min_moves, int max_moves)
{
    coord_t start {0, 0, UNKNOWN};
    coord_t end {(int16_t) (grid.size() - 1), (int16_t) (grid.at(0).size() - 1), UNKNOWN};
    int max_row = grid.size();
    int max_col = grid.at(0).size();

    priority_queue<pair<uint16_t, coord_t>, vector<pair<uint16_t, coord_t>>, greater<>> coords {};
    coords.emplace(0, start);

    set<coord_t> visited {};
    while (!coords.empty())
    {
        uint16_t heat_loss;
        coord_t current_coord;
        tie(heat_loss, current_coord) = coords.top();
        coords.pop();

        if (current_coord.row == end.row && current_coord.col == end.col)
            return heat_loss;
        if (visited.contains(current_coord))
            continue;
        visited.insert(current_coord);


        vector<coord_t> next_moves = get_moves(current_coord, max_row, max_col, min_moves, max_moves);

        for (auto move: next_moves)
        {
            uint16_t heat_loss_at_move = grid.at(move.row).at(move.col) - '0'; // Here we offset the char number value by the ASCII '0' to get correct number
            switch (move.dir)
            {
                case NORTH:
                    for (int i = current_coord.row - 1; i > move.row; --i)
                        heat_loss_at_move += grid.at(i).at(move.col) - '0';
                    break;
                case WEST:
                    for (int i = current_coord.col - 1; i > move.col; --i)
                        heat_loss_at_move += grid.at(move.row).at(i) - '0';
                    break;
                case SOUTH:
                    for (int i = current_coord.row + 1; i < move.row; ++i)
                        heat_loss_at_move += grid.at(i).at(move.col) - '0';
                    break;
                case EAST:
                    for (int i = current_coord.col + 1; i < move.col; ++i)
                        heat_loss_at_move += grid.at(move.row).at(i) - '0';
                    break;
            }
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
    if (PART_1)
        cout << "The minimised heat loss is: " << minimise_heat_loss(grid, 1, 3) << endl;
    // Part 2
    else
        cout << "The minimised heat loss is: " << minimise_heat_loss(grid, 4, 10) << endl;


    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
