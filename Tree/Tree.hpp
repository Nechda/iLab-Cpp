#pragma once
#include <cassert>
#include <iostream>
#include <limits>
#include <stack>
#include <string>

#define UNRECHEABLE assert(!"This line should be unrecheable")

namespace Containers {

struct Node {
  public:
    int key = 0;
    Node *left = nullptr;
    Node *right = nullptr;

  public:
    Node(int key_) : key(key_) {}

    static ssize_t height_diff(Node *this_) { return this_ ? get_height(this_->right) - get_height(this_->left) : 0; }
    static Node *get_copy(const Node *this_) {
        if (!this_)
            return nullptr;
        Node *res = new Node(this_->key);
        return res;
    }
    static void fix(Node *this_) {
        if (!this_)
            return;

        auto &left = this_->left;
        auto &right = this_->right;

        auto left_hegiht = get_height(left);
        auto right_height = get_height(right);
        auto left_count = get_count(left);
        auto right_count = get_count(right);

        this_->height = std::max(left_hegiht, right_height) + 1;
        this_->count = left_count + right_count + 1;
    }
    static size_t get_height(const Node *this_) { return this_ ? this_->height : 0; }
    static size_t get_count(const Node *this_) { return this_ ? this_->count : 0; }

  private:
    ssize_t height = 1;
    size_t count = 1;
};

struct tree_container {
  protected:
    tree_container() {}

    tree_container &operator=(const tree_container &) = delete;
    tree_container(const tree_container &) = delete;

    tree_container &operator=(tree_container &&rhs) noexcept {
        std::swap(root, rhs.root);
        return *this;
    }
    tree_container(tree_container &&rhs) noexcept : root(rhs.root) { rhs.root = nullptr; }

    ~tree_container() {
        auto &stack = stack_s0_;
        stack.clear();
        stack.push(root);
        while (!stack.empty()) {
            auto v = stack.top();
            stack.pop();
            if (v != nullptr) {
                stack.push(v->left);
                stack.push(v->right);
            }
            delete v;
        }
    }

    Node *root = nullptr;

    struct FlatStack {
        Node *&top() { return data_[idx_ - 1]; }
        void push(Node *item) {
            data_[idx_] = item;
            idx_++;
        }
        void pop() { idx_--; }
        bool empty() { return idx_ == 0; }
        void clear() { idx_ = 0; }

      private:
        Node *data_[sizeof(size_t) << 3U] = {};
        size_t idx_ = 0;
    };
    static FlatStack stack_s0_;
    static FlatStack stack_s1_;
};

class Tree : public tree_container {
  public:
    Tree() {}

    Tree(const Tree &rhs) {
        root = Node::get_copy(rhs.root);
        copy_tree_impl(rhs.root, root);
    }
    Tree &operator=(const Tree &rhs) {
        Tree tmp(rhs);
        std::swap(*this, tmp);
        return *this;
    }

    Tree(Tree &&rhs) = default;
    Tree &operator=(Tree &&rhs) = default;

    void insert(int key) { root = insert_impl(root, key); }

    void dump() const { print_impl("", root, 0); }

    void balance() { root = balance_impl(root); }

    int upper_bound(int key) const {
        Node *node = root;
        Node *prev = nullptr;
        size_t cur_pos = Node::get_count(node->left) + 1;
        size_t prev_pos = 0;

        do {
            if (node->key > key) {
                prev = node;
                node = node->left;
                prev_pos = cur_pos;
                cur_pos -= (node ? Node::get_count(node->right) : 0) + 1;
                continue;
            }
            if (node->key < key) {
                prev = node;
                node = node->right;
                prev_pos = cur_pos;
                cur_pos += (node ? Node::get_count(node->left) : 0) + 1;
                continue;
            }
        } while (node ? node->key != key : 0);

        if (node == root)
            return cur_pos - 1;

        int bound = prev_pos + (prev->key < key ? 0 : -1);
        bound = !node ? bound : cur_pos - 1;
        return bound;
    }

    int nth(size_t k) const {
        if (k > Node::get_count(root))
            return std::numeric_limits<int>::max();

        Node *node = root;
        size_t cur_pos = Node::get_count(node->left) + 1;
        do {
            if (cur_pos > k) {
                node = node->left;
                cur_pos -= Node::get_count(node->right) + 1;
                // now it root of left subtree
                continue;
            }
            if (cur_pos < k) {
                node = node->right;
                cur_pos += Node::get_count(node->left) + 1;
                // now it root of right subtree
                continue;
            }
        } while (cur_pos != k);

        return node->key;
    }

  private:
    void copy_tree_impl(Node *original, Node *copied) {
        if (!original)
            return;

        auto &stack_orig = tree_container::stack_s0_;
        auto &stack_copy = tree_container::stack_s1_;
        stack_orig.clear();
        stack_copy.clear();

        stack_orig.push(original);
        stack_copy.push(copied);
        while (!stack_orig.empty()) {
            auto v_orig = stack_orig.top();
            auto v_copy = stack_copy.top();
            stack_orig.pop();
            stack_copy.pop();
            if (v_orig != nullptr) {
                stack_orig.push(v_orig->left);
                stack_orig.push(v_orig->right);
                v_copy->left = Node::get_copy(v_orig->left);
                v_copy->right = Node::get_copy(v_orig->right);
                stack_copy.push(v_copy->left);
                stack_copy.push(v_copy->right);
            }
        }
    }

    void print_impl(const std::string &prefix, const Node *node, bool is_left) const {
        if (!node)
            return;

        std::cout << prefix;
        std::cout << (is_left ? "├──(L)" : "└──(R)");

        std::cout << "{ key = " << node->key << " | "
                  << " count = " << Node::get_count(node) << " | "
                  << " height = " << Node::get_height(node) << " }" << std::endl;

        print_impl(prefix + (is_left ? "│   " : "    "), node->left, 1);
        print_impl(prefix + (is_left ? "│   " : "    "), node->right, 0);
    }

    Node *insert_impl(Node *root, int k) {
        if (!root)
            return new Node{k};

        auto &path = tree_container::stack_s0_;
        path.clear();

        Node *cur = root;
        bool was_allocation = 0;
        while (k != cur->key) {
            path.push(cur);
            auto &child = k < cur->key ? cur->left : cur->right;
            if (!child) {
                child = new Node{k};
                was_allocation = 1;
                break;
            }
            cur = child;
        }

        if (!was_allocation)
            return root;

        Node *parent = nullptr;
        cur = path.top();
        path.pop();
        while (!path.empty()) {
            parent = path.top();
            path.pop();
            auto &cur_ref = parent->left == cur ? parent->left : parent->right;
            cur_ref = balance_impl(cur_ref);
            cur = parent;
        }

        return balance_impl(root);
    }

    Node *rotate_right(Node *rhs) {
        Node *lhs = rhs->left;
        rhs->left = lhs->right;
        lhs->right = rhs;
        Node::fix(rhs);
        Node::fix(lhs);
        return lhs;
    }

    Node *rotate_left(Node *lhs) {
        Node *rhs = lhs->right;
        lhs->right = rhs->left;
        rhs->left = lhs;
        Node::fix(rhs);
        Node::fix(lhs);
        return rhs;
    }

    Node *balance_impl(Node *root) {
        Node::fix(root);
        auto height_diff = Node::height_diff(root);
        if (height_diff == 2) {
            if (Node::height_diff(root->right) < 0) {
                root->right = rotate_right(root->right);
                Node::fix(root->right);
            }
            auto ret = rotate_left(root);
            Node::fix(ret);
            return ret;
        }
        if (height_diff == -2) {
            if (Node::height_diff(root->left) > 0) {
                root->left = rotate_left(root->left);
                Node::fix(root->left);
            }
            auto ret = rotate_right(root);
            Node::fix(ret);
            return ret;
        }
        return root;
    }
};
}; // namespace Containers
