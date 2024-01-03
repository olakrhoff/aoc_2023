#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

#define DEBUG 1

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

    coord() = default;

    coord(int row, int col, DIRECTIONS_T dir) : row(row), col(col), dir(dir)
    {}

    bool operator<(const coord &rhs) const
    {
        if (row < rhs.row)
            return true;
        if (rhs.row < row)
            return false;
        return col < rhs.col;
        /*
        if (col < rhs.col)
            return true;
        if (rhs.col < col)
            return false;
        return dir < rhs.dir;
         */
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
        return row == rhs.row && col == rhs.col;
    }

    bool operator!=(const coord &rhs) const
    {
        return !(*this == rhs);
    }
} coord_t;

vector<coord_t> get_moves(vector<pair<coord_t, uint64_t>> state, int max_row, int max_col)
{
    vector<coord_t> moves {};

    coord_t current = state.back().first;

    // First we find all neighbours of the grid cell, that does not move in the direction we came from
    if (current.dir != SOUTH)
        moves.emplace_back(current.row - 1, current.col, NORTH);
    if (current.dir != EAST)
        moves.emplace_back(current.row, current.col - 1, WEST);
    if (current.dir != NORTH)
        moves.emplace_back(current.row + 1, current.col, SOUTH);
    if (current.dir != WEST)
        moves.emplace_back(current.row, current.col + 1, EAST);

    // Filter out those out of bound
    vector<coord_t> filtered_moves {};
    std::copy_if(moves.begin(), moves.end(), back_inserter(filtered_moves), [max_row, max_col](coord_t coord) {
        return coord.row >= 0 && coord.row < max_row && coord.col >= 0 && coord.col < max_col;
    });


    if (state.size() < 3)
        return filtered_moves;

    // We now enforce the max three in a row rule
    bool singular_direction = true;
    /*for (int i = 1; i < state.size(); ++i)
        if (state.at(i - 1).first.dir != state.at(i).first.dir)
            singular_direction = false;
    */
    for (int i = state.size() - 2; i > state.size() - 4; --i)
        if (state.back().first.dir != state.at(i).first.dir)
            singular_direction = false;
    if (singular_direction)
    {
        DIRECTIONS_T direction = state.back().first.dir;
        for (auto itr = filtered_moves.begin(); itr != filtered_moves.end(); itr++)
        {
            if ((*itr).dir == direction)
            {
                filtered_moves.erase(itr);
                break;
            }
        }
    }

    return filtered_moves;
}

void print_heap(vector<vector<pair<coord_t, uint64_t>>> &vc)
{
    for (auto el: vc)
    {
        auto i = el.back();
        string dir;
        switch (i.first.dir)
        {
            case NORTH:
                dir = "NORTH";
                break;
            case WEST:
                dir = "WEST";
                break;
            case SOUTH:
                dir = "SOUTH";
                break;
            case EAST:
                dir = "EAST";
                break;
            case UNKNOWN:
                dir = "N/A";
                break;
        }
        cout << i.first.row << ", " << i.first.col << ", " << dir << " : " << i.second << endl;
    }
}

void fix_heap(vector<vector<pair<coord_t, uint64_t>>> &heap, bool (*comp)(vector<pair<coord_t, uint64_t>> &, vector<pair<coord_t, uint64_t>> &), int index = 0)
{
    // If we are at the top we stop
    if (index <= 0)
        return;
    vector<pair<coord_t, uint64_t>> current = heap.at(index);
    int parent_index = (index - 1) / 2;
    vector<pair<coord_t, uint64_t>> parent = heap.at(parent_index);

    // If current is less than the parent
    if (comp(current, parent))
    {
        heap.at(parent_index) = current;
        heap.at(index) = parent;
        index = parent_index;
    }
    else
    {
        // If we are a left child, we skip this step

    }
}

void print_path(const vector<string> &grid, const vector<pair<coord_t, uint64_t>> &path)
{
    for (int i = 0; i < grid.size(); ++i)
    {
        for (int j = 0; j < grid.at(i).size(); ++j)
        {
            bool found = false;
            DIRECTIONS_T dir;
            for (auto val : path)
                if (val.first == coord_t(i, j, UNKNOWN))
                {
                    found = true;
                    dir = val.first.dir;
                    break;
                }

            if (found)
            {
                switch (dir)
                {
                    case UNKNOWN:
                        cout << "*";
                        break;
                    case NORTH:
                        cout << "^";
                        break;
                    case WEST:
                        cout << "<";
                        break;
                    case SOUTH:
                        cout << "v";
                        break;
                    case EAST:
                        cout << ">";
                        break;
                }
            }
            else
                cout << grid.at(i).at(j);
        }
        cout << endl;
    }
}

// We want a BFS, from the top left to the bottom right, that has a
// maximum of three consecutive moves in the same direction
uint64_t minimise_heat_loss(const vector<string> &grid)
{
    coord_t start {0, 0, UNKNOWN};
    coord_t end {(int) (grid.size() - 1), (int) (grid.at(0).size() - 1), UNKNOWN};
    int max_row = grid.size();
    int max_col = grid.at(0).size();

    auto comp = [](vector<pair<coord_t, uint64_t>> &a, vector<pair<coord_t, uint64_t>> &b) {
        return a.back().second > b.back().second;
    };

    vector<vector<pair<coord_t, uint64_t>>> coords {};
    std::make_heap(coords.begin(), coords.end());
    vector<pair<coord_t, uint64_t>> s;
    s.emplace_back(start, 0);
    coords.emplace_back(s);
    std::push_heap(coords.begin(), coords.end(), comp);

    set<coord_t> visited {};
    while (!coords.empty())
    {
        vector<pair<coord_t, uint64_t>> current = coords.front();
        std::pop_heap(coords.begin(), coords.end(), comp);
        coords.pop_back();
        if (current.back().first.row == end.row && current.back().first.col == end.col)
        {
            cout << endl;
            print_path(grid, current);
            return current.back().second;
        }
        if (visited.contains(current.back().first))
            continue;
        visited.insert(current.back().first);


        uint64_t heat_loss = current.back().second;

        vector<coord_t> next_moves = get_moves(current, max_row, max_col);

        for (auto move: next_moves)
        {
            vector<pair<coord_t, uint64_t>> next;
            uint64_t heat_loss_at_move = grid.at(move.row).at(move.col) -
                                         '0'; // Here we offset the char number value by the ASCII '0' to get correct number
            int start_index {1};
            if (current.size() < 3)
                start_index = 0;
            next = current;
            next.emplace_back(move, heat_loss + heat_loss_at_move);
            coords.emplace_back(next);
            push_heap(coords.begin(), coords.end(), comp);
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
