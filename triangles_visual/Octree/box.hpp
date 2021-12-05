#pragma once
#include "vector.hpp"
#include <algorithm>

namespace Geomentry {

struct Box {
    Vec3 main;
    Vec3 center;
    Vec3 basis[3];

    Vec3 get_min() const {
        float coords[3] = {};
        for(int i = 0 ; i < 3; i++)
            coords[i] = std::abs(main * basis[i] * 0.5);

        double x_min = center[0];
        double y_min = center[1];
        double z_min = center[2];

        for (int i = 0; i < 8; i++) {
            bool x_flag = i & 0b001;
            bool y_flag = i & 0b010;
            bool z_flag = i & 0b100;

            float x_sign = 2.0f * x_flag - 1.0f;
            float y_sign = 2.0f * y_flag - 1.0f;
            float z_sign = 2.0f * z_flag - 1.0f;

            Vec3 delta =
                 basis[0] * x_sign * coords[0] +
                 basis[1] * y_sign * coords[1] +
                 basis[2] * z_sign * coords[2];

            x_min = std::min(x_min, (center + delta) * Vec3 { 1, 0, 0 });
            y_min = std::min(y_min, (center + delta) * Vec3 { 0, 1, 0 });
            z_min = std::min(z_min, (center + delta) * Vec3 { 0, 0, 1 });
        }

        return Vec3{ x_min, y_min, z_min };
    }

    Vec3 get_max() const {
        float coords[3] = {};
        for (int i = 0; i < 3; i++)
            coords[i] = std::abs(main * basis[i] * 0.5);

        double x_max = center[0];
        double y_max = center[1];
        double z_max = center[2];

        for (int i = 0; i < 8; i++) {
            bool x_flag = i & 0b001;
            bool y_flag = i & 0b010;
            bool z_flag = i & 0b100;

            float x_sign = 2.0f * x_flag - 1.0f;
            float y_sign = 2.0f * y_flag - 1.0f;
            float z_sign = 2.0f * z_flag - 1.0f;

            Vec3 delta =
                basis[0] * x_sign * coords[0] +
                basis[1] * y_sign * coords[1] +
                basis[2] * z_sign * coords[2];

            x_max = std::max(x_max, (center + delta) * Vec3 { 1, 0, 0 });
            y_max = std::max(y_max, (center + delta) * Vec3 { 0, 1, 0 });
            z_max = std::max(z_max, (center + delta) * Vec3 { 0, 0, 1 });
        }

        return Vec3{ x_max, y_max, z_max };
    }
};

bool is_intersected_impl(const Box &a, const Box &b) {

    auto delta = a.center - b.center;
    if(delta.length() < 1e-5)
        return true;

    auto omega_1 = a.main;
    auto omega_2 = b.main;

    // true if there is NO intersection
    auto f = [&](Vec3 D) {
        return 2.0 * std::abs(D * delta) > std::abs(D * omega_1) + std::abs(D * omega_2);
    };

    // if we found direction, where boxes are not intersection
    // so there is a plane, that divide space into two
    // non intersection part where located our boxes
    bool has_intersection = 1;


    for(int i = 0; i < 3; i++) {
        has_intersection &= !f(a.basis[i]);
        has_intersection &= !f(b.basis[i]);
    }

    return has_intersection;
}


}