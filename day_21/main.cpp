#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

#define DEBUG 0
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
    
    coord operator+(const coord &rhs) const
    {
        return {this->row + rhs.row, this->col + rhs.col};
    }
    
    coord operator-(const coord &rhs) const
    {
        return {this->row - rhs.row, this->col - rhs.col};
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

inline vector<coord_t> possible_places(const coord_t &current)
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

bool is_in_ignore_area(const coord_t &current, const set<coord_t> &ignores, int grid_width, int grid_height)
{
    // Transform to upper left corner of tile
    // Do binary search to see if corner is in ignores
    coord_t corner = current;
    if (corner.row % grid_height != 0)
    {
        int offset = 1;
        if (corner.row < 0)
            offset = -1;
        corner.row = ((corner.row / grid_height) + offset) * grid_height - offset;
    }
    if (corner.col % grid_width != 0)
    {
        int offset = 1;
        if (corner.col < 0)
            offset = -1;
        corner.col = ((corner.col / grid_width) + offset) * grid_width - offset;
    }
    
    return ignores.contains(corner);
    
    /*
    for (auto [top_left, bot_right]: ignores)
        if (current.row >= top_left.row &&
            current.row <= bot_right.row &&
            current.col >= top_left.col &&
            current.col <= bot_right.col)
            return true;
    
    return false;
    */
}

// Maps a coord in the total grid to a grid coord
coord_t get_coord_in_grid(const coord_t &current, const int grid_width, const int grid_height)
{
    int64_t row = current.row - OFFSET;
    int64_t col = current.col - OFFSET;
    
    coord_t coord_in_grid(row / grid_height, col / grid_width);
    if (row % grid_height != 0)
        coord_in_grid.row--;
    if (col % grid_width != 0)
        coord_in_grid.col--;
    
    return coord_in_grid;
}

coord_t area_from_coord(const coord_t &current, const int grid_width, const int grid_height)
{
    coord_t upper_left(OFFSET + grid_height * current.row, OFFSET + grid_width * current.col);
    //coord_t bottom_right(upper_left.row + grid_height - 1, upper_left.col + grid_width - 1);
    
    return upper_left;
}

bool is_area_full(const coord_t &current, const set<coord_t> &places, const set<coord_t> &grid, const int grid_width, const int grid_height)
{
    // We assume that input does not have '#' in upper left corner, or to the right of it
    bool is_even = places.contains(current);
    
    bool is_odd = places.contains({current.row, current.col + 1});
    
    if (!is_even && !is_odd)
        return false; // The gird is not filled up
    
    if (is_even)
    {
        for (int64_t row = current.row; row < current.row + grid_height; ++row)
        {
            for (int64_t col = current.col; col < current.col + grid_width; ++col)
            {
                if ((row + col) % 2 == 0)
                {
                    if (!places.contains({row, col}) && !grid.contains({(row % grid_height) + OFFSET, (col % grid_width) + OFFSET}))
                        return false; // Grid is not ready
                    ++col;
                }
            }
        }
    }
    else // is odd
    {
        for (int64_t row = current.row; row < current.row + grid_height; ++row)
        {
            for (int64_t col = current.col; col < current.col + grid_width; ++col)
            {
                if ((row + col) % 2 != 0)
                {
                    if (!places.contains({row, col}) && !grid.contains({(row % grid_height) + OFFSET, (col % grid_width) + OFFSET}))
                        return false; // Grid is not ready
                    ++col;
                }
            }
        }
    }
    
    return true;
}

int update_ignore_area(const set<coord_t> &places, const set<coord_t> &grid, set<coord_t> &ignores,
                       const int grid_width, const int grid_height)
{
    // If there are no ignore areas yet, we check if the middle is ready to become one
    if (ignores.empty())
    {
        if (!is_area_full({OFFSET, OFFSET}, places, grid, grid_width, grid_height))
            return 0;
        
        // The entire start gird is full, it is now ready to be ignored
        ignores.insert({OFFSET, OFFSET});
        return 1;
    }
    
    // If we already have an ignore area we need to check the ones surrounding the current areas, this is not guaranteed to be the fastest, but a strong heuristic (assumption) that it is
    
    // First generate a set of all possibilities
    set<coord_t> possible_areas {}; // These are the coords of the grid sized area in the grid og grids
    
    for (auto upper_left: ignores)
    {
        coord_t current_grid_coord = get_coord_in_grid(upper_left, grid_width, grid_height);
        
        coord_t outer_upper_left_coord = current_grid_coord - coord_t(1, 1);
        coord_t outer_bottom_right_coord = current_grid_coord + coord_t(1, 1);
        
        // Add top and bottom side
        for (int64_t col = outer_upper_left_coord.col; col <= outer_bottom_right_coord.col; ++col)
        {
            if (!is_in_ignore_area(area_from_coord({outer_upper_left_coord.row, col}, grid_width, grid_height), ignores, grid_width, grid_height))
                possible_areas.insert({outer_upper_left_coord.row, col});
            if (!is_in_ignore_area(area_from_coord({outer_bottom_right_coord.row, col}, grid_width, grid_height), ignores, grid_width, grid_height))
                possible_areas.insert({outer_bottom_right_coord.row, col});
        }
        // Add left and right side
        for (int64_t row = outer_upper_left_coord.row; row <= outer_bottom_right_coord.row; ++row)
        {
            if (!is_in_ignore_area(area_from_coord({row, outer_upper_left_coord.col}, grid_width, grid_height), ignores, grid_width, grid_height))
                possible_areas.insert({row, outer_upper_left_coord.col});
            if (!is_in_ignore_area(area_from_coord({row, outer_bottom_right_coord.col}, grid_width, grid_height), ignores, grid_width, grid_height))
                possible_areas.insert({row, outer_bottom_right_coord.col});
        }
    }
    
    // Check all possibilities, if new area, add it
    vector<coord_t> new_ignores {};
    for (auto area: possible_areas)
        if (is_area_full(area_from_coord(area, grid_width, grid_height), places, grid, grid_width, grid_height))
            new_ignores.emplace_back(area_from_coord(area, grid_width, grid_height));
    
    for (auto ignore : new_ignores)
        ignores.insert(ignore);
    
    return new_ignores.size();
    
    // If we added any, try and fuse the new area(s) together with the other areas to limit the number of freestanding areas
    // TODO: Add this
}

uint64_t find_area_value(const set<coord_t> &grid, const int grid_width,
                         const int grid_height, bool is_even)
{
    uint64_t sum {};
    
    if (is_even)
    {
        for (int row = OFFSET; row < grid_height + OFFSET; ++row)
        {
            for (int col = OFFSET; col < grid_width + OFFSET; ++col)
            {
                if ((row + col) % 2 == 0)
                {
                    if (!grid.contains({row, col}))
                        sum++;
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
                    if (!grid.contains({row, col}))
                        sum++;
                    ++col;
                }
            }
        }
    }
    
    return sum;
}

uint64_t find_number_of_garden_tiles_in_infinite_terrain(const set<coord_t> &grid, const int grid_width,
                                                         const int grid_height, coord_t start, uint64_t steps = STEPS_PART_2)
{
    uint64_t odd_area_value = find_area_value(grid, grid_width, grid_height, false);
    uint64_t even_area_value = find_area_value(grid, grid_width, grid_height, true);
    
    set<coord_t> places {};
    places.emplace(start);
    set<coord_t> ignores {};
    int full_areas_odd {}, full_areas_even {};
    for (int step = 0; step < steps; ++step)
    {
        if (step == 6 ||
            step == 10 ||
            step == 50 ||
            step == 100 ||
            step == 500 ||
            step == 1000 ||
            step == 5000)
            cout << places.size() + full_areas_odd * odd_area_value + full_areas_even * even_area_value << endl;
        //cout << endl;
        //print_terrain(grid, grid_width, grid_height, places);
        //print_grid(grid, grid_width, grid_height, places);
        
        set<coord_t> new_places {};
        for (auto place: places)
            for (auto new_place: possible_places(place))
                if (!grid.contains(transform_to_grid(new_place, grid_width, grid_height)) &&
                    !is_in_ignore_area(new_place, ignores, grid_width, grid_height))
                    new_places.emplace(new_place);
        
        int new_areas = update_ignore_area(new_places, grid, ignores, grid_width, grid_height);
        
        if (step % 2 == 0)
            full_areas_even += new_areas;
        else
            full_areas_odd += new_areas;
        
        places = new_places;
    }
    
    return places.size();
}

uint64_t solve_part_2(const set<coord_t> &grid, const int grid_width,
                      const int grid_height, coord_t start, uint64_t steps = STEPS_PART_2);

uint64_t solve_vertical_line(const set<coord_t> &grid, const int grid_width,
                             const int grid_height, coord_t start, uint64_t steps)
{

}

uint64_t solve_half(const set<coord_t> &grid, const int grid_width,
                    const int grid_height, coord_t start, uint64_t steps)
{
    if (steps <= grid_height)
        return find_number_of_garden_tiles_in_infinite_terrain(grid, grid_width, grid_height, start, steps) / 2;
    
    return 2 * solve_half(grid, grid_width, grid_height, {start.row, start.col - ((int64_t)steps / 2)}, steps / 2) + solve_part_2(grid, grid_width, grid_height, {start.row + (int64_t)steps / 2, start.col}, steps / 2);
}

uint64_t solve_part_2(const set<coord_t> &grid, const int grid_width,
                      const int grid_height, coord_t start, uint64_t steps)
{
    return 2 * solve_half(grid, grid_width, grid_height, start, steps);
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
             << solve_part_2(grid, width, height, start) << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
