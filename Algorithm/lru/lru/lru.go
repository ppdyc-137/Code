package main

import (
	"container/list"
	"fmt"
)

type LRU struct {
	cap     int
	hashmap map[int]*list.Element
	list    *list.List
}

type Node struct {
	key, value int
}

func (lru *LRU) Get(key int) int {
	ele, exists := lru.hashmap[key]
	if !exists {
		return -1
	}
	lru.list.MoveToFront(ele)
	node := ele.Value.(*Node)
	return node.value
}

func (lru *LRU) Set(key, value int) {
	ele, exists := lru.hashmap[key]
	if exists {
		lru.list.MoveToFront(ele)
		node := ele.Value.(*Node)
		node.value = value
		return
	}

	if lru.list.Len() >= lru.cap {
		ele := lru.list.Back()
		delete(lru.hashmap, ele.Value.(*Node).key)
		lru.list.Remove(ele)
	}

	ele = lru.list.PushFront(&Node{key, value})
	lru.hashmap[key] = ele
}

func main() {
	lru := LRU{
		cap:     2,
		hashmap: make(map[int]*list.Element),
		list:    list.New(),
	}
	lru.Set(1, 11)
	lru.Set(2, 22)
	fmt.Println(lru.Get(1))
	lru.Set(3, 33)

	fmt.Println(lru.Get(2))
}
