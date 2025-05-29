#include <iostream>
#include <utility>
#include <vector>

using namespace std;

void heap_sink(auto& nums, int node, int len) {
    while (node < len) {
        int left = node * 2 + 1;
        int right = left + 1;

        int max = node;

        if (left < len && nums[left] > nums[max]) {
            max = left;
        }
        if (right < len && nums[right] > nums[max]) {
            max = right;
        }
        if (max == node) {
            break;
        }

        swap(nums[node], nums[max]);
        node = max;
    }
}

void make_heap(vector<int>& nums) {
    int sz = nums.size();
    for (int node = sz / 2 - 1; node >= 0; node--) {
        heap_sink(nums, node, sz);
    }
}

void heapsort(vector<int>& nums) {
    make_heap(nums);

    int sz = nums.size();
    while (sz > 0) {
        swap(nums[0], nums[sz - 1]);
        sz--;
        heap_sink(nums, 0, sz);
    }
}

int main() {
    {
        vector<int> nums = {5, 2, 6};
        heapsort(nums);
        for (auto n : nums) {
            std::cout << n << " ";
        }
        std::cout << '\n';
    }
}
