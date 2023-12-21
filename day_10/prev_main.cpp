#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

#define DEBUG 0

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

typedef enum PIPES
{
    VERTICAL = '|',
    HORIZONTAL = '-',
    NORTH_EAST = 'L',
    NORTH_WEST = 'J',
    SOUTH_WEST = '7',
    SOUTH_EAST = 'F',
    GROUND = '.',
    STARTING = 'S'
} PIPES_T;

typedef struct node
{
    coord_t coord {};
    uint64_t distance {};
    bool visited {false};
    PIPES_T pipe;
    vector<node *> nodes {};

    explicit node(const coord_t &coord, PIPES_T pipe) : coord(coord), pipe(pipe)
    {}
} node_t;

typedef struct point
{
    coord_t coord;
    PIPES_T pipe;

    point(coord_t coord, PIPES_T pipe) : coord(coord), pipe(pipe)
    {}

    point() = default;

    coord_t get_first() const
    {
        coord_t neighbour = coord;
        switch (pipe)
        {
            case VERTICAL:
                neighbour.row--;
                break;
            case HORIZONTAL:
                neighbour.col--;
                break;
            case NORTH_EAST:
                neighbour.row--;
                break;
            case NORTH_WEST:
                neighbour.row--;
                break;
            case SOUTH_WEST:
                neighbour.row++;
                break;
            case SOUTH_EAST:
                neighbour.row++;
                break;
            case GROUND:
                neighbour.row = -1;
                break;
            case STARTING:
                break;
        }

        return neighbour;
    }

    coord_t get_second() const
    {
        coord_t neighbour = coord;
        switch (pipe)
        {
            case VERTICAL:
                neighbour.row++;
                break;
            case HORIZONTAL:
                neighbour.col++;
                break;
            case NORTH_EAST:
                neighbour.col++;
                break;
            case NORTH_WEST:
                neighbour.col--;
                break;
            case SOUTH_WEST:
                neighbour.col--;
                break;
            case SOUTH_EAST:
                neighbour.col++;
                break;
            case GROUND:
                neighbour.row = -1;
                break;
            case STARTING:
                break;
        }

        return neighbour;
    }

    bool operator<(const point &rhs) const
    {
        return coord < rhs.coord;
    }

    bool operator>(const point &rhs) const
    {
        return rhs < *this;
    }

    bool operator<=(const point &rhs) const
    {
        return !(rhs < *this);
    }

    bool operator>=(const point &rhs) const
    {
        return !(*this < rhs);
    }

    bool operator==(const point &rhs) const
    {
        return coord == rhs.coord;
    }

    bool operator!=(const point &rhs) const
    {
        return !(rhs == *this);
    }

} point_t;

vector<node_t> parse_line_to_nodes(const string &line, int64_t row)
{
    vector<node_t> nodes {};
    for (int64_t col = 0; col < line.length(); ++col)
        nodes.emplace_back(node_t({row, col}, (PIPES_T) line.at(col)));

    return nodes;
}

void sort_heap_back(vector<node_t *> heap, bool (*func)(node_t *, node_t *))
{
    int index = heap.size() - 1;
    while (index > 0)
    {
        node_t *current = heap.at(index);
        int parent_index = (index - 1) / 2;
        node_t *parent = heap.at(parent_index);

        // If current is less than the parent
        if (func(current, parent))
        {
            heap.at(parent_index) = current;
            heap.at(index) = parent;
            index = parent_index;
        }
        else
            return;
    }
}

int search_binary(vector<node_t *> nodes, node_t value, bool (*func)(node_t, node_t))
{
    if (nodes.empty())
        return -1;
    int start = 0;
    int left = start;
    int right = nodes.size() - 1;
    int mid = ((right) - left) / 2 + left;

    while (left + 1 < right)
    {
        mid = ((right) - left) / 2 + left;
        if (func(value, *nodes.at(mid)))
            right = mid;
        else
            left = mid;
    }
    if (nodes.at(mid)->coord != value.coord)
        return -1;

    return (int) mid;
}

typedef struct graph
{
    vector<node_t *> nodes;

    void add_node(node_t node)
    {
        if (node.pipe == GROUND)
            return;
        node_t *current = nullptr;
        int index = search_binary(nodes, node, [](node_t a, node_t b) { return a.coord < b.coord; });
        if (index >= 0)
        {
            current = nodes.at(index);
            current->pipe = node.pipe;
        }
        else
        {
            current = new node_t(node);
            nodes.emplace_back(current);
            std::sort(nodes.begin(), nodes.end(), [](node_t *a, node_t *b) { return a->coord < b->coord; });
        }

        vector<node_t> neighbours {};
        switch (current->pipe)
        {
            case VERTICAL:
                neighbours.emplace_back(node_t(coord_t(current->coord.row + 1, current->coord.col), GROUND));
                neighbours.emplace_back(node_t(coord_t(current->coord.row - 1, current->coord.col), GROUND));
                break;
            case HORIZONTAL:
                neighbours.emplace_back(node_t(coord_t(current->coord.row, current->coord.col + 1), GROUND));
                neighbours.emplace_back(node_t(coord_t(current->coord.row, current->coord.col - 1), GROUND));
                break;
            case NORTH_EAST:
                neighbours.emplace_back(node_t(coord_t(current->coord.row - 1, current->coord.col), GROUND));
                neighbours.emplace_back(node_t(coord_t(current->coord.row, current->coord.col + 1), GROUND));
                break;
            case NORTH_WEST:
                neighbours.emplace_back(node_t(coord_t(current->coord.row - 1, current->coord.col), GROUND));
                neighbours.emplace_back(node_t(coord_t(current->coord.row, current->coord.col - 1), GROUND));
                break;
            case SOUTH_WEST:
                neighbours.emplace_back(node_t(coord_t(current->coord.row + 1, current->coord.col), GROUND));
                neighbours.emplace_back(node_t(coord_t(current->coord.row, current->coord.col - 1), GROUND));
                break;
            case SOUTH_EAST:
                neighbours.emplace_back(node_t(coord_t(current->coord.row + 1, current->coord.col), GROUND));
                neighbours.emplace_back(node_t(coord_t(current->coord.row, current->coord.col + 1), GROUND));
                break;
            case GROUND:
                break;
            case STARTING:
                break;
        }

        vector<node_t *> actual_neighbours {};
        for (auto ne: neighbours)
        {
            bool exists = false;
            for (auto n: nodes)
            {
                if (n->coord == ne.coord)
                {
                    actual_neighbours.emplace_back(n);
                    exists = true;
                    break;
                }
            }
            if (!exists)
            {
                node_t *new_node = new node_t(ne);
                nodes.emplace_back(new_node);
                actual_neighbours.emplace_back(new_node);
            }
        }

        for (auto a: actual_neighbours)
        {
            if (std::find(current->nodes.begin(), current->nodes.end(), a) == current->nodes.end())
                current->nodes.emplace_back(a);
            if (a->pipe == STARTING)
                if (std::find(a->nodes.begin(), a->nodes.end(), current) == a->nodes.end())
                    a->nodes.emplace_back(current);
        }
    }

    void remove_node(node_t *node)
    {
        auto itr = std::find(nodes.begin(), nodes.end(), node);

        if (itr == nodes.end())
        {
            cout << "Did not find node to remove" << endl;
            exit(1);
        }

        node_t *current = *itr;

        vector<node_t *> neighbours = current->nodes;

        for (auto n: neighbours)
        {
            auto pos = std::find(n->nodes.begin(), n->nodes.end(), current);
            if (pos == n->nodes.end())
            {
                cout << "Did not find node to remove in the neighbours list" << endl;
                exit(1);
            }

            n->nodes.erase(pos);
        }

        nodes.erase(itr);
        delete node;
    }

    /**
     * This method should only leave the main loop left, all other pipes should be removed.
     */
    void clean()
    {
        for (auto itr = nodes.begin(); itr != nodes.end(); itr++)
        {
            node_t *n = *itr;
            for (auto neighbour: n->nodes)
            {
                // The neighbour does not point to current
                if (std::find(neighbour->nodes.begin(), neighbour->nodes.end(), n) == neighbour->nodes.end())
                {
                    // Remove point
                    n->nodes.erase(std::find(n->nodes.begin(), n->nodes.end(), neighbour));
                    --itr;
                    break;
                }
            }
        }

        bool changed = true;
        while (changed)
        {
            changed = false;
            for (auto node: nodes)
            {
                if (node->nodes.size() < 2)
                {
                    remove_node(node);
                    changed = true;
                    break;
                }
            }
        }
    }

    /**
     * Runs a BFS search returning the maximum distance it finds.
     */
    uint64_t bfs() const
    {
        vector<node_t *> found {};

        //make_heap(found.begin(), found.end());
        node_t *start = nullptr;
        for (auto n: nodes)
            if (n->pipe == STARTING)
                start = n;
        if (start == nullptr)
        {
            cout << "Start should be found" << endl;
            exit(1);
        }
        start->visited = true;
        start->distance = 0;

        found.emplace_back(start);
        std::push_heap(found.begin(), found.end());

        while (!found.empty())
        {
            auto current = found.front();
            std::pop_heap(found.begin(), found.end());
            found.pop_back();

            for (auto n: current->nodes)
            {
                if (n->visited)
                    continue;

                found.emplace_back(n);
                n->visited = true;
                n->distance = current->distance + 1;
                //push_heap(found.begin(), found.end());
                sort_heap_back(found, [](node_t *a, node_t *b) { return a->distance < b->distance; });
                //sort_heap(found.begin(), found.end(), [](const node_t *a, const node_t *b){return a->distance < b->distance;});
            }
        }

        uint64_t max {};
        for (auto n: nodes)
            if (n->distance > max)
                max = n->distance;

        return max;
    }

    void print() const
    {
        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                node_t *current = nullptr;
                for (auto n: nodes)
                    if (n->coord == coord_t(row, col))
                    {
                        current = n;
                        break;
                    }
                if (current == nullptr)
                {
                    cout << ".";
                }
                else
                {
                    cout << current->distance;
                }
            }
            cout << endl;
        }
    }
} graph_t;

uint64_t find_furthest_distance(graph_t &maze)
{
    maze.clean();
    return maze.bfs();
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
    graph_t maze;
    uint64_t row {};
    while (!file.eof())
    {
        getline(file, temp);
        //Parse input
        for (auto p: parse_line_to_nodes(temp, row))
            maze.add_node(p);
        row++;
    }

    file.close();

    // Part 1
    cout << "The furthest from the start S is: " << find_furthest_distance(maze) << endl;
    maze.print();

    // Part 2

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
