#pragma once
#include <iostream>
#include <string>
#include <cassert>
#include <limits>

#define UNRECHEABLE assert(!"This line should be unrecheable")

namespace Containers {
    class Tree {
        public:
            Tree() {};
            int count(int key) {
                //auto pnn = helper_find(key);
                //return !!pnn.second;
                return 0;
            }
            void insert(int key) {
                root = insert_impl(root, key);
            }

            void dump() {
                print_impl("", root, 0);
            }

            void balance() {
                root = balance_impl(root);
            }

            int nth(int k) {
                if(k > root->count)
                    return std::numeric_limits<int>::max();

                static auto safe_get_count = [](const Node* node) -> ssize_t {
                    return node ? node->count : 0;
                };

                Node* node = root;
                size_t cur_pos = safe_get_count(node->left) + 1;
                do{
                    if(cur_pos > k) {
                        node = node->left;
                        cur_pos -= safe_get_count(node->right) + 1;
                        // now it root of left subtree
                        continue;
                    }
                    if(cur_pos < k) {
                        node = node->right;
                        cur_pos += safe_get_count(node->left) + 1;
                        // now it root of right subtree
                        continue;
                    }
                }while(cur_pos != k);

                return node->key;
            }

        private:
            struct Node {
                int key = 0;
                ssize_t height = 1;
                ssize_t count = 1;
                Node* left = nullptr;
                Node* right = nullptr;
            };
            Node* root = nullptr;

            void print_impl(const std::string& prefix, const Node* node, bool is_left) {
                if(!node) return;

                std::cout << prefix;
                std::cout << (is_left ? "├──(L)" : "└──(R)" );

                std::cout << "{ key = " << node->key << " | " << " count = " << node->count << " }" << std::endl;

                print_impl(prefix + (is_left ? "│   " : "    "), node->left, 1);
                print_impl(prefix + (is_left ? "│   " : "    "), node->right, 0);
            }

            Node* insert_impl(Node* root, int k) {
                if(!root) return new Node{k};

                if(k < root->key) 
                    root->left = insert_impl(root->left, k);
                else
                    root->right = insert_impl(root->right, k);

                return balance_impl(root);
            }

            ssize_t get_height(Node* node) {
                return node ? node->height : 0;
            }
            ssize_t get_count(Node* node) {
                return node ? node->count : 0;
            }
            void fix(Node* node) {
                auto& left = node->left;
                auto& right = node->right;

                auto left_hegiht = get_height(left);
                auto right_height = get_height(right);
                auto left_count = get_count(left);
                auto right_count = get_count(right);

                node->height = std::max(left_hegiht, right_height) + 1;
                node->count = left_count + right_count + 1;
            }
            ssize_t height_diff(Node* node) {
                return node ? get_height(node->right) - get_height(node->left) : 0;
            }

            Node* rotate_right(Node* rhs) {
                Node* lhs = rhs->left;
                rhs->left = lhs->right;
                lhs->right = rhs;
                fix(rhs);
                fix(lhs);
                return lhs;
            }

            Node* rotate_left(Node* lhs) {
                Node* rhs = lhs->right;
                lhs->right = rhs->left;
                rhs->left = lhs;
                fix(rhs);
                fix(lhs);
                return rhs;
            }

            Node* balance_impl(Node* root) {
                fix(root);
                auto diff = height_diff(root);
                if(diff == 2) {
                    if(height_diff(root->right) < 0)
                        root->right = rotate_right(root->right);
                    return rotate_left(root);
                }
                if(diff == -2) {
                    if(height_diff(root->left) > 0)
                        root->left = rotate_left(root->left);
                    return rotate_right(root);
                }
                return root;
            }
    };
};




/*
            int nth(int k) {
                if(k > root->n_nodes)
                    return std::numeric_limits<int>::max();

                static auto safe_get_count = [](const Node* node) -> ssize_t {
                    return node ? node->n_nodes : 0;
                };

                Node* node = root;
                size_t cur_pos = safe_get_count(node->left) + 1;
                do{
                    if(cur_pos > k) {
                        node = node->left;
                        cur_pos -= safe_get_count(node->right) + 1;
                        // now it root of left subtree
                        continue;
                    }
                    if(cur_pos < k) {
                        node = node->right;
                        cur_pos += safe_get_count(node->left) + 1;
                        // now it root of right subtree
                        continue;
                    }
                }while(cur_pos != k);

                return node->key;
            }
            */



/*
            std::pair<Node*, Node*> get_left_side(Node* node) {
                Node* prev = nullptr;
                while(node && node->left) {
                    prev = node;
                    node = node->left;
                }
                return {prev, node};
            }

            std::pair<Node*, Node*> get_right_side(Node* node) {
                Node* prev = nullptr;
                while(node && node->right) {
                    prev = node;
                    node = node->right;
                }
                return {prev, node};
            }

            void increase_count(Node* node, int key) {
                do{
                    node->n_nodes++;
                    if(node->key == key)
                        return;
                    node = key < node->key ? node->left : node->right;
                }while(node);
            }

            void decreace_count(Node* node, int key) {
                do{
                    node->n_nodes--;
                    if(node->key == key)
                        return;
                    node = key < node->key ? node->left : node->right;
                }while(node);
            }

            void balance_impl(Node* node) {
                if(!node) return;
                static auto safe_get_count = [](const Node* node) -> ssize_t {
                    return node ? node->n_nodes + 1 : 0;
                };

                auto left_side_count = safe_get_count(node->left);
                auto right_side_count = safe_get_count(node->right);
                if(left_side_count + 2 <= right_side_count) {
                    // pop median from right side
                    auto pnn = get_left_side(node->right);
                    pnn.first = !pnn.first ? node : pnn.first;
                    // update counts
                    decreace_count(node->right, pnn.second->key);

                    auto root_key = node->key;
                    node->key = pnn.second->key; // median at the root
                    helper_remove(pnn); // remove median from subtree

                    // insert old root into right-subtree
                    pnn = get_right_side(node->left);
                    pnn.first = !pnn.first ? node : pnn.first;
                    if(!pnn.second) {
                        pnn.first->left = new Node{root_key};
                    } else {
                        pnn.second->right = new Node{root_key};
                    }
                    // update counts
                    increase_count(node->left, root_key);
                }
                if(left_side_count >= right_side_count + 2) {
                    // pop median from left side
                    auto pnn = get_right_side(node->left);
                    pnn.first = !pnn.first ? node : pnn.first;
                    // update counts
                    decreace_count(node->left, pnn.second->key);

                    auto root_key = node->key;
                    node->key = pnn.second->key; // median at the root
                    helper_remove(pnn); // remove median from subtree

                    // insert old root into left-subtree
                    pnn = get_left_side(node->right);
                    pnn.first = !pnn.first ? node : pnn.first;
                    if(!pnn.second) {
                        pnn.first->right = new Node{root_key};
                    } else {
                        pnn.second->left = new Node{root_key};
                    }
                    // update counts
                    increase_count(node->right, root_key);
                }

                balance_impl(node->left);
                balance_impl(node->right);

                return;
            }
            */