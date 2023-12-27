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

    coord() = default;

    coord(uint64_t row, uint64_t col, TYPES_T type) : row(row), col(col), type(type)
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

void rotate_clockwise(set<coord_t> &rocks, uint64_t max_row, uint64_t max_col, uint64_t rotations)
{
    set<coord_t> new_rocks {};
    switch (rotations)
    {
        case 0:
            return;
        case 1:
            for (const auto &rock: rocks)
                new_rocks.insert({rock.col, max_col - rock.row - 1, rock.type});
            break;
        case 2:
            for (const auto &rock: rocks)
                new_rocks.insert({max_row - rock.row - 1, max_col - rock.col - 1, rock.type});
            break;
        case 3:
            for (const auto &rock: rocks)
                new_rocks.insert({max_row - rock.col - 1, rock.row, rock.type});
            break;
        default:
            cout << "Number of rotations should not exceed 3" << endl;
            exit(1);
    }
    rocks = new_rocks;
}

void tilt_in_direction(set<coord_t> &rocks, const uint64_t max_row, const uint64_t max_col)
{
    set<coord_t> new_rocks {};
    bool first {true};
    uint64_t movable_rocks_count {};
    uint64_t movable_rocks_row {};
    for (auto r_itr = rocks.rbegin(); r_itr != rocks.rend(); r_itr++)
    {
        coord_t current = *r_itr;
        if (current.type == IMMOVABLE)
        {
            if (movable_rocks_row != current.row)
                for (int i = 0; i < movable_rocks_count; ++i)
                    new_rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});
            else
                for (int i = 0; i < movable_rocks_count; ++i)
                    new_rocks.insert({current.row, current.col + i + 1, MOVABLE});

            new_rocks.insert(current);
            movable_rocks_count = 0;
            movable_rocks_row = current.row;
        }
        else
        {
            if (first)
            {
                movable_rocks_row = current.row;
                first = false;
            }
            else if (movable_rocks_row != current.row)
            {
                for (int i = 0; i < movable_rocks_count; ++i)
                    new_rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});
                movable_rocks_count = 0;
                movable_rocks_row = current.row;
            }
            movable_rocks_count++;
        }
    }
    for (int i = 0; i < movable_rocks_count; ++i)
        new_rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});

    // Prepare for the next tilt
    rotate_clockwise(new_rocks, max_row, max_col, 1);


    rocks = {};
    first  = true;
    movable_rocks_count = 0;
    movable_rocks_row = 0;
    for (auto r_itr = new_rocks.rbegin(); r_itr != new_rocks.rend(); r_itr++)
    {
        coord_t current = *r_itr;
        if (current.type == IMMOVABLE)
        {
            if (movable_rocks_row != current.row)
                for (int i = 0; i < movable_rocks_count; ++i)
                    rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});
            else
                for (int i = 0; i < movable_rocks_count; ++i)
                    rocks.insert({current.row, current.col + i + 1, MOVABLE});

            rocks.insert(current);
            movable_rocks_count = 0;
            movable_rocks_row = current.row;
        }
        else
        {
            if (first)
            {
                movable_rocks_row = current.row;
                first = false;
            }
            else if (movable_rocks_row != current.row)
            {
                for (int i = 0; i < movable_rocks_count; ++i)
                    rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});
                movable_rocks_count = 0;
                movable_rocks_row = current.row;
            }
            movable_rocks_count++;
        }
    }
    for (int i = 0; i < movable_rocks_count; ++i)
        rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});

    // Prepare for the next tilt
    rotate_clockwise(rocks, max_row, max_col, 1);

    new_rocks = {};
    first  = true;
    movable_rocks_count = 0;
    movable_rocks_row = 0;
    for (auto r_itr = rocks.rbegin(); r_itr != rocks.rend(); r_itr++)
    {
        coord_t current = *r_itr;
        if (current.type == IMMOVABLE)
        {
            if (movable_rocks_row != current.row)
                for (int i = 0; i < movable_rocks_count; ++i)
                    new_rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});
            else
                for (int i = 0; i < movable_rocks_count; ++i)
                    new_rocks.insert({current.row, current.col + i + 1, MOVABLE});

            new_rocks.insert(current);
            movable_rocks_count = 0;
            movable_rocks_row = current.row;
        }
        else
        {
            if (first)
            {
                movable_rocks_row = current.row;
                first = false;
            }
            else if (movable_rocks_row != current.row)
            {
                for (int i = 0; i < movable_rocks_count; ++i)
                    new_rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});
                movable_rocks_count = 0;
                movable_rocks_row = current.row;
            }
            movable_rocks_count++;
        }
    }
    for (int i = 0; i < movable_rocks_count; ++i)
        new_rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});

    // Prepare for the next tilt
    rotate_clockwise(new_rocks, max_row, max_col, 1);

    rocks = {};
    first  = true;
    movable_rocks_count = 0;
    movable_rocks_row = 0;
    for (auto r_itr = new_rocks.rbegin(); r_itr != new_rocks.rend(); r_itr++)
    {
        coord_t current = *r_itr;
        if (current.type == IMMOVABLE)
        {
            if (movable_rocks_row != current.row)
                for (int i = 0; i < movable_rocks_count; ++i)
                    rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});
            else
                for (int i = 0; i < movable_rocks_count; ++i)
                    rocks.insert({current.row, current.col + i + 1, MOVABLE});

            rocks.insert(current);
            movable_rocks_count = 0;
            movable_rocks_row = current.row;
        }
        else
        {
            if (first)
            {
                movable_rocks_row = current.row;
                first = false;
            }
            else if (movable_rocks_row != current.row)
            {
                for (int i = 0; i < movable_rocks_count; ++i)
                    rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});
                movable_rocks_count = 0;
                movable_rocks_row = current.row;
            }
            movable_rocks_count++;
        }
    }
    for (int i = 0; i < movable_rocks_count; ++i)
        rocks.insert({movable_rocks_row, (uint64_t) i, MOVABLE});

    // Prepare for the next tilt
    rotate_clockwise(rocks, max_row, max_col, 1);
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
    rotate_clockwise(rocks, max_row, max_col, 3); // Init rotate to max north point west
    tilt_in_direction(rocks, max_row, max_col);

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

uint64_t find_north_load_after_cycles(vector<string> &grid, uint64_t cycles)
{
    set<coord_t> rocks = parse_to_rocks(grid);
    uint64_t max_row = grid.size(), max_col = grid.at(0).length();

    map<set<coord_t>, uint64_t> cache;
    bool detected {false};
    rotate_clockwise(rocks, max_row, max_col, 3); // Init it to north pointing west
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

            i = cycles - (remaining % (current - prev));
            uint64_t iteration = cycles - i;
            for (auto [key, val] : cache)
                if (val == iteration + prev)
                {
                    rocks = key;
                    i = cycles;
                    break;
                }
            break;
        }
        else
            cache.insert({rocks, (uint64_t) i});

        //cout << endl;
        //print_rock(rocks, max_row, max_col);
        //cout << endl;
        tilt_in_direction(rocks, max_row, max_col);
        //tilt_in_direction(rocks, max_row, max_col);
        //tilt_in_direction(rocks, max_row, max_col);
        //tilt_in_direction(rocks, max_row, max_col);
    }
    rotate_clockwise(rocks, max_row, max_col, 1); // Correct the init shift of north to west
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
        cout << "The load on the north beam after " << CYCLES << " cycles is: "
             << find_north_load_after_cycles(grid, CYCLES) << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
