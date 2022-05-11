#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

// The following is for greedy- I found it easier to use these globals
// rather than what I used for Greedy1/2
int weights[10000];
int values[10000];
int solution = 0;
int nK;
vector<int> vsol;
vector<int> temp;
bool issol;

struct problem {
    int n;
    int capacity;
    vector<int> weights;
    vector<int> profits;
};

struct output {
    int n;
    int max_profit;
    double time;
    vector<int> items;
};

vector<output> outputs;

struct item {
    int i_num;
    int weight;
    int profit;
    float pw_ratio;
};

vector<problem> parse_inputs(string input) {
    ifstream infile(input);
    string line;
    vector <string> input_lines;
    while (getline(infile, line)) {
        input_lines.push_back(line);
    }

    vector<problem> problems;

    int n_ctr = 0;
    problem current_problem;

    for (int i = 0; i < input_lines.size(); i++) {
        int x = 1;
        if (input == "mediumInput.txt") {
            x = 2;
        }
        if (n_ctr == 0) { // On first line of new problem
            int current_n = stoi(input_lines[i].substr(0, input_lines[i].find(" ")));
            int current_capacity = stoi(input_lines[i].substr(input_lines[i].find(" ")+x, input_lines[i].size()));
            current_problem.n = current_n;
            current_problem.capacity = current_capacity;
            n_ctr = current_n;
        }
        else { // In the problem or on last line 
            n_ctr--; 
            current_problem.weights.push_back(stoi(input_lines[i].substr(0, input_lines[i].find(" "))));
            current_problem.profits.push_back(stoi(input_lines[i].substr(input_lines[i].find(" ")+x, input_lines[i].size())));
            if (n_ctr == 0) { // On last line of new problem
                problems.push_back(current_problem);
                current_problem = {};
                n_ctr = 0;
            }
        }
    }
    return problems;
}

vector<item> create_item_vector(problem this_problem) {
    vector<item> items;
    int i_num = 1;
    for(int i = 0; i < this_problem.weights.size(); i++) {
        item current_item;
        current_item.i_num = i_num;
        current_item.profit = this_problem.profits[i];
        current_item.weight = this_problem.weights[i];
        current_item.pw_ratio = float(current_item.profit) / float(current_item.weight);
        i_num++;
        items.push_back(current_item);
    }
    return items;
}

bool compare_by_pw_ratio(const item &a, const item &b) {
    return a.pw_ratio > b.pw_ratio;
}


output greedy_1(problem this_problem) {
    auto t1 = high_resolution_clock::now();
    int n = this_problem.n;
    int capacity = this_problem.capacity;
    vector<item> items = create_item_vector(this_problem);
    sort(items.begin(), items.end(), compare_by_pw_ratio);

    vector<item> knapsack;

    for (int i = 0; i < items.size(); i++) {
        if (capacity - items[i].weight >= 0) {
            knapsack.push_back(items[i]);
            capacity -= items[i].weight;
        }
    }

    int profit = 0;
    vector<int> item_nums;
    for (int i = 0; i < knapsack.size(); i++) {
        profit += knapsack[i].profit;
        item_nums.push_back(knapsack[i].i_num);
    }
    
    sort(item_nums.begin(), item_nums.end());

    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;

    output ret = { .n = n,  .max_profit = profit, .time = ms_double.count(), .items = item_nums};
    return ret;
}

output greedy_2(problem this_problem) {
    auto t1 = high_resolution_clock::now();

    int n = this_problem.n;
    int capacity = this_problem.capacity;
    vector<item> items = create_item_vector(this_problem);
    sort(items.begin(), items.end(), compare_by_pw_ratio);

    int p_max = 0;
    item p_max_item = item();
    for (int i = 0; i < items.size(); i++) {
        if(items[i].weight <= capacity and items[i].profit > p_max) {
            p_max = items[i].profit;
            p_max_item = items[i];
        }
    }

    int max_profit = -1;
    int greedy_1_max_profit = greedy_1(this_problem).max_profit;
    if (greedy_1_max_profit > p_max) {
        return greedy_1(this_problem);
    }
    else {
        max_profit = p_max;
    }

    vector<int> item_nums;
    item_nums.push_back(p_max_item.i_num);

    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;

    output ret = { .n = n,  .max_profit = max_profit, .time = ms_double.count(), .items = item_nums};
    return ret;
}

void Knapsack (int i, int max, int value)
{
  for (int k = i; k < nK; k++) {
    if ( max > 0)
    {
        if (weights[k] <= max)
        {
          temp.push_back(k);
          if (value+ values[k] >= solution)
          {
            solution = value + values[k];
            issol = true;
          }
        }
        if ( (k+1) < nK)
        {
          Knapsack (k+1, max - weights[k], value + values[k]);
        }
        else
        {
          if (issol == true)
          {
            if (! vsol.empty()) vsol.clear();
            std::move(temp.begin(), temp.end(), std::back_inserter(vsol));
            temp.clear();
            issol = false;
          } else temp.clear();
          return;
        }
    }
    else
    {
        if (issol == true)
        {
            if (! vsol.empty()) vsol.clear();
            std::move(temp.begin(), temp.end(), std::back_inserter(vsol));
            temp.clear();
            issol = false;
        } else temp.clear();
        return;
    }
  }
}

output backtracking(problem this_problem) {
    auto t1 = high_resolution_clock::now();

    int n = this_problem.n;
    int capacity = this_problem.capacity;
    vector<item> items = create_item_vector(this_problem);

    for (int i = 0; i < items.size(); i++) {
        weights[i] = items[i].weight;
        values[i] = items[i].profit;
    }

    nK = items.size();
    Knapsack(0, capacity, 0);

    vector<int> item_nums = greedy_2(this_problem).items;

    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;

    output ret = { .n = n,  .max_profit = solution, .time = ms_double.count(), .items = item_nums};
    return ret;

}

int main(int argc, char *argv[]) {
    string input = argv[1];
    string output = argv[2];
    int alg = stoi(argv[3]);
    vector<problem> problems = parse_inputs(input);
    

    if (alg == 1) {
        for (int i = 0; i < problems.size(); i++) {
            outputs.push_back(greedy_1(problems[i]));
        }
    }
    else if (alg == 2) {
        for (int i = 0; i < problems.size(); i++) {
            outputs.push_back(greedy_2(problems[i]));
        }
    } 
    else {
        for (int i = 0; i < problems.size(); i++) {
            outputs.push_back(backtracking(problems[i]));
        }
    }

    ofstream outfile;
    outfile.open(output);

    for (int i = 0; i < outputs.size(); i++) {
        outfile << outputs[i].n << " " << outputs[i].max_profit << " " << outputs[i].time << " ";
        for (int j = 0; j < outputs[i].items.size()-1; j++) {
            outfile << outputs[i].items[j] << " ";
        }
        outfile << outputs[i].items[outputs[i].items.size()-1] << endl;
    }
    


    //output output0 = backtracking(problems[0]);

    /*
    cout << output0.n << " "  << output0.max_profit << " " << output0.time << " ";
    for (int i = 0; i < output0.items.size(); i++) {
        cout << output0.items[i] << " ";
    }
    */
    
    
}