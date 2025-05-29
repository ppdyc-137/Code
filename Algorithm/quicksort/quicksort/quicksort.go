package main

import "fmt"

func partition(nums []int) int {
	n := len(nums)
	pivot := nums[n-1]
	l := 0
	for r := range n - 1 {
		if nums[r] <= pivot {
			nums[l], nums[r] = nums[r], nums[l]
			l++
		}
	}
	nums[l], nums[n-1] = nums[n-1], nums[l]
	return l
}

func quicksort(nums []int) {
	if len(nums) <= 1 {
		return
	}
	pivot := partition(nums)
	if pivot > 0 {
		quicksort(nums[:pivot])
	}
	if pivot <= len(nums) {
		quicksort(nums[pivot+1:])
	}
}

func findKthLargest(nums []int, k int) int {
	pivot := partition(nums)
	if pivot == k-1 {
		return nums[pivot]
	} else if pivot < k-1 {
		return findKthLargest(nums[pivot+1:], k-pivot-1)
	} else {
		return findKthLargest(nums[:pivot], k)
	}
}

func main() {
	{
		nums := []int{3, 2, 1, 8, 9, 11}
		fmt.Println(findKthLargest(nums, 2))
	}
	{
		nums := []int{1, 2, 3, 4}
		quicksort(nums)
		fmt.Println(nums)
	}
	{
		nums := []int{4, 3, 2, 1}
		quicksort(nums)
		fmt.Println(nums)
	}
	{
		nums := []int{1, 1, 1, 1, 1}
		quicksort(nums)
		fmt.Println(nums)
	}
}
