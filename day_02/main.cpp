#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define DEBUG 0
#define PART_1 false

#define RED_CAP 12
#define GREEN_CAP 13
#define BLUE_CAP 14


typedef struct
{
    uint64_t red {0}, green {0}, blue {0};
} draw_t;

typedef struct
{
    uint64_t ID {0};
    vector<draw_t> draws;
} game_t;

vector<string> split_on_delimiter(const string &list, const char delimiter)
{
    vector<string> res;
    string temp;
    for (auto c : list)
    {
        if (c == ' ' && temp == "")
            continue;
        
        if (c == delimiter)
        {
            res.emplace_back(temp);
            temp = "";
            continue;
        }
        temp += c;
    }
    res.emplace_back(temp);
    
    return res;
}

draw_t parse_draw(const string &input)
{
    draw_t draw;
    vector<string> per_colour = split_on_delimiter(input, ',');
    
    for (auto c : per_colour)
    {
        uint64_t number = stoi(c.substr(0, c.find(' ')));
        if (c.contains("red"))
            draw.red = number;
        else if (c.contains("green"))
            draw.green = number;
        else if (c.contains("blue"))
            draw.blue = number;
        else
        {
            cout << "Should contain one of the colours" << endl;
            exit(2);
        }
    }
    
    return draw;
}

game_t parse_game(const string &input)
{
    game_t game;
    string game_id = input.substr(5, input.find(':') - 5);
    game.ID = (uint64_t)stoi(game_id);
    
    
    vector<string> subsets = split_on_delimiter(input.substr(input.find(':') + 1), ';');
    
    vector<draw_t> draws;
    for (auto s : subsets)
        draws.emplace_back(parse_draw(s));
    
    game.draws = draws;
    
    return game;
}

bool is_game_possible(const game_t &game)
{
    for (auto draw : game.draws)
        if (draw.red > RED_CAP || draw.green > GREEN_CAP || draw.blue > BLUE_CAP)
            return false;
    
    return true;
}

uint64_t power_of_game(const game_t &game)
{
    uint64_t max_red {0}, max_green {0}, max_blue {0};
    
    for (auto draw : game.draws)
    {
        if (draw.red > max_red)
            max_red = draw.red;
        if (draw.green > max_green)
            max_green = draw.green;
        if (draw.blue > max_blue)
            max_blue = draw.blue;
    }
    
    return max_red * max_green * max_blue;
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
    
    uint64_t sum_of_ids {0};
    uint64_t sum_of_power {0};
    while (!file.eof())
    {
        getline(file, temp);
        game_t game = parse_game(temp);
        if (PART_1)
        {
            if (is_game_possible(game))
                sum_of_ids += game.ID;
        }
        else
            sum_of_power += power_of_game(game);
    }
    
    file.close();
    if (PART_1)
        cout << "Sum of IDs: " << sum_of_ids << endl;
    else
        cout << "Sum of power of all games: " << sum_of_power << endl;
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
