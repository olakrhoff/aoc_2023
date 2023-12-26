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
    MOVEABLE,
    IMMOVEABLE
} TYPES_T;

typedef struct coord
{
    uint64_t row {}, col {};
    TYPES_T type {MOVEABLE};

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

void tilt_in_direction(set<coord_t> &rocks, uint64_t max_row, uint64_t max_col, DIRECTIONS_T dir)
{
    bool changed {false};
    switch (dir)
    {
        case NORTH:
            do
            {
                changed = false;
                for (auto itr = rocks.begin(); itr != rocks.end(); itr++)
                {
                    coord_t current = (*itr);
                    if (current.row == 0 || current.type == IMMOVEABLE)
                        continue;

                    coord_t temp = current;
                    temp.row--;
                    while (rocks.find(temp) == rocks.end() && rocks.find({temp.row, temp.col, IMMOVEABLE}) == rocks.end())
                    {
                        if (temp.row == 0)
                            break;
                        temp.row--;
                    }
                    if (rocks.find(temp) != rocks.end() || rocks.find({temp.row, temp.col, IMMOVEABLE}) != rocks.end())
                        temp.row++;

                    if (current == temp)
                        continue;

                    itr = rocks.erase(itr);
                    rocks.insert(temp);
                    changed = true;

                    if (itr == rocks.end())
                        break;
                }
            } while (changed);
            break;
        case WEST:
            do
            {
                changed = false;
                for (auto itr = rocks.begin(); itr != rocks.end(); itr++)
                {
                    coord_t current = (*itr);
                    if (current.col == 0 || current.type == IMMOVEABLE)
                        continue;

                    coord_t temp = current;
                    temp.col--;
                    while (rocks.find(temp) == rocks.end() && rocks.find({temp.row, temp.col, IMMOVEABLE}) == rocks.end())
                    {
                        if (temp.col == 0)
                            break;
                        temp.col--;
                    }
                    if (rocks.find(temp) != rocks.end() || rocks.find({temp.row, temp.col, IMMOVEABLE}) != rocks.end())
                        temp.col++;

                    if (current == temp)
                        continue;

                    itr = rocks.erase(itr);
                    rocks.insert(temp);
                    changed = true;

                    if (itr == rocks.end())
                        break;
                }
            } while (changed);
            break;
        case SOUTH:
            do
            {
                changed = false;
                for (auto itr = rocks.begin(); itr != rocks.end(); itr++)
                {
                    coord_t current = (*itr);
                    if (current.row == max_row - 1 || current.type == IMMOVEABLE)
                        continue;

                    coord_t temp = current;
                    temp.row++;
                    while (rocks.find(temp) == rocks.end() && rocks.find({temp.row, temp.col, IMMOVEABLE}) == rocks.end())
                    {
                        if (temp.row == max_row - 1)
                            break;
                        temp.row++;
                    }
                    if (rocks.find(temp) != rocks.end() || rocks.find({temp.row, temp.col, IMMOVEABLE}) != rocks.end())
                        temp.row--;

                    if (current == temp)
                        continue;

                    itr = rocks.erase(itr);
                    rocks.insert(temp);
                    changed = true;

                    if (itr == rocks.end())
                        break;
                }
            } while (changed);
            break;
        case EAST:
            do
            {
                changed = false;
                for (auto itr = rocks.begin(); itr != rocks.end(); itr++)
                {
                    coord_t current = (*itr);
                    if (current.col == max_col - 1 || current.type == IMMOVEABLE)
                        continue;

                    coord_t temp = current;
                    temp.col++;
                    while (rocks.find(temp) == rocks.end() && rocks.find({temp.row, temp.col, IMMOVEABLE}) == rocks.end())
                    {
                        if (temp.col == max_col - 1)
                            break;
                        temp.col++;
                    }
                    if (rocks.find(temp) != rocks.end() || rocks.find({temp.row, temp.col, IMMOVEABLE}) != rocks.end())
                        temp.col--;

                    if (current == temp)
                        continue;

                    itr = rocks.erase(itr);
                    rocks.insert(temp);
                    changed = true;

                    if (itr == rocks.end())
                        break;
                }
            } while (changed);
            break;
    }
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
                new_coord.type = MOVEABLE;
            else
                new_coord.type = IMMOVEABLE;

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
        if ((*itr).type == MOVEABLE)
            load += max_col - (*itr).row;

    return load;
}

void print_rock(const set<coord_t> &rocks, uint64_t max_row, uint64_t max_col)
{
    for (uint64_t row = 0; row < max_row; ++row)
    {
        for (uint64_t col = 0; col < max_col; ++col)
        {
            if (rocks.contains({row, col, MOVEABLE}))
                cout << 'O';
            else if (rocks.contains({row, col, IMMOVEABLE}))
                cout << '#';
            else
                cout << '.';
        }
        cout << endl;
    }
}

const set<coord_t> SOLUTION = {
        {0, 5, IMMOVEABLE},

        {1, 4, IMMOVEABLE},
        {1, 8, MOVEABLE},
        {1, 9, IMMOVEABLE},

        {2, 5, IMMOVEABLE},
        {2, 6, IMMOVEABLE},

        {3, 3, IMMOVEABLE},

        {4, 5, MOVEABLE},
        {4, 6, MOVEABLE},
        {4, 7, MOVEABLE},
        {4, 8, IMMOVEABLE},

        {5, 1, MOVEABLE},
        {5, 2, IMMOVEABLE},
        {5, 6, MOVEABLE},
        {5, 7, IMMOVEABLE},
        {5, 9, IMMOVEABLE},

        {6, 4, MOVEABLE},
        {6, 5, IMMOVEABLE},
        {6, 9, MOVEABLE},

        {7, 6, MOVEABLE},
        {7, 7, MOVEABLE},
        {7, 8, MOVEABLE},
        {7, 9, MOVEABLE},

        {8, 0, IMMOVEABLE},
        {8, 5, IMMOVEABLE},
        {8, 6, IMMOVEABLE},
        {8, 7, IMMOVEABLE},
        {8, 9, MOVEABLE},

        {9, 0, IMMOVEABLE},
        {9, 2, MOVEABLE},
        {9, 3, MOVEABLE},
        {9, 4, MOVEABLE},
        {9, 5, IMMOVEABLE},
        {9, 8, MOVEABLE},
        {9, 9, MOVEABLE},
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
        for (int c = 0; c < 4; ++c)
            tilt_in_direction(rocks, max_row, max_col, (DIRECTIONS_T)c);
    }
    print_rock(rocks, max_row, max_col);
    uint64_t load {};

    for (auto itr = rocks.begin(); itr != rocks.end(); itr++)
        if ((*itr).type == MOVEABLE)
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
