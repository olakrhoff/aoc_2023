#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#define DEBUG 0
#define PART_1 false

typedef struct node
{
    string label {};
    node *left {};
    node *right {};
} node_t;

typedef struct graph
{
    vector<node_t *> nodes;
    
    virtual ~graph()
    {
        for (auto n : nodes)
            delete n;
    }
    
    void insert_node(const string &node)
    {
        for (node_t *n : nodes)
            if (n->label == node)
                return;
        
        // Node not found
        node_t *current = new node_t;
        current->label = node;
        nodes.emplace_back(current);
    }
    
    /*
     * This method assumes that the insert_node has been called on the
     * nodes used, such that exists is guaranteed, otherwise we panic
     */
    void insert_edge(const string &from, const string &to, bool left)
    {
        node_t *from_node, *to_node;
        for (node_t *n : nodes)
        {
            if (n->label == from)
                from_node = n;
            else if (n->label == to)
                to_node = n;
        }
        
        if (from_node == nullptr || to_node == nullptr)
        {
            cout << "Both from and to node should be found" << endl;
            exit(1);
        }
        
        if (left)
            from_node->left = to_node;
        else
            from_node->right = to_node;
    }
    
    void add(const node_t &node)
    {
        // Ensure that all nodes are in the graph
        insert_node(node.label);
        insert_node(node.left->label);
        insert_node(node.right->label);
        
        // Add the edges between the nodes
        insert_edge(node.label, node.left->label, true);
        insert_edge(node.label, node.right->label, false);
        
        delete node.left;
        delete node.right;
    }
} graph_t;

node_t parse_line_to_node(const string &line)
{
    node_t node;
    string label = line.substr(0, 3);
    
    string left = line.substr(7, 3);
    string right = line.substr(12, 3);
    
    node.label = label;
    node.left = new node_t;
    node.left->label = left;
    node.right = new node_t;
    node.right->label = right;
    
    return node;
}

uint64_t find_steps(const string &start, const string &end, const string &instructions, const graph_t &network)
{
    node_t *start_node = nullptr;
    for (node_t *n : network.nodes)
    {
        if (n->label == start)
        {
            start_node = n;
            break;
        }
    }
    if (start_node == nullptr)
    {
        cout << "Should find start node, but did not" << endl;
        exit(1);
    }
    
    node_t *end_node = nullptr;
    for (node_t *n : network.nodes)
    {
        if (n->label == end)
        {
            end_node = n;
            break;
        }
    }
    if (end_node == nullptr)
    {
        cout << "Should find end node, but did not" << endl;
        exit(1);
    }
    
    
    uint64_t steps {};
    node_t *current = start_node;
    while (current != end_node)
    {
        bool left = instructions.at(steps % instructions.size()) == 'L';
        
        if (left)
            current = current->left;
        else
            current = current->right;
        
        steps++;
    }
    
    return steps;
}

uint64_t gcd(uint64_t a, uint64_t b)
{
    if (a == 0)
        return b;
    return gcd(b % a, a);
}

uint64_t gcd_list(const vector<uint64_t> &numbers)
{
    uint64_t gcd_res = numbers.at(0);
    for (int i = 1; i < numbers.size(); ++i)
    {
        gcd_res = gcd(numbers.at(i), gcd_res);
        
        if(gcd_res == 1)
            return 1;
    }
    return gcd_res;
}

typedef struct instance
{
    node_t *node {};
    uint64_t step {};
    
    instance(node_t *node, uint64_t step) : node(node), step(step)
    {}
    
    bool operator<(const instance &rhs) const
    {
        if (node < rhs.node)
            return true;
        if (rhs.node < node)
            return false;
        return step < rhs.step;
    }
    
    bool operator>(const instance &rhs) const
    {
        return rhs < *this;
    }
    
    bool operator<=(const instance &rhs) const
    {
        return !(rhs < *this);
    }
    
    bool operator>=(const instance &rhs) const
    {
        return !(*this < rhs);
    }
} instance_t;

typedef struct state
{
    private:
    // We have this double data to maintain both the order and enable quick searches through the data
    vector<instance_t> instances {};
    map<instance_t, uint64_t> mapped_instances {};
    uint64_t cycle_length {};
    
    public:
    node_t *get_node(uint64_t steps) const
    {
        for (int i = 0; i < instances.size(); ++i)
        {
            if (instances.at(i).step == steps % cycle_length)
                return instances.at(i).node;
        }
        
        cout << "Should find a node" << endl;
        exit(1);
    }
    
    bool has(node_t *node, uint64_t step) const
    {
        if (instances.size() < step)
            return false;
        
        auto itr = mapped_instances.find({node, step});
        if (itr == mapped_instances.end())
            return false;
        return true;
    }
    
    uint64_t get_first() const
    {
        for (auto instance : instances)
        {
            if (instance.node->label.at(2) == 'Z')
                return instance.step;
        }
        cout << "Should find a node with **Z" << endl;
        exit(1);
    }
    
    uint64_t get_size() const
    {
        return instances.size();
    }
    
    instance_t get_instance_at(uint64_t index) const
    {
        return instances.at(index);
    }
    
    uint64_t get_cycle_length() const
    {
        return cycle_length;
    }
    
    void emplace_back(instance_t instance)
    {
        instances.emplace_back(instance);
        mapped_instances.insert({instance, (uint64_t)(instances.size() - 1)});
    }
    
    void set_cycle_length(uint64_t steps)
    {
        cycle_length = steps;
    }
    
} state_t;

bool check_for_cycle(state_t state, const string &instructions)
{
    if (state.get_size() == 0)
        return false;
    
    if (state.get_instance_at(state.get_size() - 1).step < instructions.length())
        return false;
    
    if (state.get_size() % 2 != 0)
        return false;
    
    for (int i = 0; i < state.get_size() / 2; ++i)
    {
        if (state.get_instance_at(i).node != state.get_instance_at(i + (state.get_size() / 2)).node)
            return false;
        if (state.get_instance_at(i).step != state.get_instance_at(i + (state.get_size() / 2)).step + state.get_cycle_length() / 2)
            return false;
    }
    
    return true;
}

/**
 * We hit a cycle if we are at the same node at the same instruction.
 * We thus store all states, to find out how long a cycle is and return
 * at which indexes in this cycle the state is at an end node.
 */
state_t find_cycle(const node_t *start, const graph_t &network, const string &instructions)
{
    node_t *start_node = nullptr;
    for (node_t *n : network.nodes)
    {
        if (n->label == start->label)
        {
            start_node = n;
            break;
        }
    }
    if (start_node == nullptr)
    {
        cout << "Should find start node, but did not" << endl;
        exit(1);
    }
    
    state_t state;
    uint64_t steps {};
    node_t *current = start_node;
    //while (!check_for_cycle(state, instructions))
    while (!state.has(current, steps % instructions.size()))
    {
        //if (current->label.at(2) == 'Z')
        {
            state.emplace_back({current, steps});
        }
        bool left = instructions.at(steps % instructions.size()) == 'L';
        
        if (left)
            current = current->left;
        else
            current = current->right;
        
        steps++;
        state.set_cycle_length(steps);
    }
    
    return state;
}

bool all_nodes_equal(const vector<state_t> &states, uint64_t steps, const char c)
{
    for (auto s : states)
        if (s.get_node(steps)->label.at(2) != c)
            return false;
    
    return true;
}

uint64_t find_parallel_steps(const char start, const char end, const string &instructions, const graph_t &network)
{
    vector<node_t *> start_nodes {};
    for (node_t *n : network.nodes)
        if (n->label.at(2) == start)
            start_nodes.emplace_back(n);
    if (start_nodes.empty())
    {
        cout << "Should find some start nodes, but did not" << endl;
        exit(1);
    }
    
    
    // OK, my hopes did not come to flourish, we will have to do some cycle detection
    // and then use modulo maths to calculate when all will occur at the same time
    vector<state_t> states {};
    
    for (node_t *n : start_nodes)
        states.emplace_back(find_cycle(n, network, instructions));
    
    uint64_t steps {1};
    for (int i = 0; i < states.size(); ++i)
    {
        uint64_t n = states.at(i).get_first();
        steps = (steps * n) / gcd(steps, n);
    }
    
    return steps;
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
    graph_t network;
    string instructions;
    getline(file, instructions);
    getline(file, temp);
    while (!file.eof())
    {
        getline(file, temp);
        network.add(parse_line_to_node(temp));
    }
    
    file.close();
    
    // Part 1
    if (PART_1)
        cout << "Steps from AAA to ZZZ is: " << find_steps("AAA", "ZZZ", instructions, network) << endl;
    // Part 2
    else
        cout << "Parallel steps from **A to **Z is: " << find_parallel_steps('A', 'Z', instructions, network) << endl;
    
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    
    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;
    
    return EXIT_SUCCESS;
}
