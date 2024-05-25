/* 给定一个正整数数组 nums 和一个目标正整数 target
 * ，请找出所有可能的组合，使得组合中的元素和等于 target
 * 。给定数组可能包含重复元素，每个元素只可被选择一次。请以列表形式返回这些组合，列表中不应包含重复组合。
 */

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

void backtrack(vector<int>& state, int target, int start, vector<int>& choices,
               vector<vector<int>>& res)
{
    if (target == 0) {
        res.push_back(state);
        return;
    }

    for (int i = start; i < choices.size(); i++) {
        int choice = choices[i];
        if (target - choice < 0)
            break;

        if (i > start && choices[i] == choices[i - 1])
            continue;

        state.push_back(choices[i]);
        backtrack(state, target - choice, i + 1, choices, res);
        state.pop_back();
    }
}

vector<vector<int>> subsetSum(vector<int> nums, int target)
{
    vector<int> state;
    vector<vector<int>> res;
    sort(nums.begin(), nums.end());
    backtrack(state, target, 0, nums, res);
    return res;
}

int main()
{
    vector<int> nums{1, 1, 2, 2, 3};
    int target = 4;
    auto res = subsetSum(nums, target);
    for (auto i : res) {
        for (auto j : i) {
            cout << j << " ";
        }
        cout << "\n";
    }
}
