#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

#define DEBUG 0
#define PART_1 false

typedef struct card
{
    uint64_t id {};
    uint64_t count {1}; // Used in part 2
    vector<uint64_t> winnings {};
    vector<uint64_t> guesses {};
} card_t;

string &trim(string &line)
{
    while (line.at(0) == ' ')
        line = line.substr(1);
    while (line.at(line.size() - 1) == ' ')
        line = line.substr(0, line.size() - 1);
    
    return line;
}

vector<string> split(const string &line, const char delimiter)
{
    vector<string> res {};
    
    string temp = line;
    while (temp.find(delimiter) != -1)
    {
        res.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + 1);
        trim(temp);
    }
    res.emplace_back(temp);
    
    return res;
}

vector<uint64_t> get_numbers_from_list(const string &line)
{
    vector<uint64_t> nums {};
    
    vector<string> string_nums = split(line, ' ');
    
    for (auto s : string_nums)
        nums.emplace_back(stoi(s));
    
    return nums;
}

card_t parse_line(const string &line)
{
    card_t card;
    
    card.id = stoi(line.substr(5, line.find(':') - 5 + 1));
    
    string temp = line.substr(line.find(':') + 1);
    
    vector<string> lists = split(temp, '|');
    
    if (lists.size() != 2)
    {
        cout << "Should not be that the split list is not of size 2" << endl;
        exit(-1);
    }
    
    card.winnings = get_numbers_from_list(trim(lists.at(0)));
    std::sort(card.winnings.begin(), card.winnings.end());
    card.guesses = get_numbers_from_list(trim(lists.at(1)));
    std::sort(card.guesses.begin(), card.guesses.end());
    
    return card;
}

uint64_t get_points(card_t &card)
{
    int wins {0};
    for (auto win : card.winnings)
        if (std::find(card.guesses.begin(), card.guesses.end(), win) != card.guesses.end())
            wins++;
    
    if (wins == 0)
        return 0;
    return 1 << (wins - 1);
}

uint64_t get_matching_numbers(card_t &card)
{
    int wins {0};
    for (auto win : card.winnings)
        if (std::find(card.guesses.begin(), card.guesses.end(), win) != card.guesses.end())
            wins++;
    
    return wins;
}

uint64_t get_number_of_cards(vector<card_t> &cards)
{
    for (int i = 0; i < cards.size(); ++i)
    {
        uint64_t wins = get_matching_numbers(cards.at(i));
        for (int j = i + 1; j <= i + wins; ++j)
            cards.at(j).count += cards.at(i).count;
    }
    
    uint64_t sum {};
    
    for (auto card : cards)
        sum += card.count;
    
    return sum;
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
    uint64_t sum {};
    vector<card_t> cards {};
    while (!file.eof())
    {
        getline(file, temp);
        card_t card = parse_line(temp);
        if (PART_1)
            sum += get_points(card);
        else
            cards.emplace_back(card);
    }
    
    file.close();
    
    // Part 1
    if (PART_1)
        cout << "The sum of points is: " << sum << endl;
    // Part 2
    else
        cout << "The number of cards is: " << get_number_of_cards(cards) << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
