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

vector<coord_t> get_coords_on_line(const coord_t &start, const coord_t &end)
{
    vector<coord_t> coords {};
    switch (get_direction(start, end))
    {
        case NORTH:
            for (int row = start.row; row >= end.row; --row)
                coords.emplace_back(row, start.col);
            break;
        case WEST:
            for (int col = start.col; col >= end.col; --col)
                coords.emplace_back(start.row, col);
            break;
        case SOUTH:
            for (int row = start.row; row <= end.row; ++row)
                coords.emplace_back(row, start.col);
            break;
        case EAST:
            for (int col = start.col; col <= end.col; ++col)
                coords.emplace_back(start.row, col);
            break;
    }

    return coords;
}

void print_coords(const map<coord_t, vector<coord_t>> &adjacent)
{
    // Initialize max and min values to extreme values
    int64_t max_row = numeric_limits<int64_t>::min();
    int64_t min_row = numeric_limits<int64_t>::max();
    int64_t max_col = numeric_limits<int64_t>::min();
    int64_t min_col = numeric_limits<int64_t>::max();

    // Iterate through the vector and update max and min values
    for (const auto &[c, list] : adjacent)
    {
        max_row = max(max_row, c.row);
        min_row = min(min_row, c.row);
        max_col = max(max_col, c.col);
        min_col = min(min_col, c.col);
    }

    vector<coord_t> coords {};
    for (const auto &[c, list] : adjacent)
        for (auto coord : get_coords_on_line(c, list.front()))
            coords.emplace_back(coord);

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

void get_interior_points(DIRECTIONS_T prev_direction, const vector<coord_t> &coords, const coord_t &current,
                         set<coord_t> &interior, const map<coord_t, vector<coord_t>> &adjacent)
{
    switch (prev_direction)
    {
        case NORTH:
        {
            // Find all points to the east of our point
            vector<coord_t> candidates {};
            for (auto c : coords)
                if (c.col > current.col)
                    candidates.emplace_back(c);
            // Sort them from closest to furthest away in col distance
            std::sort(candidates.begin(), candidates.end(), [](const coord_t &a, const coord_t &b) {
                if (a.col != b.col)
                    return a.col < b.col;
                return a.row < b.row;
            });

            if (candidates.size() % 2 != 0)
            {
                cout << "There should be an even number due to the pairs" << endl;
                exit(EXIT_FAILURE);
            }
            int64_t nearest_col {};
            // These come in pairs, since all lines must have to edges, with either a row or col being equal
            for (int i = 0; i < candidates.size(); i += 2)
            {
                coord_t top = candidates.at(i), bottom = candidates.at(i + 1);

                // If the line between the pair intercepts our current row, we have found our distance
                if (top.row <= current.row && bottom.row >= current.row)
                {
                    // If they are neighbours, ignore
                    if (top.row == current.row || bottom.row == current.row)
                        if (adjacent.contains(current))
                        {
                            auto neighbours = adjacent.at(current);
                            if (neighbours.at(0) == top ||
                                neighbours.at(0) == bottom ||
                                neighbours.at(1) == top ||
                                neighbours.at(1) == bottom)
                                return;
                        }

                    nearest_col = top.col;
                    break;
                }
            }

            // If so, add all interior points between said line and current point
            for (int64_t col = current.col + 1; col < nearest_col; ++col)
                interior.emplace(current.row, col);
            break;
        }
        case EAST:
        {
            // Find all points to the south of our point
            vector<coord_t> candidates {};
            for (auto c : coords)
                if (c.row > current.row)
                    candidates.emplace_back(c);
            // Sort them from closest to furthest away in row distance
            std::sort(candidates.begin(), candidates.end(), [](const coord_t &a, const coord_t &b) {
                if (a.row != b.row)
                    return a.row < b.row;
                return a.col > b.col;
            });

            if (candidates.size() % 2 != 0)
            {
                cout << "There should be an even number due to the pairs" << endl;
                exit(EXIT_FAILURE);
            }
            int64_t nearest_row {};
            // These come in pairs, since all lines must have to edges, with either a row or col being equal
            for (int i = 0; i < candidates.size(); i += 2)
            {
                coord_t right = candidates.at(i), left = candidates.at(i + 1);

                // If the line between the pair intercepts our current row, we have found our distance
                if (right.col >= current.col && left.col <= current.col)
                {
                    // If they are neighbours, ignore
                    if (right.col == current.col || left.col == current.col)
                        if (adjacent.contains(current))
                        {
                            auto neighbours = adjacent.at(current);
                            if (neighbours.at(0) == right ||
                                neighbours.at(0) == left ||
                                neighbours.at(1) == right ||
                                neighbours.at(1) == left)
                                return;
                        }

                    nearest_row = right.row;
                    break;
                }
            }

            // If so, add all interior points between said line and current point
            for (int64_t row = current.row + 1; row < nearest_row; ++row)
                interior.emplace(row, current.col);
            break;
        }
        case SOUTH:
        {
            // Find all points to the west of our point
            vector<coord_t> candidates {};
            for (auto c : coords)
                if (c.col < current.col)
                    candidates.emplace_back(c);
            // Sort them from closest to furthest away in col distance
            std::sort(candidates.begin(), candidates.end(), [](const coord_t &a, const coord_t &b) {
                if (a.col != b.col)
                    return a.col > b.col;
                return a.row < b.row;
            });

            if (candidates.size() % 2 != 0)
            {
                cout << "There should be an even number due to the pairs" << endl;
                exit(EXIT_FAILURE);
            }
            int64_t nearest_col {};
            // These come in pairs, since all lines must have to edges, with either a row or col being equal
            for (int i = 0; i < candidates.size(); i += 2)
            {
                coord_t top = candidates.at(i), bottom = candidates.at(i + 1);

                // If the line between the pair intercepts our current row, we have found our distance
                if (top.row <= current.row && bottom.row >= current.row)
                {
                    // If they are neighbours, ignore
                    if (top.row == current.row || bottom.row == current.row)
                        if (adjacent.contains(current))
                        {
                            auto neighbours = adjacent.at(current);
                            if (neighbours.at(0) == top ||
                                neighbours.at(0) == bottom ||
                                neighbours.at(1) == top ||
                                neighbours.at(1) == bottom)
                                return;
                        }

                    nearest_col = top.col;
                    break;
                }
            }

            // If so, add all interior points between said line and current point
            for (int64_t col = current.col - 1; col > nearest_col; --col)
                interior.emplace(current.row, col);
            break;
        }
        case WEST:
        {
            // Find all points to the north of our point
            vector<coord_t> candidates {};
            for (auto c : coords)
                if (c.row < current.row)
                    candidates.emplace_back(c);
            // Sort them from closest to furthest away in row distance
            std::sort(candidates.begin(), candidates.end(), [](const coord_t &a, const coord_t &b) {
                if (a.row != b.row)
                    return a.row > b.row;
                return a.col > b.col;
            });

            if (candidates.size() % 2 != 0)
            {
                cout << "There should be an even number due to the pairs" << endl;
                exit(EXIT_FAILURE);
            }
            int64_t nearest_row {};
            // These come in pairs, since all lines must have to edges, with either a row or col being equal
            for (int i = 0; i < candidates.size(); i += 2)
            {
                coord_t right = candidates.at(i), left = candidates.at(i + 1);

                // If the line between the pair intercepts our current row, we have found our distance
                if (right.col >= current.col && left.col <= current.col)
                {
                    // If they are neighbours, ignore
                    if (right.col == current.col || left.col == current.col)
                        if (adjacent.contains(current))
                        {
                            auto neighbours = adjacent.at(current);
                            if (neighbours.at(0) == right ||
                                neighbours.at(0) == left ||
                                neighbours.at(1) == right ||
                                neighbours.at(1) == left)
                                return;
                        }

                    nearest_row = right.row;
                    break;
                }
            }

            // If so, add all interior points between said line and current point
            for (int64_t row = current.row - 1; row > nearest_row; --row)
                interior.emplace(row, current.col);
            break;
        }
    }
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

int64_t find_total_area(const vector<instruction_t> &instructions)
{
    vector<coord_t> coords {};
    coords.emplace_back(0, 0);

    for (auto ins : instructions)
    {
        coord_t current = coords.back();
        switch (ins.dir)
        {
            case NORTH:
                coords.emplace_back(current.row - ins.length - 1, current.col);
                break;
            case WEST:
                coords.emplace_back(current.row, current.col - ins.length - 1);
                break;
            case SOUTH:
                coords.emplace_back(current.row + ins.length + 1, current.col);
                break;
            case EAST:
                coords.emplace_back(current.row, current.col + ins.length + 1);
                break;
        }
    }

    coords.pop_back(); // Remove the duplicate of the final step back to start

    int64_t perimeter_count = static_cast<int64_t>(coords.size());
    for (int i = 0; i < coords.size(); ++i)
        perimeter_count += get_num_coords_on_line(coords.at(i), coords.at((i + 1) % coords.size())) - 2;


    map<coord_t, vector<coord_t>> adjacent {};
    for (int i = 0; i < coords.size(); ++i)
        adjacent.insert({coords.at(i),
                         {coords.at((i - 1 + coords.size()) % coords.size()), coords.at((i + 1) % coords.size())}});

    if (DEBUG && PART_1)
        print_coords(adjacent);

    // Fill in the interior
    set<coord_t> interior {};
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
        for (auto coord : get_coords_on_line(prev, current))
            get_interior_points(prev_direction, coords, coord, interior, adjacent);

        bool found = false;
        for (auto next : adjacent.at(current))
            if (next != prev)
            {
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

        static uint64_t counter {};

        counter++;
        cout << (double) counter / perimeter_count << "%" << endl;

    } while (current != start_coord);


    return perimeter_count + interior.size();
}

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
