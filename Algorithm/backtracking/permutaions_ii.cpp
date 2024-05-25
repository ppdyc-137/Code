/* 输入一个整数数组，数组中可能包含重复元素，返回所有不重复的排列。 */

#include <vector>
#include <iostream>
#include <unordered_set>

using namespace std;

void backtrack(vector<int>& state, vector<int>& choices, vector<bool>& selected,
               vector<vector<int>>& res)
{
    if (state.size() == choices.size()) {
        res.push_back(state);
        return;
    }

    unordered_set<int> duplicated;
    for (int i = 0; i < choices.size(); i++) {
        int choice = choices[i];
        if (selected[i] || duplicated.find(choice) != duplicated.end())
            continue;

        selected[i] = true;
        duplicated.emplace(choice);
        state.push_back(choice);
        backtrack(state, choices, selected, res);
        selected[i] = false;
        state.pop_back();
    }

}

vector<vector<int>> permutation(vector<int> nums)
{
    vector<int> state;
    vector<bool> selected(nums.size(), false);
    vector<vector<int>> res;
    backtrack(state, nums, selected, res);
    return res;
}

int main()
{
    vector<int> nums{1, 1, 2, 3};
    auto res = permutation(nums);
    for (auto i : res) {
        for (auto j : i) {
            cout << j << " ";
        }
        cout << "\n";
    }
}
