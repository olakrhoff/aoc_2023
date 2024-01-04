#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <sstream>

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

typedef struct instruction
{
    DIRECTIONS_T dir {};
    uint64_t length {};
} instruction_t;

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

instruction_t parse_to_instruction(const string &line)
{
    instruction_t ins;

    switch (line.at(0))
    {
        case 'U':
            ins.dir = NORTH;
            break;
        case 'L':
            ins.dir = WEST;
            break;
        case 'D':
            ins.dir = SOUTH;
            break;
        case 'R':
            ins.dir = EAST;
            break;
    }

    string length_string = line.substr(2, line.substr(2).find(' '));
    ins.length = stoi(length_string);

    return ins;
}

int64_t string_hex_to_int(const string &hex)
{
    std::istringstream iss(hex);
    int64_t result;
    iss >> std::hex >> result;

    // Check if the conversion was successful
    if (iss.fail())
    {
        std::cerr << "Conversion failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    return result;
}

instruction_t parse_to_instruction_corrected(const string &line)
{
    instruction_t ins;

    string hex = line.substr(line.find('(') + 2, 6);

    string length_in_hex = hex.substr(0, 5);

    ins.length = string_hex_to_int(length_in_hex);

    switch (string_hex_to_int(hex.substr(5)))
    {
        case 0:
            ins.dir = EAST;
            break;
        case 1:
            ins.dir = SOUTH;
            break;
        case 2:
            ins.dir = WEST;
            break;
        case 3:
            ins.dir = NORTH;
            break;
        default:
            cout << "Last number should be [0,4]" << endl;
            exit(EXIT_FAILURE);
    }

    return ins;
}

void print_coords(const vector<coord_t> &coords)
{
    // Initialize max and min values to extreme values
    int64_t max_row = numeric_limits<int64_t>::min();
    int64_t min_row = numeric_limits<int64_t>::max();
    int64_t max_col = numeric_limits<int64_t>::min();
    int64_t min_col = numeric_limits<int64_t>::max();

    // Iterate through the vector and update max and min values
    for (const auto& c : coords) {
        max_row = max(max_row, c.row);
        min_row = min(min_row, c.row);
        max_col = max(max_col, c.col);
        min_col = min(min_col, c.col);
    }

    cout << endl;
    for (int64_t row = min_row; row <= max_row; ++row)
    {
        for (int64_t col = min_col; col <= max_col; ++col)
        {
            bool found = false;
            for (auto c : coords)
                if (c == coord_t(row, col))
                {
                    found = true;
                    break;
                }

            if (found)
                cout << '#';
            else
                cout << '.';
        }
        cout << endl;
    }
}

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

void get_interior_points(DIRECTIONS_T prev_direction, const vector<coord_t> &coords, const coord_t &current, set<coord_t> &interior)
{
    switch (prev_direction)
    {
        case NORTH:
        {
            vector<coord_t> candidates {};
            for (auto c : coords)
                if (c.row == current.row && c.col > current.col)
                    candidates.emplace_back(c);

            int64_t min_col = candidates.front().col;
            for (auto c : candidates)
                if (c.col < min_col)
                    min_col = c.col;

            for (int i = 0; i < min_col - current.col - 1; ++i)
                interior.emplace(current.row, current.col + 1 + i);
            break;
        }
        case EAST:
        {
            vector<coord_t> candidates {};
            for (auto c: coords)
                if (c.col == current.col && c.row > current.row)
                    candidates.emplace_back(c);

            int64_t min_row = candidates.front().row;
            for (auto c: candidates)
                if (c.row < min_row)
                    min_row = c.row;

            for (int i = 0; i < min_row - current.row - 1; ++i)
                interior.emplace(current.row + 1 + i, current.col);
            break;
        }
        case SOUTH:
        {
            vector<coord_t> candidates {};
            for (auto c: coords)
                if (c.row == current.row && c.col < current.col)
                    candidates.emplace_back(c);

            int64_t max_col = candidates.front().col;
            for (auto c: candidates)
                if (c.col > max_col)
                    max_col = c.col;

            for (int i = 0; i < current.col - max_col - 1; ++i)
                interior.emplace(current.row, current.col - 1 - i);
            break;
        }
        case WEST:
        {
            vector<coord_t> candidates {};
            for (auto c: coords)
                if (c.col == current.col && c.row < current.row)
                    candidates.emplace_back(c);

            int64_t max_row = candidates.front().row;
            for (auto c: candidates)
                if (c.row > max_row)
                    max_row = c.row;

            for (int i = 0; i < current.row - max_row - 1; ++i)
                interior.emplace(current.row - 1 - i, current.col);
            break;
        }
    }
}

uint64_t find_total_area(const vector<instruction_t> &instructions)
{
    vector<coord_t> coords {};
    coords.emplace_back(0, 0);

    for (auto ins : instructions)
    {
        coord_t current = coords.back();
        for (int i = 0; i < ins.length; ++i)
        {
            switch (ins.dir)
            {
                case NORTH:
                    coords.emplace_back(current.row - 1 - i, current.col);
                    break;
                case WEST:
                    coords.emplace_back(current.row, current.col - 1 - i);
                    break;
                case SOUTH:
                    coords.emplace_back(current.row + 1 + i, current.col);
                    break;
                case EAST:
                    coords.emplace_back(current.row, current.col + 1 + i);
                    break;
            }
        }
    }

    coords.pop_back(); // Remove the duplicate of the final step back to start
    if (DEBUG && PART_1)
        print_coords(coords);

    map<coord_t, vector<coord_t>> adjacent {};
    for (int i = 0; i < coords.size(); ++i)
        adjacent.insert({coords.at(i), {coords.at((i - 1 + coords.size()) % coords.size()), coords.at((i + 1) % coords.size())}});


    // Fill in the interior
    set<coord_t> interior {};
    uint64_t interior_count {};
    // The start coordinate is the top-left most coord
    coord_t start_coord = (*adjacent.begin()).first;
    coord_t current = start_coord, prev;
    // Ensure that we explore in clockwise order such that we use the
    // left hand rule and check for interior points to the right
    if (get_direction(current, adjacent.at(current).at(0)) == SOUTH)
        prev = adjacent.at(current).at(0);
    else
        prev = adjacent.at(current).at(1);

    do
    {
        DIRECTIONS_T prev_direction = get_direction(prev, current);
        get_interior_points(prev_direction, coords, current, interior);

        bool found = false;
        for (auto next: adjacent.at(current))
            if (next != prev)
            {
                // If we round an interior corner we need to check both directions from the corner
                DIRECTIONS_T next_direction = get_direction(current, next);
                // Checks if we don't have south and north, or west and east
                // in other words if we have a corner
                if ((next_direction + prev_direction) % 2 != 0)
                    get_interior_points(next_direction, coords, current, interior);
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


    } while (current != start_coord);

    return coords.size() + interior.size();
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
    vector<instruction_t> instructions {};
    while (!file.eof())
    {
        getline(file, temp);
        if (PART_1)
            instructions.emplace_back(parse_to_instruction(temp));
        else
            instructions.emplace_back(parse_to_instruction_corrected(temp));

    }
    
    file.close();
    
    // Part 1
    if (PART_1)
        cout << "The number of cubic meters of lava that the lagoon can hold is: " << find_total_area(instructions) << endl;
    // Part 2
    else
        cout << "The number of cubic meters of lava that the lagoon can hold is: " << find_total_area(instructions) << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
