#pragma once
#include "vector.hpp"
#include "box.hpp"
#include <glm/glm.hpp>
#include <algorithm>

namespace Geomentry {

struct Cylinder;
bool is_intersected_impl(const Cylinder &a, const Cylinder &b);

struct Cylinder {
    Vec3 c;
    Vec3 w;
    double r;
    double h;
    Vec3 u; // perp vector to the main axis, so select basis

    Cylinder() {}

    #if 0
    explicit Cylinder(const Triangle& tr) {
        auto to_point = tr[2] - tr[0];
        auto axis = tr[1] - tr[0];
        axis.normalize();

        w = axis;
        r = Vec3{to_point - axis * (to_point * axis)}.length();
        auto coord_2 = to_point * axis;
        auto coord_1 = (tr[2] - tr[0]) * axis;
        auto coord_0 = 0.0;

        auto min_ = std::min(std::min(coord_0, coord_1), coord_2);
        auto max_ = std::max(std::max(coord_0, coord_1), coord_2);
        h = max_ - min_;
        c = tr[0] + axis * (min_ + max_) * 0.5;
    }
    #endif

    Cylinder(const glm::Triangle& tr, std::pair<glm::vec3, glm::vec3> p_axis) {
        // get representation in another basis
        auto get_repr = [p_axis](glm::vec3 p) {
            auto to_point = p - p_axis.first;
            auto axis = p_axis.second - p_axis.first;
            axis /= glm::length(axis);
            auto dot_ = glm::dot(to_point, axis);
            auto perp = to_point - axis * dot_;
            return std::pair<float, float>{glm::length(perp), dot_};
        };

        auto rep_0 = get_repr(tr[0]);
        auto rep_1 = get_repr(tr[1]);
        auto rep_2 = get_repr(tr[2]);

        auto coord_2 = rep_2.second;
        auto coord_1 = rep_1.second;
        auto coord_0 = rep_0.second;

        auto axis = p_axis.second - p_axis.first;
        axis /= glm::length(axis);
        w.init_from(axis);

        auto min_ = std::min(std::min(coord_0, coord_1), coord_2);
        auto max_ = std::max(std::max(coord_0, coord_1), coord_2);
        h = std::max(max_ - min_, 0.01f);
        c.init_from(p_axis.first + axis * (min_ + max_) * 0.5f);
        r = std::max(std::max(rep_0.first, rep_1.first), rep_2.first);

        u = Vec3{1.0f,0.0f,0.0f};
        if(Vec3{u ^ w}.length() < 1e-5) {
            u = Vec3{0,1,0} ^ w;
            u.normalize();
        } else {
            u = u ^ w;
            u.normalize();
        }
    }

    bool intersected(const Cylinder &rhs) const {
        const auto &lhs = *this;
        return is_intersected_impl(lhs, rhs);
    }

    Box get_box() const {
        auto v = u ^ w;
        v.normalize();
        auto main = (v * r + u * r) * 2.0 + w * h;
        return Box{main, c, {w, u, v}};
    }
};

bool is_intersected_impl(const Cylinder &a, const Cylinder &b) {

    auto delta = a.c - b.c;
    if(delta.length() < 1e-5)
        return true;

    auto k = a.w ^ b.w;
    if(k.length() < 1e-5) {
        auto dot_ = delta * a.w;
        auto paral = a.w * dot_;
        auto perp = delta - a.w * dot_;
        perp.normalize();
        k = perp;
    }

    auto third_1 = k ^ a.w;
    third_1.normalize();
    auto omega_1 = (third_1 * a.r + k * a.r) * 2.0 + a.w * a.h;

    auto third_2 = k ^ b.w;
    third_2.normalize();
    auto omega_2 = (third_2 * b.r + k * b.r) * 2.0 + b.w * b.h;

    Box b_0{omega_1, a.c, {a.w, k, third_1}};
    Box b_1{omega_2, b.c, {b.w, k, third_2}};

    return is_intersected_impl(b_0, b_1);

    #if 0
    // here is approximation cylinders by boxes

    auto delta = a.c - b.c;
    if(delta.length() < 1e-5)
        return true;

    auto k = a.w ^ b.w;
    if(k.length() < 1e-5) {
        auto dot_ = delta * a.w;
        auto paral = a.w * dot_;
        auto perp = delta - a.w * dot_;
        perp.normalize();
        k = perp;
    }

    auto third_1 = k ^ a.w;
    third_1.normalize();
    auto omega_1 = (third_1 * a.r + k * a.r) * 2.0 + a.w * a.h;

    auto third_2 = k ^ b.w;
    third_2.normalize();
    auto omega_2 = (third_2 * b.r + k * b.r) * 2.0 + b.w * b.h;

    // true if there is NO intersection
    auto f = [&](Vec3 D) {
        return 0.5 * std::abs(D * delta) > std::abs(D * omega_1) + std::abs(D * omega_2);
    };

    // if we found direction, where boxes are not intersection
    // so there is a plane, that divide space into two
    // non intersection part where located our boxes
    bool has_intersection = 1;

    has_intersection &= !f(a.w);
    has_intersection &= !f(b.w);
    has_intersection &= !f(k);
    has_intersection &= !f(third_1);
    has_intersection &= !f(third_2);

    return has_intersection;
    #endif
}
} 