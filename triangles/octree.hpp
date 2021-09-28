#include <array>
#include <list>
#include <unordered_set>
#include <cassert>
#include <iostream>
#include <bitset>
#include "triangle.hpp"


namespace Algorithm
{
    struct OctMask {
        int mask_ = 0b11111111;

        void and_op(bool mask[2], int dim) {
            assert(0 <= dim && dim < 3);
            char dim_mask[] = {
                mask[1] * 0b11001100 + mask[0] * 0b00110011,
                mask[1] * 0b01100110 + mask[0] * 0b10011001,
                mask[1] * 0b11110000 + mask[0] * 0b00001111
            };
            mask_ &= dim_mask[dim];
        }

        bool get_bit(size_t idx) const {
            return (mask_ >> idx) & 1;
        }

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
        Geomentry::Vec3 max_;
        Geomentry::Vec3 min_;

        AABB(const Geomentry::Triangle& t) {
            for(auto i : {0,1,2}) {
                min_[i] = +std::numeric_limits<double>::infinity();
                max_[i] = -std::numeric_limits<double>::infinity();
                for(auto v : {0,1,2}) {
                    min_[i] = std::min(min_[i], t[v][i]);
                    max_[i] = std::max(max_[i], t[v][i]);
                }
            }
        }

        AABB(const Geomentry::Vec3& min, const Geomentry::Vec3& max) : 
            min_(min), max_(max) {}
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
        std::array<Node_t*, 8U> childs = {};
        std::list<size_t> triangle_idx;
    };

    struct Octree {
            Octree() = delete;
            Octree(Geomentry::Vec3 min, Geomentry::Vec3 max, std::vector<Geomentry::Triangle>& trs) :
                min_(min), max_(max), root(nullptr), triangles_(trs)
            {
                root = new Node_t;
            }
            
            void insert(const Geomentry::Triangle& t, size_t tr_idx = 0) {
                AABB aabb_triangle(t);
                AABB aabb_cell(min_, max_);

                iteration(aabb_cell, aabb_triangle, tr_idx, root, 0);
            }

            const Node_t* get_root() const {
                return root;
            }

            ~Octree() {
                clean_up(root);
            }

            void DFS() {
                DFS_impl(root);
            }

            void dump_node(Node_t* node) {
                if(!node) return;

                std::string tr_idx_str;
                auto idx_list = node->triangle_idx;
                for(auto it = idx_list.begin(); it != idx_list.end(); it++) {
                    tr_idx_str.append(std::to_string(*it));
                    if(std::next(it) == idx_list.end())
                        continue;
                    tr_idx_str.append("|");
                }

                std::string childs_str;
                for(int i = 0; i < 8; i++)
                    childs_str.append(
                        "<c" + std::to_string(i) + "> " + std::to_string(i) + " "
                    ).append(i == 7 ? "" : "|");
                printf("node_%p[shape = record, label = \" {{idx: | %s}|{%s}}\"]\n",
                    node, tr_idx_str.c_str(), childs_str.c_str());

                for(int i = 0; i < 8; i++) {
                    if(!node->childs[i])
                        continue;
                    
                    printf("node_%p:c%d -> node_%p\n", node, i, node->childs[i]);
                }

                for(int i = 0; i < 8; i++)
                    dump_node(node->childs[i]);
            }

            void dump() {
                printf(
                    "digraph G {\n"
                    "splines=polyline;\n"
                );
                dump_node(root);
                printf("}\n");
            }

        private:
            const size_t max_n_iter = 4;
            const Geomentry::Vec3 min_;
            const Geomentry::Vec3 max_;
            const std::vector<Geomentry::Triangle>& triangles_;
            Node_t* root;
            std::vector<size_t> current_sequence;
            std::unordered_set<size_t> intersected;
            void select_oct(AABB& aabb_cell, size_t octant) 
            {
                auto disp = (aabb_cell.max_ - aabb_cell.min_) * 0.5;
                auto new_max = aabb_cell.min_ + disp;
                aabb_cell.max_ = new_max;

                bool need_disp[3] = { 
                    ( (octant + 0) >> 1 ) & 1,
                    ( (octant + 1) >> 1 ) & 1,
                    octant > 3
                };

                Geomentry::Vec3 offset;
                for(auto d : {0,1,2}) {
                    aabb_cell.min_[d] += need_disp[d] * disp[d];
                    aabb_cell.max_[d] += need_disp[d] * disp[d];
                }
            }
            void iteration(AABB& aabb_cell, const AABB& aabb_triangle, size_t tr_idx, Node_t* node, size_t n_iter) {

                // Главный реузльтат этого участка кода: битовая
                // маска октантов, в которой находится треугольник
                auto center = (aabb_cell.max_ + aabb_cell.min_) * 0.5;

                OctMask octmask;
                for(auto d : {0,1,2}) {
                    auto min = aabb_triangle.min_[d];
                    auto max = aabb_triangle.max_[d];
                    
                    bool mask[2] = {};
                    bool min_in_high = center[d] < min;
                    bool max_in_high = center[d] < max;
                    mask[0] = !min_in_high || !max_in_high;
                    mask[1] =  min_in_high ||  max_in_high;

                    octmask.and_op(mask, d);
                }

                std::bitset<8> b(octmask.mask_);
                std::cout << b << std::endl;

                // Если нашелся октант, в который данный треугольник помещается полностью,
                // то тупо запускаем рекурсивно спуск дальше
                auto non_zero_bits = octmask.non_zero_bits();
                if(non_zero_bits == 1 && n_iter < max_n_iter) {
                    for(size_t i = 0; i < 8; i++)
                    if(octmask.get_bit(i)) {
                        select_oct(aabb_cell, i);
                        node->childs[i] = node->childs[i] ? node->childs[i] : new Node_t;
                        auto next_node = node->childs[i];
                        iteration(aabb_cell, aabb_triangle, tr_idx, next_node, n_iter + 1);
                        return;
                    }
                }

                if(non_zero_bits == 8) {
                    // все октанты заняты данным треугольником, т.е.
                    // просто добавляем его в список текущего нода
                    node->triangle_idx.push_front(tr_idx);
                    return;
                }

                // в противном случае мы не можем точно сказать в каком именно октанте
                // находится треугольник и мы сообщаем всем детям, что в них располагается
                // треугольник
                auto& childs = node->childs;
                for(int m = octmask.mask_, child_idx = 0; m; m >>= 1, child_idx++) {
                    if((m & 1) == 0) continue; 
                    childs[child_idx] =
                        childs[child_idx] ? childs[child_idx] : new Node_t;
                    auto child = childs[child_idx];
                    child->triangle_idx.push_front(tr_idx);
                }
            }
            void check_intersection() {
                for(size_t i = 0; i + 1 < current_sequence.size(); i++)
                for(size_t j = i + 1; j < current_sequence.size(); j++) {
                    bool is_intersected =
                        triangles_[i].intersected(triangles_[j]);
                    if(is_intersected) {
                        intersected.insert(i);
                        intersected.insert(j);
                    }
                }
            }
            void DFS_impl(Node_t* root) {
                if(root == nullptr) {
                    check_intersection();
                    return;
                }

                // добавляем новые треугольники
                size_t appended = 0;
                for(auto idx : root->triangle_idx) {
                    appended++;
                    current_sequence.push_back(idx);
                }

                // заходим в рекурсию
                for(auto& child : root->childs) {
                    DFS_impl(child);
                }

                // после обработки вершины удаляем все добавленные треугольники
                for(size_t i = 0; i < appended; i++)
                    current_sequence.pop_back();
            }
            void clean_up(Node_t* node) {
                if(node == nullptr)
                    return;

                for(auto& child : node->childs)
                    clean_up(child);
                delete node;
            }
    };
}