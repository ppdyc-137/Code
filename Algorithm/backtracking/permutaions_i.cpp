/* 输入一个整数数组，数组中不包含重复元素，返回所有可能的排列。 */

#include <vector>
#include <iostream>

using namespace std;

void backtrack(vector<int>& state, vector<int>& choices, vector<bool>& selected,
               vector<vector<int>>& res)
{
    if (state.size() == choices.size()) {
        res.push_back(state);
        return;
    }

    for (int i = 0; i < choices.size(); i++) {
        if (selected[i])
            continue;

        selected[i] = true;
        state.push_back(choices[i]);
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
    vector<int> nums{1, 2, 3};
    auto res = permutation(nums);
    for (auto i : res) {
        for (auto j : i) {
            cout << j << " ";
        }
        cout << "\n";
    }
}
