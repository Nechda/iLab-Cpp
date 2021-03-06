#pragma once
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <math.h>

namespace Geomentry {
const double EPSILON = 1e-9;

struct Vec3 {
    double x = 0;
    double y = 0;
    double z = 0;

    void normalize() {
        double length = (*this) * (*this);
        length = sqrt(length);
        x /= length;
        y /= length;
        z /= length;
    }

    Vec3 operator*(double k) const {
        Vec3 result(*this);
        for (auto d : {0, 1, 2})
            result[d] *= k;
        return result;
    }

    double operator*(const Vec3 &lhs) {
        const auto &rhs = *this;
        return rhs.x * lhs.x + rhs.y * lhs.y + rhs.z * lhs.z;
    };

    // cross product
    Vec3 operator^(const Vec3 &b) const {
        const auto &a = *this;
        Vec3 result;
        result.x = a.y * b.z - a.z * b.y;
        result.y = a.z * b.x - a.x * b.z;
        result.z = a.x * b.y - a.y * b.x;
        return result;
    }

    Vec3 operator+(const Vec3 &rhs) const {
        Vec3 result(*this);
        for (auto d : {0, 1, 2})
            result[d] += rhs[d];
        return result;
    }

    Vec3 operator-(const Vec3 &rhs) const {
        Vec3 result(*this);
        for (auto d : {0, 1, 2})
            result[d] -= rhs[d];
        return result;
    }

    double &operator[](size_t i) {
        switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            return x;
        }
    }

    const double &operator[](size_t i) const {
        switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            return x;
        }
    }
};

struct Segment {
    Vec3 p;
    Vec3 q;

    bool intersected(const Segment &rhs) {
        const auto &lhs = *this;

        auto d = rhs[0] - lhs[0];
        auto v1 = rhs[1] - rhs[0];
        auto v2 = lhs[1] - lhs[0];

        auto n = v1 ^ v2;

        // check that lines are not the same
        auto tmp = n * n;
        if (tmp < EPSILON) {
            // they are parallel
            v1.normalize();
            d = d - v1 * (v1 * d);
            tmp = d * d;
            // but maybe not the same
            return tmp < EPSILON;
        }

        auto vol = n * n;

        v1 = d ^ v1;
        v2 = d ^ v2;

        auto alpha = v1 * n / vol;
        auto betta = v2 * n / vol;

        return 0 <= alpha && alpha <= 1 && 0 <= betta && betta <= 1;
    }

    const Vec3 &operator[](size_t i) const {
        switch (i) {
        case 0:
            return p;
        case 1:
            return q;
        default:
            return p;
        }
    }

    Vec3 &operator[](size_t i) {
        switch (i) {
        case 0:
            return p;
        case 1:
            return q;
        default:
            return p;
        }
    }
};

struct Triangle {
    Vec3 v[3] = {};

    Vec3 &operator[](size_t i) { return v[i % 3]; }

    const Vec3 &operator[](size_t i) const { return v[i % 3]; }

    Vec3 get_normal() const {
        Vec3 v_0 = v[1] - v[0];
        Vec3 v_1 = v[2] - v[0];
        Vec3 n = v_0 ^ v_1;
        n.normalize();
        return n;
    }

    bool intersected(const Triangle &lhs) const;

    bool is_point_in_trianle(const Vec3 &v) const {
        const auto &tr = *this;
        auto qp = v - tr[0];
        auto n = tr.get_normal();
        auto v1 = tr[1] - tr[0];
        auto v2 = tr[2] - tr[0];
        auto v1_orth = n ^ v1;
        auto v2_orth = n ^ v2;

        auto lambda1 = (qp * v2_orth) / (v1 * v2_orth);
        auto lambda2 = (qp * v1_orth) / (v2 * v1_orth);

        return lambda1 >= 0 && lambda2 >= 0 && (lambda1 + lambda2) <= 1;
    }
};

bool is_intersected_impl(const Triangle &a, const Triangle &b);

} // namespace Geomentry