#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

using namespace std;

#define DEBUG 0
#define PART_1 false
#define CYCLES 1000000000

typedef enum TYPES
{
    MOVABLE,
    IMMOVABLE
} TYPES_T;

typedef struct coord
{
    uint64_t row {}, col {};
    TYPES_T type {MOVABLE};

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
        return type < rhs.type;
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
        return row == rhs.row && col == rhs.col && type == rhs.type;
    }

    bool operator!=(const coord &rhs) const
    {
        return !(*this == rhs);
    }
} coord_t;

typedef enum DIRECTIONS
{
    NORTH,
    WEST,
    SOUTH,
    EAST
} DIRECTIONS_T;

void rotate_counter_clockwise(set<coord_t> &rocks, uint64_t max_row, uint64_t max_col)
{
    set<coord_t> new_rocks {};
    for (auto itr = rocks.begin(); itr != rocks.end(); itr++)
        new_rocks.insert({(*itr).col, max_col - (*itr).row - 1, (*itr).type});

    rocks = new_rocks;
}

void tilt_in_direction(set<coord_t> &rocks, const uint64_t max_row, const uint64_t max_col, DIRECTIONS_T dir)
{
    for (int i = 0; i < (dir + 3) % 4; ++i)
        rotate_counter_clockwise(rocks, max_row, max_col);

    set<coord_t> new_rocks {};
    for (int row = 0; row < max_row; ++row)
    {
        uint64_t movable_rocks_count {};
        for (int col = max_col; col >= 0; --col)
        {
            if (rocks.contains({(uint64_t)row, (uint64_t)col, IMMOVABLE}))
            {
                for (int i = 0; i < movable_rocks_count; ++i)
                {
                    new_rocks.insert({(uint64_t)row, (uint64_t)col + i + 1, MOVABLE});
                }
                new_rocks.insert({(uint64_t)row, (uint64_t)col, IMMOVABLE});
                movable_rocks_count = 0;
            }
            else if (rocks.contains({(uint64_t)row, (uint64_t)col, MOVABLE}))
                movable_rocks_count++;
        }
        for (int i = 0; i < movable_rocks_count; ++i)
            new_rocks.insert({(uint64_t)row, (uint64_t)i, MOVABLE});
    }

    rocks = new_rocks;

    for (int i = 0; i < (8 - (dir + 3)) % 4; ++i)
        rotate_counter_clockwise(rocks, max_row, max_col);
}

set<coord_t> parse_to_rocks(const vector<string> &grid)
{
    set<coord_t> rocks {};
    for (int row = 0; row < grid.size(); ++row)
    {
        for (int col = 0; col < grid.at(row).length(); ++col)
        {
            if (grid.at(row).at(col) == '.')
                continue;
            coord_t new_coord;
            new_coord.row = row;
            new_coord.col = col;

            if (grid.at(row).at(col) == 'O')
                new_coord.type = MOVABLE;
            else
                new_coord.type = IMMOVABLE;

            rocks.insert(new_coord);
        }
    }

    return rocks;
}

uint64_t find_north_load(vector<string> &grid)
{
    set<coord_t> rocks = parse_to_rocks(grid);
    uint64_t max_row = grid.size(), max_col = grid.at(0).length();

    tilt_in_direction(rocks, max_row, max_col, NORTH);

    uint64_t load {};

    for (auto itr = rocks.begin(); itr != rocks.end(); itr++)
        if ((*itr).type == MOVABLE)
            load += max_col - (*itr).row;

    return load;
}

void print_rock(const set<coord_t> &rocks, uint64_t max_row, uint64_t max_col)
{
    for (uint64_t row = 0; row < max_row; ++row)
    {
        for (uint64_t col = 0; col < max_col; ++col)
        {
            if (rocks.contains({row, col, MOVABLE}))
                cout << 'O';
            else if (rocks.contains({row, col, IMMOVABLE}))
                cout << '#';
            else
                cout << '.';
        }
        cout << endl;
    }
}

const set<coord_t> SOLUTION = {
        {0, 5, IMMOVABLE},

        {1, 4, IMMOVABLE},
        {1, 8, MOVABLE},
        {1, 9, IMMOVABLE},

        {2, 5, IMMOVABLE},
        {2, 6, IMMOVABLE},

        {3, 3, IMMOVABLE},

        {4, 5, MOVABLE},
        {4, 6, MOVABLE},
        {4, 7, MOVABLE},
        {4, 8, IMMOVABLE},

        {5, 1, MOVABLE},
        {5, 2, IMMOVABLE},
        {5, 6, MOVABLE},
        {5, 7, IMMOVABLE},
        {5, 9, IMMOVABLE},

        {6, 4, MOVABLE},
        {6, 5, IMMOVABLE},
        {6, 9, MOVABLE},

        {7, 6, MOVABLE},
        {7, 7, MOVABLE},
        {7, 8, MOVABLE},
        {7, 9, MOVABLE},

        {8, 0, IMMOVABLE},
        {8, 5, IMMOVABLE},
        {8, 6, IMMOVABLE},
        {8, 7, IMMOVABLE},
        {8, 9, MOVABLE},

        {9, 0, IMMOVABLE},
        {9, 2, MOVABLE},
        {9, 3, MOVABLE},
        {9, 4, MOVABLE},
        {9, 5, IMMOVABLE},
        {9, 8, MOVABLE},
        {9, 9, MOVABLE},
};

uint64_t find_north_load_after_cycles(vector<string> &grid, uint64_t cycles)
{
    set<coord_t> rocks = parse_to_rocks(grid);
    uint64_t max_row = grid.size(), max_col = grid.at(0).length();

    map<set<coord_t>, uint64_t> cache;
    bool detected {false};
    for (int i = 0; i < cycles; ++i)
    {
        if (cache.contains(rocks) && !detected)
        {
            // We have found a previous condition, skip as much as possible
            detected = true;
            auto entry = cache.find(rocks);
            uint64_t current = i;
            uint64_t prev = entry->second;
            uint64_t remaining = cycles - prev;
            uint64_t start_of_loop = prev;
            i = cycles - (remaining % (current - prev));
        }
        else
            cache.insert({rocks, (uint64_t)i});
        //print_rock(rocks, max_row, max_col);
        //cout << endl;
//        for (int c = 0; c < 4; ++c)
  //      {
            //print_rock(rocks, max_row, max_col);
            //cout << endl;
            tilt_in_direction(rocks, max_row, max_col, NORTH);
            tilt_in_direction(rocks, max_row, max_col, WEST);
            tilt_in_direction(rocks, max_row, max_col, SOUTH);
            tilt_in_direction(rocks, max_row, max_col, EAST);
    //    }
    }
    //print_rock(rocks, max_row, max_col);
    uint64_t load {};

    for (auto itr = rocks.begin(); itr != rocks.end(); itr++)
        if ((*itr).type == MOVABLE)
            load += max_col - (*itr).row;

    return load;
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
        cout << "The load on the north beam is: " << find_north_load(grid) << endl;
    // Part 2
    else
        cout << "The load on the north beam after " << CYCLES << " cycles is: " << find_north_load_after_cycles(grid, CYCLES) << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
