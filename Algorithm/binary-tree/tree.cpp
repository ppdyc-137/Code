#include <queue>
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
};

void do_something(TreeNode* node) { (void)node; }

void DFS(TreeNode* node) {
    if (node == nullptr) {
        return;
    }

    // do_something(node);
    DFS(node->left);
    do_something(node);
    DFS(node->right);
    // do_something(node);
}

void BFS(TreeNode* node) {
    if (node == nullptr) {
        return;
    }

    int depth = 1;
    std::queue<TreeNode*> queue;
    queue.push(node);
    while (!queue.empty()) {
        auto size = queue.size();
        for (size_t i = 0; i < size; i++) {
            auto* node = queue.front();
            queue.pop();

            do_something(node);

            if (node->left) {
                queue.push(node->left);
            }
            if (node->right) {
                queue.push(node->right);
            }
        }
        depth++;
    }
}
