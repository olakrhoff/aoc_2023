#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

#define DEBUG 0
#define PART_1 false

typedef enum TYPES
{
    FIVE,
    FOUR,
    FULL_HOUSE,
    THREE,
    TWO_PAIR,
    ONE_PAIR,
    HIGH_CARD
} TYPES_T;

typedef struct bet
{
    vector<uint64_t> hand {};
    uint64_t bid {};
    TYPES_T type {};
    
    bet(const vector<uint64_t> &hand, uint64_t bid) : hand(hand), bid(bid)
    {
        type = get_type();
    }
    
    TYPES_T get_type()
    {
        uint64_t count[15] {};
        for (auto h : hand)
            count[h]++;
        vector<uint64_t> sorted_count {};
        
        for (int i = 2; i < 15; ++i)
                sorted_count.emplace_back(count[i]);
        std::sort(sorted_count.begin(), sorted_count.end());
        std::reverse(sorted_count.begin(), sorted_count.end());
        uint64_t jokers = count[1];
        
        
        if (sorted_count.at(0) == 5 - jokers)
            return FIVE;
        
        if (sorted_count.at(0) == 4 - jokers)
            return FOUR;
        
        bool two = false, three = false;
        uint64_t jokers_temp = jokers;
        
        if (sorted_count.at(0) >= 3 - jokers_temp)
        {
            three = true;
            jokers_temp -= 3 - sorted_count.at(0);
        }
        if (sorted_count.at(1) >= 2 - jokers_temp)
            two = true;
        if (two && three)
            return FULL_HOUSE;
        
        
        
        if (sorted_count.at(0) == 3 - jokers)
            return THREE;
        
        
        jokers_temp = jokers;
        uint64_t twos {};
        if (sorted_count.at(0) >= 2 - jokers_temp)
        {
            twos++;
            jokers_temp -= 2 - sorted_count.at(0);
        }
        if (sorted_count.at(1) >= 2 - jokers_temp)
            twos++;
        if (twos == 2)
            return TWO_PAIR;
        
        
        if (sorted_count.at(0) == 2 - jokers)
            return ONE_PAIR;
        
        return HIGH_CARD;
    }
    
    bool operator<(const bet &rhs) const
    {
        if (this->type < rhs.type)
            return false;
        else if (this->type > rhs.type)
            return true;
        
        // Inferred that the type is equal
        
        for (int i = 0; i < this->hand.size(); ++i)
        {
            if (this->hand.at(i) > rhs.hand.at(i))
                return false;
            else if (this->hand.at(i) < rhs.hand.at(i))
                return true;
        }
        
        return false;
    }
    
    bool operator>(const bet &rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const bet &rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const bet &rhs) const
    {
        return !(*this < rhs);
    }
    
} bet_t;

uint64_t get_value_of_card(const char c)
{
    switch (c)
    {
        case 'A':
            return 14;
        case 'K':
            return 13;
        case 'Q':
            return 12;
        case 'J':
        {
            if (PART_1)
                return 11;
            else
                return 1; // This will be the lowest number
        }
        case 'T':
            return 10;
        default:
            return c - '0';
    }
}

bet_t parse_line_to_bet(const string &line)
{
    vector<uint64_t> hand {};
    for (int i = 0; i < 5; ++i)
        hand.emplace_back(get_value_of_card(line.at(i)));
    
    uint64_t bid = stoi(line.substr(6));
    
    return bet_t(hand, bid);
}

uint64_t get_total_winnings(const vector<bet_t> &bets)
{
    uint64_t winnings {};
    
    for (int i = 0; i < bets.size(); ++i)
        winnings += bets.at(i).bid * (i + 1);
    
    return winnings;
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
    vector<bet_t> bets {};
    while (!file.eof())
    {
        getline(file, temp);
        bets.emplace_back(parse_line_to_bet(temp));
    }
    
    file.close();
    
    sort(bets.begin(), bets.end());
    
    // Part 1
    cout << "The total winnings are: " << get_total_winnings(bets) << endl;
    
    // Part 2
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
