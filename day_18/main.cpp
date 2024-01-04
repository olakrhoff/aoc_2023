#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <algorithm>

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

int64_t get_num_coords_on_line(const coord_t &start, const coord_t &end)
{
    switch (get_direction(start, end))
    {
        case NORTH:
            return start.row - end.row + 1;
        case WEST:
            return start.col - end.col + 1;
        case SOUTH:
            return end.row - start.row + 1;
        case EAST:
            return end.col - start.col + 1;
    }
}

int64_t shoelace_formula(const vector<coord_t> &coords)
{
    int64_t area {};

    for (int i = 0; i < coords.size(); ++i)
    {
        int y = (i + 1) % coords.size();
        area += coords.at(i).row * coords.at(y).col - coords.at(i).col * coords.at(y).row;
    }

    return area / 2;
}

/**
 * Pick's theorem: https://en.wikipedia.org/wiki/Pick%27s_theorem
 * Shoelace formula: https://en.wikipedia.org/wiki/Shoelace_formula
 *
 * We use the shoelace formula to calculate the area within the points,
 * then we use Pick's theorem: A = i + b/2 - 1 (i: interior integer point; b: on border integer points)
 * We want the interior points plus the points on the border to find our ducts => i + b
 * We can rewrite the theorem as such: i + b = A + 1 + b/2
 */
int64_t solve(const vector<instruction_t> &instructions)
{
    vector<coord_t> coords {};
    coords.emplace_back(0, 0);

    for (auto ins : instructions)
    {
        coord_t current = coords.back();
        switch (ins.dir)
        {
            case NORTH:
                coords.emplace_back(current.row - ins.length, current.col);
                break;
            case WEST:
                coords.emplace_back(current.row, current.col - ins.length);
                break;
            case SOUTH:
                coords.emplace_back(current.row + ins.length, current.col);
                break;
            case EAST:
                coords.emplace_back(current.row, current.col + ins.length);
                break;
        }
    }

    coords.pop_back(); // Remove the duplicate of the final step back to start

    for (auto &c : coords)
        c.row *= -1;

    // A = i + b/2 - 1
    // i = A - b/2 + 1
    // i + b = A + 1 + b/2
    int64_t area = shoelace_formula(coords);

    int64_t perimeter = static_cast<int64_t>(coords.size());
    for (int i = 0; i < coords.size(); ++i)
        perimeter += get_num_coords_on_line(coords.at(i), coords.at((i + 1) % coords.size())) - 2;

    return area + 1 + perimeter / 2;
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
        cout << "The number of cubic meters of lava that the lagoon can hold is: " << solve(instructions)
             << endl;
        // Part 2
    else
        cout << "The number of cubic meters of lava that the lagoon can hold is: " << solve(instructions)
             << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
