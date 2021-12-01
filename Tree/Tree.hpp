#pragma once
#include <cassert>
#include <iostream>
#include <limits>
#include <stack>
#include <string>

#define UNRECHEABLE assert(!"This line should be unrecheable")

namespace Containers {
class Tree {
  public:
    Tree() {}

    Tree(const Tree &rhs) {
        if (!rhs.root)
            return;
        root = Node::get_copy(rhs.root);
        copy_tree_impl(rhs.root, root);
    }

    Tree &operator=(const Tree &rhs) {
        if (this == &rhs)
            return *this;
        Tree tmp(rhs);
        std::swap(tmp.root, root);
    }

    Tree(Tree&& rhs) {
        std::swap(root, rhs.root);
    }

    Tree &operator=(Tree &&rhs) {
        std::swap(root, rhs.root);
    }

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

        int bound = prev_pos + (prev->key < key ? 0 : -1);
        bound = !node ? bound : cur_pos - 1;
        return bound;
    }

    ~Tree() {
        std::stack<Node *> stack;
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

    int nth(int k) const {
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
    struct Node {
      public:
        Node(int key_) : key(key_) {}
        int key = 0;

        Node *left = nullptr;
        Node *right = nullptr;

        static Node *get_copy(const Node *this_) {
            if (!this_)
                return nullptr;
            Node *res = new Node(this_->key);
            return res;
        }
        static ssize_t get_height(const Node *this_) { return this_ ? this_->height : 0; }
        static ssize_t get_count(const Node *this_) { return this_ ? this_->count : 0; }
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
        static ssize_t height_diff(Node *this_) {
            return this_ ? get_height(this_->right) - get_height(this_->left) : 0;
        }

      private:
        ssize_t height = 1;
        ssize_t count = 1;
    };
    Node *root = nullptr;

    void copy_tree_impl(Node *original, Node *copied) {
        if (!original)
            return;

        std::stack<Node *> stack_orig;
        std::stack<Node *> stack_copy;
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
                  << " count = " << Node::get_count(node) << " }" << std::endl;

        print_impl(prefix + (is_left ? "│   " : "    "), node->left, 1);
        print_impl(prefix + (is_left ? "│   " : "    "), node->right, 0);
    }

    Node *insert_impl(Node *root, int k) {
        if (!root)
            return new Node{k};

        if (k < root->key)
            root->left = insert_impl(root->left, k);
        else
            root->right = insert_impl(root->right, k);

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
            if (Node::height_diff(root->right) < 0)
                root->right = rotate_right(root->right);
            return rotate_left(root);
        }
        if (height_diff == -2) {
            if (Node::height_diff(root->left) > 0)
                root->left = rotate_left(root->left);
            return rotate_right(root);
        }
        return root;
    }
};
}; // namespace Containers
