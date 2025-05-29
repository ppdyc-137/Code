#include <iostream>
#include <utility>
#include <vector>

int partition(auto& nums, int low, int high) {
    int pivot = nums[high];
    int l = low;
    for (int r = low; r <= high - 1; r++) {
        if (nums[r] <= pivot) {
            std::swap(nums[l], nums[r]);
            l++;
        }
    }
    std::swap(nums[l], nums[high]);
    return l;
}

void helper(auto& nums, int low, int high) {
    if (low >= high) {
        return;
    }
    int pivot = partition(nums, low, high);
    helper(nums, low, pivot - 1);
    helper(nums, pivot + 1, high);
}

void quicksort(std::vector<int>& nums) { helper(nums, 0, nums.size() - 1); }

int quickselect(std::vector<int>& nums, int low, int high, int k) {
    if (low == high) {
        return nums[low];
    }
    int pivot = partition(nums, low, high);
    if (pivot == k - 1) {
        return nums[pivot];
    } else if (pivot < k - 1) {
        return quickselect(nums, pivot + 1, high, k);
    } else {
        return quickselect(nums, low, pivot - 1, k);
    }
}

int findKthLargest(std::vector<int>& nums, int k) { return quickselect(nums, 0, nums.size() - 1, nums.size() - k + 1); }

int main() {
    {
        std::vector<int> nums = {3, 2, 1, 1, 8, 9, 11};
        quicksort(nums);
        for (auto num : nums) {
            std::cout << num << " ";
        }
        std::cout << '\n';
    }
    {
        std::vector<int> nums = {1, 1, 1};
        quicksort(nums);
        for (auto num : nums) {
            std::cout << num << " ";
        }
        std::cout << '\n';
    }
    {
        std::vector<int> nums = {3, 2, 1};
        quicksort(nums);
        for (auto num : nums) {
            std::cout << num << " ";
        }
        std::cout << '\n';
    }
}
