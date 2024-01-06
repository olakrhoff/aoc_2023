#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

#define DEBUG 0
#define PART_1 false
#define MIN_VALUE 1
#define MAX_VALUE 4000

typedef enum CATEGORIES
{
    X,
    M,
    A,
    S
} CATEGORIES_T;

typedef struct part
{
    uint64_t data[4];

    inline uint64_t sum() const
    {
        return data[0] + data[1] + data[2] + data[3];
    }
} part_t;

typedef struct rule
{
    bool needs_process {true};
    CATEGORIES_T category;
    bool less_than;
    uint64_t compare_number;
    string destination;

    optional<string> handle(const part_t &part) const
    {
        if (!needs_process)
            return destination;
        uint64_t value = part.data[category];

        if (less_than)
        {
            if (value < compare_number)
                return destination;
            else
                return nullopt;
        }

        if (value > compare_number)
            return destination;
        return nullopt;
    }

    tuple<part_t, part_t, uint64_t, part_t, part_t, uint64_t, string> process_part(const part_t &lower_part, const part_t &upper_part) const
    {
        part_t passed_lower, passed_upper, failed_lower, failed_upper;
        uint64_t passed {}, failed {};

        if (!needs_process)
        {
            passed = upper_part.data[category] - lower_part.data[category] + 1;
            passed_lower = lower_part;
            passed_upper = upper_part;

            return {passed_lower, passed_upper, passed, failed_lower, failed_upper, 0, destination};
        }

        if (less_than)
        {
            // Everything fails
            if (lower_part.data[category] >= compare_number)
            {
                failed = upper_part.data[category] - lower_part.data[category] + 1;
                failed_lower = lower_part;
                failed_upper = upper_part;
                return {passed_lower, passed_upper, 0, failed_lower, failed_upper, failed, destination};
            }
            // Everything passed
            else if (upper_part.data[category] < compare_number)
            {
                passed = upper_part.data[category] - lower_part.data[category] + 1;
                passed_lower = lower_part;
                passed_upper = upper_part;
                return {passed_lower, passed_upper, passed, failed_lower, failed_upper, 0, destination};
            }
            // The compare number is between the lower and upper limit, and we get a split in the number of passes and fails
            else
            {
                passed = compare_number - lower_part.data[category];
                passed_lower = lower_part;
                passed_upper = upper_part;
                passed_upper.data[category] = compare_number - 1;

                failed = upper_part.data[category] - compare_number + 1;
                failed_lower = lower_part;
                failed_lower.data[category] = compare_number;
                failed_upper = upper_part;

                return {passed_lower, passed_upper, passed, failed_lower, failed_upper, failed, destination};
            }
        }
        else
        {
            // Everything fails
            if (upper_part.data[category] <= compare_number)
            {
                failed = upper_part.data[category] - lower_part.data[category] + 1;
                failed_lower = lower_part;
                failed_upper = upper_part;
                return {passed_lower, passed_upper, 0, failed_lower, failed_upper, failed, destination};
            }
            // Everything passed
            else if (lower_part.data[category] > compare_number)
            {
                passed = upper_part.data[category] - lower_part.data[category] + 1;
                passed_lower = lower_part;
                passed_upper = upper_part;
                return {passed_lower, passed_upper, passed, failed_lower, failed_upper, 0, destination};
            }
            // The compare number is between the lower and upper limit, and we get a split in the number of passes and fails
            else
            {
                passed = upper_part.data[category] - compare_number;
                passed_lower = lower_part;
                passed_lower.data[category] = compare_number + 1;
                passed_upper = upper_part;

                failed = compare_number - lower_part.data[category] + 1;
                failed_lower = lower_part;
                failed_upper = upper_part;
                failed_upper.data[category] = compare_number;

                return {passed_lower, passed_upper, passed, failed_lower, failed_upper, failed, destination};
            }
        }

    }
} rule_t;

typedef struct workflow
{
    string name {};
    vector<rule_t> rules {};
} workflow_t;


vector<string> split(const string &line, const char delimiter)
{
    vector<string> res {};

    string temp = line;
    while (temp.find(delimiter) != -1)
    {
        res.emplace_back(temp.substr(0, temp.find(delimiter)));
        temp = temp.substr(temp.find(delimiter) + 1);
    }
    res.emplace_back(temp);

    return res;
}

workflow_t parse_workflow(const string &line)
{
    workflow_t workflow {};

    workflow.name = line.substr(0, line.find('{'));
    string rules = line.substr(line.find('{') + 1);
    rules = rules.substr(0, rules.length() - 1);

    vector<string> rules_split = split(rules, ',');

    for (auto &rule_str : rules_split)
    {
        rule_t rule;
        if (rule_str.find(':') == -1)
        {
            rule.needs_process = false;
            rule.destination = rule_str;
            workflow.rules.emplace_back(rule);
            continue;
        }
        switch (rule_str.at(0))
        {
            case 'x':
                rule.category = X;
                break;
            case 'm':
                rule.category = M;
                break;
            case 'a':
                rule.category = A;
                break;
            case 's':
                rule.category = S;
                break;
            default:
                cout << "Should be able to find category" << endl;
                exit(EXIT_FAILURE);
        }
        rule.less_than = rule_str.at(1) == '<';
        rule.compare_number = static_cast<uint64_t>(stoi(rule_str.substr(2, rule_str.find(':') - 2)));
        rule.destination = rule_str.substr(rule_str.find(':') + 1);
        workflow.rules.emplace_back(rule);
    }

    return workflow;
}

part_t parse_part(const string &line)
{
    part_t part;
    string trimmed_line = line.substr(1, line.length() - 2);
    vector<string> data_for_part = split(trimmed_line, ',');

    for (int i = 0; i < 4; ++i)
        part.data[i] = stoi(data_for_part.at(i).substr(2));

    return part;
}

uint64_t sort_part(const part_t &part, const map<string, workflow_t> &workflows, const string &start_workflow = "in")
{
    if (start_workflow == "A")
        return part.sum();
    if (start_workflow == "R")
        return 0;

    for (auto rule : workflows.at(start_workflow).rules)
    {
        auto res = rule.handle(part);
        if (res.has_value())
            return sort_part(part, workflows, res.value());
    }

}

uint64_t sum_parts(const vector<part_t> &parts, const map<string, workflow_t> &workflows)
{
    uint64_t sum {};

    for (auto part : parts)
        sum += sort_part(part, workflows);

    return sum;
}

uint64_t find_permutations(const map<string, workflow_t> &workflows, const string &current_workflow = "in",
                           const int current_rule = 0, part_t current_lower_limits = {MIN_VALUE, MIN_VALUE, MIN_VALUE, MIN_VALUE},
                           part_t current_upper_limits = {MAX_VALUE, MAX_VALUE, MAX_VALUE, MAX_VALUE})
{
    if (current_workflow == "A")
    {
        //return 1;
        return (current_upper_limits.data[X] - current_lower_limits.data[X] + 1) *
               (current_upper_limits.data[M] - current_lower_limits.data[M] + 1) *
               (current_upper_limits.data[A] - current_lower_limits.data[A] + 1) *
               (current_upper_limits.data[S] - current_lower_limits.data[S] + 1);
    }
    if (current_workflow == "R")
        return 0;

    workflow_t current = workflows.at(current_workflow);
    part_t passed_lower, passed_upper, failed_lower, failed_upper;
    uint64_t passed, failed;
    string redirect;

    tie(passed_lower, passed_upper, passed, failed_lower, failed_upper, failed, redirect) = current.rules.at(current_rule).process_part(current_lower_limits, current_upper_limits);

    uint64_t passed_vals {}, failed_vals {};
    if (passed > 0)
        passed_vals = find_permutations(workflows, redirect, 0, passed_lower, passed_upper);
    if (failed > 0)
        failed_vals = find_permutations(workflows, current_workflow, current_rule + 1, failed_lower, failed_upper);

    return passed_vals + failed_vals;
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
    map<string, workflow_t> workflows {};
    vector<part_t> parts {};
    bool parsed_first {false};
    while (!file.eof())
    {
        getline(file, temp);
        if (temp.empty())
        {
            parsed_first = true;
            continue;
        }
        if (!parsed_first)
        {
            auto workflow = parse_workflow(temp);
            workflows.emplace(workflow.name, workflow);
        }
        else
            parts.emplace_back(parse_part(temp));
    }

    file.close();

    // Part 1
    if (PART_1)
        cout << "Sum of all accepted parts: " << sum_parts(parts, workflows) << endl;
    // Part 2
    else
        cout << "Number of possible accepted parts: " << find_permutations(workflows) << endl;

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

    cout << "Time taken: " << duration.count() / 1000 << " milliseconds" << endl;

    return EXIT_SUCCESS;
}
