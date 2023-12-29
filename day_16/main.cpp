#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace std;

#define DEBUG 0
#define PART_1 false

typedef enum DIRECTIONS
{
    NORTH,
    WEST,
    SOUTH,
    EAST
} DIRECTIONS_T;

typedef struct coord
{
    int row {}, col {};
    DIRECTIONS_T dir {NORTH};

    coord() = default;

    coord(int row, int col, DIRECTIONS_T dir) : row(row), col(col), dir(dir)
    {}

    bool operator<(const coord &rhs) const
    {
        if (row < rhs.row)
            return true;
        if (rhs.row < row)
            return false;
        if (col < rhs.col)
            return true;
        if (rhs.col < col)
            return false;
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
        return row == rhs.row && col == rhs.col && dir == rhs.dir;
    }

    bool operator!=(const coord &rhs) const
    {
        return !(*this == rhs);
    }
} coord_t;

coord_t move_dir(const coord_t &start, const char surface)
{
    coord_t next_coord = start;
    if (surface == '.' || surface == '-' || surface == '|')
    {
        switch (start.dir)
        {
            case NORTH:
                next_coord.row--;
                break;
            case WEST:
                next_coord.col--;
                break;
            case SOUTH:
                next_coord.row++;
                break;
            case EAST:
                next_coord.col++;
                break;
        }
    }
    else if (surface == '/')
    {
        switch (start.dir)
        {
            case NORTH:
                next_coord.col++;
                next_coord.dir = EAST;
                break;
            case WEST:
                next_coord.row++;
                next_coord.dir = SOUTH;
                break;
            case SOUTH:
                next_coord.col--;
                next_coord.dir = WEST;
                break;
            case EAST:
                next_coord.row--;
                next_coord.dir = NORTH;
                break;
        }
    }
    else if (surface == '\\')
    {
        switch (start.dir)
        {
            case NORTH:
                next_coord.col--;
                next_coord.dir = WEST;
                break;
            case WEST:
                next_coord.row--;
                next_coord.dir = NORTH;
                break;
            case SOUTH:
                next_coord.col++;
                next_coord.dir = EAST;
                break;
            case EAST:
                next_coord.row++;
                next_coord.dir = SOUTH;
                break;
        }
    }
    else
    {
        cout << "Should detect all surface types" << endl;
        exit(EXIT_FAILURE);
    }

    return next_coord;
}

bool within_bound(const coord_t &start, int max_row, int max_col)
{
    if (start.row < 0 || start.row >= max_row ||
        start.col < 0 || start.col >= max_col)
        return false;

    return true;
}

void reflect_light(const vector<string> &grid, set<coord_t> &lights, coord_t start)
{
    while (!lights.contains(start))
    {
        if (!within_bound(start, grid.size(), grid.at(0).size()))
            return;
        lights.insert(start);
        auto tile = grid.at(start.row).at(start.col);
        if (tile == '|' && (start.dir % 2) == 1)
        {
            coord_t up = {(uint8_t) (start.row - 1), start.col, NORTH};
            reflect_light(grid, lights, up);
            coord_t down = {(uint8_t) (start.row + 1), start.col, SOUTH};
            reflect_light(grid, lights, down);
            return;
        }
        else if (tile == '-' && (start.dir % 2) == 0)
        {
            coord_t left = {start.row, (uint8_t) (start.col - 1), WEST};
            reflect_light(grid, lights, left);
            coord_t right = {start.row, (uint8_t) (start.col + 1), EAST};
            reflect_light(grid, lights, right);
            return;
        }

        start = move_dir(start, tile);
    }
}

uint64_t find_energised_tiles(const vector<string> &grid)
{
    set<coord_t> lights {};
    reflect_light(grid, lights, {0, 0, EAST});

    uint64_t sum {1};
    coord_t prev = *lights.begin();
    for (auto light: lights)
    {
        if (light.row != prev.row || light.col != prev.col)
            ++sum;
        prev = light;
    }

    return sum;
}

uint64_t find_max_energised_tiles(const vector<string> &grid)
{
    vector<coord_t> starts {};

    for (int row = 0; row < grid.size(); ++row)
    {
        starts.emplace_back(row, 0, EAST);
        starts.emplace_back(row, grid.at(0).size() - 1, WEST);
    }

    for (int col = 0; col < grid.at(0).size(); ++col)
    {
        starts.emplace_back(0, col, SOUTH);
        starts.emplace_back(grid.size() - 1, col, NORTH);
    }

    uint64_t best_so_far {};

    for (auto start: starts)
    {
        set<coord_t> lights {};
        reflect_light(grid, lights, start);

        uint64_t sum {1};
        coord_t prev = *lights.begin();
        for (auto light: lights)
        {
            if (light.row != prev.row || light.col != prev.col)
                ++sum;
            prev = light;
        }

        if (sum > best_so_far)
            best_so_far = sum;
    }

    return best_so_far;
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
        cout << "Number of energised tiles is: " << find_energised_tiles(grid) << endl;
        // Part 2
    else
        cout << "The number of energised tiles, maximised, is: " << find_max_energised_tiles(grid) << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
