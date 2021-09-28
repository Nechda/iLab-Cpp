#pragma once
#include <cstddef>

namespace Geomentry
{
    struct Vec3 {
        double x = 0;
        double y = 0;
        double z = 0;

        Vec3 operator*(double k) {
            Vec3 result(*this);
            for(auto d : {0,1,2})
                result[d] *= k;
            return result;
        }

        Vec3 operator+(const Vec3& rhs) {
            Vec3 result(*this);
            for(auto d : {0,1,2})
                result[d] += rhs[d];
            return result;
        }

        Vec3 operator-(const Vec3& rhs) {
            Vec3 result(*this);
            for(auto d : {0,1,2})
                result[d] -= rhs[d];
            return result;
        }

        double& operator[](size_t i) {
            switch (i) {
                case 0 : return x;
                case 1 : return y;
                case 2 : return z;
                default: return x;
            }
        }

        const double& operator[](size_t i) const {
            switch (i) {
                case 0 : return x;
                case 1 : return y;
                case 2 : return z;
                default: return x;
            }
        }
    };

    struct Triangle {

        bool intersected(const Triangle& lhs) const {
            const auto& rhs = *this;

            // какой-то код проверки столкновения пересечения треугольников
            return false;
        }

        Vec3& operator[](size_t i) {
            return v[i % 3];
        }
        const Vec3& operator[](size_t i) const {
            return v[i % 3];
        }
        Vec3 v[3] = {};
    };

    bool is_intersected(const Triangle& a, const Triangle& b) {
        return false;
    }
}