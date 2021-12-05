#pragma once
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <math.h>
#include <glm/glm.hpp>


namespace Geomentry {
const double EPSILON = 1e-9;

struct Vec3 {
    double x = 0;
    double y = 0;
    double z = 0;

    void init_from(const glm::vec3& rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
    }

    void normalize() {
        double length = (*this) * (*this);
        length = sqrt(length);
        x /= length;
        y /= length;
        z /= length;
    }

    double length() const {
        double res = x * x + y * y + z * z;
        return std::sqrt(res);
    }

    Vec3 operator*(double k) const {
        Vec3 result(*this);
        for (auto d : {0, 1, 2})
            result[d] *= k;
        return result;
    }

    double operator*(const Vec3 &lhs) const {
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
}