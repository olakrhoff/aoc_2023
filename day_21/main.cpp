#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

#define DEBUG 1
#define PART_1 false
#define DEBUG_STEPS 6
#define STEPS 64
#define STEPS_PART_2 26501365
const int OFFSET = STEPS_PART_2 * 2;

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

vector<pair<bool, coord_t>> parse_line_to_coord(const vector<string> &grid)
{
    vector<pair<bool, coord_t>> coords {};
    for (int row = 0; row < grid.size(); ++row)
        for (int col = 0; col < grid.at(row).length(); ++col)
            switch (grid.at(row).at(col))
            {
                case '#':
                    coords.emplace_back(false, coord_t(row + OFFSET, col + OFFSET));
                    break;
                case 'S':
                    coords.emplace_back(true, coord_t(row + OFFSET, col + OFFSET));
                    break;
            }
    
    return coords;
}

vector<coord_t> possible_places(const coord_t &current)
{
    vector<coord_t> new_places {};
    
    new_places.emplace_back(current.row - 1, current.col);
    new_places.emplace_back(current.row, current.col - 1);
    new_places.emplace_back(current.row + 1, current.col);
    new_places.emplace_back(current.row, current.col + 1);
    
    return new_places;
}

uint64_t find_number_of_garden_tiles(const set<coord_t> &grid, coord_t start)
{
    set<coord_t> places {};
    places.emplace(start);
    for (int step = 0; step < (DEBUG ? DEBUG_STEPS : STEPS); ++step)
    {
        set<coord_t> new_places {};
        for (auto place: places)
            for (auto new_place: possible_places(place))
                if (!grid.contains(new_place))
                    new_places.emplace(new_place);
        
        places = new_places;
    }
    
    return places.size();
}

coord_t transform_to_grid(const coord_t &current, int grid_width, int grid_height)
{
    coord_t transformed_coord = current;
    
    transformed_coord.row %= grid_height;
    transformed_coord.col %= grid_width;
    
    transformed_coord.row += OFFSET;
    transformed_coord.col += OFFSET;
    
    return transformed_coord;
}

int64_t round_to_nearest_num(int64_t num, int64_t mod, bool up)
{
    if (num % mod == 0)
        return num;
    
    if (num < 0)
        num -= mod;
    
    int temp = num / mod;
    int offset = 0;
    if (up)
        offset = 1;
    
    return (temp + offset) * mod;
}

void print_terrain(const set<coord_t> &grid, int grid_width, int grid_height, const set<coord_t> &tiles)
{
    
    int64_t max_row = max_element(tiles.begin(), tiles.end(), [](const coord_t &a, const coord_t &b)
    {
        return a.row < b.row;
    })->row;
    max_row = round_to_nearest_num(max_row, grid_height, true);
    int64_t max_col = max_element(tiles.begin(), tiles.end(), [](const coord_t &a, const coord_t &b)
    {
        return a.col < b.col;
    })->col;
    max_col = round_to_nearest_num(max_col, grid_width, true);
    int64_t min_row = max_element(tiles.begin(), tiles.end(), [](const coord_t &a, const coord_t &b)
    {
        return a.row > b.row;
    })->row;
    min_row = round_to_nearest_num(min_row, grid_height, false);
    int64_t min_col = max_element(tiles.begin(), tiles.end(), [](const coord_t &a, const coord_t &b)
    {
        return a.col > b.col;
    })->col;
    min_col = round_to_nearest_num(min_col, grid_width, false);
    
    
    for (int row = min_row; row < max_row; ++row)
    {
        for (int col = min_col; col < max_col; ++col)
        {
            if (grid.contains(transform_to_grid(coord_t(row, col), grid_width, grid_height)))
                cout << "#";
            else if (tiles.contains(coord_t(row, col)))
                cout << "O";
            else
                cout << ".";
        }
        cout << endl;
        
    }
}

void print_grid(const set<coord_t> &grid, int grid_width, int grid_height, const set<coord_t> &tiles)
{
    for (int row = 0; row < grid_height; ++row)
    {
        for (int col = 0; col < grid_width; ++col)
        {
            if (grid.contains(coord_t(row + OFFSET, col + OFFSET)))
                cout << "#";
            else if (tiles.contains(coord_t(row + OFFSET, col + OFFSET)))
                cout << "O";
            else
                cout << ".";
        }
        cout << endl;
        
    }
}

bool is_in_ignore_area(const coord_t &current, const vector<pair<coord_t, coord_t>> &ignores)
{
    for (auto [top_left, bot_right]: ignores)
        if (current.row >= top_left.row &&
            current.row <= bot_right.row &&
            current.col >= top_left.col &&
            current.col <= top_left.col)
            return true;
    
    return false;
}

void update_ignore_area(const set<coord_t> &places, const set<coord_t> &grid, vector<pair<coord_t, coord_t>> &ignores, const int grid_width,
                        const int grid_height)
{
    // If there are no ignore areas yet, we check if the middle is ready to become one
    if (ignores.empty())
    {
        // We assume that input does not have '#' in upper left corner, or to the right of it
        bool is_even = places.contains({OFFSET, OFFSET});
        
        bool is_odd = places.contains({OFFSET, OFFSET + 1});
        
        if (!is_even && !is_odd)
            return; // The gird is not filled up
        
        if (is_even)
        {
            for (int row = OFFSET; row < grid_height + OFFSET; ++row)
            {
                for (int col = OFFSET; col < grid_width + OFFSET; ++col)
                {
                    if ((row + col) % 2 == 0)
                    {
                        if (!places.contains({row, col}) && !grid.contains({row, col}))
                            return; // Grid is not ready
                        ++col;
                    }
                }
            }
        }
        else
        {
            for (int row = OFFSET; row < grid_height + OFFSET; ++row)
            {
                for (int col = OFFSET; col < grid_width + OFFSET; ++col)
                {
                    if ((row + col) % 2 != 0)
                    {
                        if (!places.contains({row, col}) && !grid.contains({row, col}))
                            return; // Grid is not ready
                        ++col;
                    }
                }
            }
        }
        // The entire start gird is full, it is now ready to be ignored
        ignores.emplace_back(make_pair<coord_t, coord_t>({OFFSET, OFFSET},
                                                         {OFFSET + grid_height - 1, OFFSET + grid_width - 1}));
        return;
    }
    
    return;
    cout << "unhandled" << endl;
    exit(EXIT_FAILURE);
}

uint64_t find_number_of_garden_tiles_in_infinite_terrain(const set<coord_t> &grid, const int grid_width,
                                                         const int grid_height, coord_t start)
{
    set<coord_t> places {};
    places.emplace(start);
    vector<pair<coord_t, coord_t>> ignores {};
    for (int step = 0; step < STEPS_PART_2; ++step)
    {
        if (step == 6 ||
            step == 10 ||
            step == 50 ||
            step == 100 ||
            step == 500 ||
            step == 1000 ||
            step == 5000)
            cout << places.size() << endl;
        cout << endl;
        //print_terrain(grid, grid_width, grid_height, places);
        print_grid(grid, grid_width, grid_height, places);
        
        set<coord_t> new_places {};
        for (auto place: places)
            for (auto new_place: possible_places(place))
                if (!grid.contains(transform_to_grid(new_place, grid_width, grid_height)) &&
                    !is_in_ignore_area(new_place, ignores))
                    new_places.emplace(new_place);
        
        update_ignore_area(new_places, grid, ignores, grid_width, grid_height);
        
        places = new_places;
    }
    
    return places.size();
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
    set<coord_t> grid;
    vector<string> temp_grid {};
    coord_t start;
    while (!file.eof())
    {
        getline(file, temp);
        temp_grid.emplace_back(temp);
    }
    
    file.close();
    
    for (auto [is_start, coord]: parse_line_to_coord(temp_grid))
    {
        if (is_start)
            start = coord;
        else
            grid.emplace(coord);
    }
    int width = temp_grid.at(0).size(), height = temp_grid.size();
    
    // Part 1
    if (PART_1)
        cout << "The total number of garden tiles that is reachable is: " << find_number_of_garden_tiles(grid, start)
             << endl;
        // Part 2
    else
        cout << "The total number of garden tiles reachable in infinite terrain is: "
             << find_number_of_garden_tiles_in_infinite_terrain(grid, width, height, start) << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
