#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0

typedef struct coord
{
    int64_t row {}, col {};

    coord(int64_t row, int64_t col) : row(row), col(col)
    {}

    coord operator-(const coord &rhs)
    {
        return {row - rhs.row, col - rhs.col};
    }
} coord_t;

uint64_t abs(coord_t coord)
{
    return abs(coord.row) + abs(coord.col);
}

uint64_t find_distances(vector<string> &grid)
{
    // Expand the rows that are empty
    for (auto row_itr = grid.begin(); row_itr != grid.end(); row_itr++)
    {
        bool clean_row = true;
        for (char & col_itr : *row_itr)
        {
            if (col_itr == '#')
            {
                clean_row = false;
                break;
            }
        }
        if (clean_row)
        {
            row_itr = grid.insert(row_itr, *row_itr);
            row_itr++;
        }
    }

    // Expand the columns that are empty
    for (int col = 0; col < grid.at(0).length(); ++col)
    {
        bool clean_col = true;
        for (auto & row : grid)
        {
            if (row.at(col) == '#')
            {
                clean_col = false;
                break;
            }
        }
        if (clean_col)
        {
            for (auto & row : grid)
                row.insert(col, 1, '.' );
            col++;
        }
    }

    vector<coord_t> galaxies {};

    // Add all the galaxies
    for (int row = 0; row < grid.size(); ++row)
        for (int col = 0; col < grid.at(row).length(); ++col)
            if (grid.at(row).at(col) == '#')
                galaxies.emplace_back(coord_t(row, col));


    // Now we need to find the distance between all pairs
    uint64_t distance {};
    for (int i = 0; i < galaxies.size(); ++i)
        for (int j = i + 1; j < galaxies.size(); ++j)
            distance += abs(galaxies.at(i) - galaxies.at(j));

    return distance;
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
    cout << "The sum of distances between galaxies is: " << find_distances(grid) << endl;
    
    // Part 2
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
