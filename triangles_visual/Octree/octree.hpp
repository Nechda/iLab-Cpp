#pragma once
#include "triangle.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <limits>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <vector>

namespace Algorithm {
struct OctMask {
    int mask_ = 0b11111111;

    void and_op(bool mask[2], int dim) {
        assert(0 <= dim && dim < 3);
        int dim_mask[] = {mask[1] * 0b11001100 + mask[0] * 0b00110011,
                          mask[1] * 0b01100110 + mask[0] * 0b10011001,
                          mask[1] * 0b11110000 + mask[0] * 0b00001111};
        mask_ &= dim_mask[dim];
    }

    bool get_bit(size_t idx) const { return (mask_ >> idx) & 1; }

    size_t non_zero_bits() const {
        // dark magic here
        size_t result = mask_;
        result = (result & 0b01010101) + ((result >> 1) & 0b01010101);
        result = (result & 0b00110011) + ((result >> 2) & 0b00110011);
        result = (result & 0b00001111) + ((result >> 4) & 0b00001111);
        return result;
    }
};

struct AABB {
    Geomentry::Vec3 min_;
    Geomentry::Vec3 max_;

    AABB(const Geomentry::Triangle &t) {
        for (auto i : {0, 1, 2}) {
            min_[i] = +std::numeric_limits<double>::infinity();
            max_[i] = -std::numeric_limits<double>::infinity();
            for (auto v : {0, 1, 2}) {
                min_[i] = std::min(min_[i], t[v][i]);
                max_[i] = std::max(max_[i], t[v][i]);
            }
        }
    }

    AABB(const Geomentry::Vec3 &min, const Geomentry::Vec3 &max)
        : min_(min), max_(max) {}
};

/*
    bot:
    +---+---+
    | 0 | 1 |
    +---+---+
    | 3 | 2 |
    +---+---+

    top:
    +---+---+
    | 4 | 5 |
    +---+---+
    | 7 | 6 |
    +---+---+
*/
struct Node_t {
    std::array<Node_t *, 8U> childs = {};
    std::list<size_t> triangle_idx;
    size_t n_trs = 0;
};

struct Octree {
    Octree() = delete;
    Octree(Geomentry::Vec3 min, Geomentry::Vec3 max,
           std::vector<Geomentry::Triangle> &trs)
        : min_(min), max_(max), triangles_(trs), root(nullptr) {
        root = new Node_t;
        was_intersected.resize(trs.size());
    }

    void insert(const Geomentry::Triangle &t, size_t tr_idx = 0) {
        AABB aabb_triangle(t);
        AABB aabb_cell(min_, max_);

        iteration(aabb_cell, aabb_triangle, tr_idx, root, 0);
    }

    const Node_t *get_root() const { return root; }

    ~Octree() { clean_up(root); }

    void DFS() { DFS_impl(root); }

    void dump_node(Node_t *node) const {
        if (!node)
            return;

        std::string tr_idx_str;
        auto idx_list = node->triangle_idx;
        for (auto it = idx_list.begin(); it != idx_list.end(); it++) {
            tr_idx_str.append(std::to_string(*it));
            if (std::next(it) == idx_list.end())
                continue;
            tr_idx_str.append("|");
        }

        std::string childs_str;
        for (int i = 0; i < 8; i++)
            childs_str
                .append("<c" + std::to_string(i) + "> " + std::to_string(i) +
                        " ")
                .append(i == 7 ? "" : "|");
        printf("node_%p[shape = record, label = \" {{idx: | %s}|{%s}}\"]\n",
               node, tr_idx_str.c_str(), childs_str.c_str());

        for (int i = 0; i < 8; i++) {
            if (!node->childs[i])
                continue;

            printf("node_%p:c%d -> node_%p\n", node, i, node->childs[i]);
        }

        for (int i = 0; i < 8; i++)
            dump_node(node->childs[i]);
    }

    void dump() {
        printf("digraph G {\n"
               "splines=polyline;\n");
        dump_node(root);
        printf("}\n");
    }

    const std::vector<bool> get_set() { return was_intersected; }

private:
    const size_t max_n_iter = 10;
    const size_t max_n_elem = 16;
    const Geomentry::Vec3 min_;
    const Geomentry::Vec3 max_;
    const std::vector<Geomentry::Triangle> &triangles_;
    std::vector<bool> was_intersected;
    Node_t *root;

    void select_oct(AABB &aabb_cell, size_t octant) {
        auto disp = (aabb_cell.max_ - aabb_cell.min_) * 0.5;
        auto new_max = aabb_cell.min_ + disp;
        aabb_cell.max_ = new_max;

        size_t need_disp[3] = {((octant + 0) >> 1) & 1, ((octant + 1) >> 1) & 1,
                               octant > 3};

        Geomentry::Vec3 offset;
        for (auto d : {0, 1, 2}) {
            aabb_cell.min_[d] += need_disp[d] * disp[d];
            aabb_cell.max_[d] += need_disp[d] * disp[d];
        }
    }

    void insert_imlp(AABB &aabb_cell, const AABB &aabb_triangle, size_t tr_idx,
                   Node_t *node, size_t n_iter) {
        // The main result of lines below is bitmask of
        // octants, that consist current triangle.
        auto center = (aabb_cell.max_ + aabb_cell.min_) * 0.5;

        OctMask octmask;
        for (auto d : {0, 1, 2}) {
            auto min = aabb_triangle.min_[d];
            auto max = aabb_triangle.max_[d];

            bool mask[2] = {};
            bool min_in_high = center[d] < min;
            bool max_in_high = center[d] < max;
            mask[0] = !min_in_high || !max_in_high;
            mask[1] = min_in_high || max_in_high;

            octmask.and_op(mask, d);
        }

        auto non_zero_bits = octmask.non_zero_bits();

        if(non_zero_bits == 0) return;

        auto aabb_save = aabb_cell;
        if (n_iter < max_n_iter) {
            for (size_t i = 0; i < 8; i++) {
                if (octmask.get_bit(i)) {
                    aabb_cell = aabb_save;
                    select_oct(aabb_cell, i);
                    node->childs[i] =
                        node->childs[i] ? node->childs[i] : new Node_t;
                    auto next_node = node->childs[i];
                    iteration(aabb_cell, aabb_triangle, tr_idx, next_node,
                              n_iter + 1);
                }
            }
        }

        node->triangle_idx.push_front(tr_idx);
    }

    void iteration(AABB &aabb_cell, const AABB &aabb_triangle, size_t tr_idx,
                   Node_t *node, size_t n_iter) {

        node->n_trs++;
        if (node->n_trs < max_n_elem || n_iter >= max_n_iter) {
            node->triangle_idx.push_front(tr_idx);
            return;
        }

        if(node->n_trs == max_n_elem) {
            AABB save_cell = aabb_cell;
            for(auto idx : node->triangle_idx) {
                AABB aabb_triangle_tmp = AABB(triangles_[idx]);
                save_cell = aabb_cell;
                insert_imlp(save_cell, aabb_triangle_tmp, idx, node, n_iter);
            }
            save_cell = aabb_cell;
            insert_imlp(aabb_cell, aabb_triangle, tr_idx, node, n_iter);
            node->triangle_idx.clear();
            return;
        }

        // recursive insertion
        insert_imlp(aabb_cell, aabb_triangle, tr_idx, node, n_iter);
    }

    void check_intersection(Node_t *root) {
        static std::vector<size_t> current_sequence(max_n_elem);
        current_sequence.resize(0);
        for(auto it : root->triangle_idx)
            current_sequence.push_back(it);

        for (size_t i = 0; i + 1 < current_sequence.size(); i++) {
            for (size_t j = i + 1; j < current_sequence.size(); j++) {
                auto tr_idx_0 = current_sequence[i];
                auto tr_idx_1 = current_sequence[j];

                if(was_intersected[tr_idx_0] && was_intersected[tr_idx_1])
                    continue;

                bool is_intersected = triangles_[tr_idx_0].intersected(triangles_[tr_idx_1]);
                if (is_intersected) {
                    was_intersected[tr_idx_0] = 1;
                    was_intersected[tr_idx_1] = 1;
                }
            }
        }
    }

    void DFS_impl(Node_t *root) {
        if (root == nullptr)
            return;

        // go into new recursion interation
        size_t null_childs = 0;
        for (auto &child : root->childs) {
            DFS_impl(child);
            null_childs += child == nullptr;
        }

        if (null_childs == 8)
            check_intersection(root);
    }

    void clean_up(Node_t *node) {
        if (node == nullptr)
            return;

        for (auto &child : node->childs)
            clean_up(child);
        delete node;
    }
};
} // namespace Algorithm
