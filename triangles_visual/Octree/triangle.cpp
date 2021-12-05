#include "triangle.hpp"
#include <cmath>

namespace Geomentry {
static bool is_intersected(const Segment &seg, const Triangle &tr) {
    auto n = tr.get_normal();
    auto v = seg[1] - seg[0];
    auto q = tr[0];
    auto p = seg[0];
    auto qp = p - q;
    auto alpha = -(qp * n) / (v * n);

    // vector qp --- from triangle vertex to segment vertex
    if (!(0 <= alpha && alpha <= 1))
        return false;

    // vector qp --- from triangle vertex to point into triangle's plane
    qp = qp + v * alpha;

    // barycentric based test for checking intersectiong between point and
    // triangle
    return tr.is_point_in_trianle(qp + tr[0]);
}

static inline bool is_intersected_proto(const Triangle &a, const Triangle &b) {
    Segment s0{a[0], a[1]};
    Segment s1{a[1], a[2]};
    Segment s2{a[2], a[0]};
    return is_intersected(s0, b) || is_intersected(s1, b) ||
           is_intersected(s2, b);
}

bool is_intersected_impl(const Triangle &a, const Triangle &b) {
    auto n = a.get_normal();
    auto cross_normals = n ^ b.get_normal();
    auto normal_length_sqr = cross_normals * cross_normals;
    if (normal_length_sqr < EPSILON) {
        auto dist = std::fabs(n * (a[0] - b[0]));
        if (dist < EPSILON) {
            bool answ = false;

            for (int i = 0; i < 3 && !answ; i++) {
                answ |= b.is_point_in_trianle(a[i]);
                answ |= a.is_point_in_trianle(b[i]);
            }

            for (int i = 0; i < 3 && !answ; i++)
                for (int j = 0; j < 3 && !answ; j++) {
#define at(idx_) ((idx_) % 3)
                    Segment s0{a[at(i)], a[at(i + 1)]};
                    Segment s1{b[at(j)], b[at(j + 1)]};
                    answ |= s0.intersected(s1);
#undef at
                }

            return answ;
        }
        return false;
    }
    return is_intersected_proto(a, b) || is_intersected_proto(b, a);
}

bool Triangle::intersected(const Triangle &rhs) const {
    const auto &lhs = *this;
    return is_intersected_impl(lhs, rhs);
}

std::ostream &operator<<(std::ostream &stream, const Vec3 &vec) {
    stream << "{" << vec[0] << ", " << vec[1] << ", " << vec[2] << "}";
    return stream;
}
} // namespace Geomentry


namespace glm {
    std::istream &operator>>(std::istream &stream, vec3 &vec) {
        for (auto d : { 0, 1, 2 })
            stream >> vec[d];
        return stream;
    }

    std::istream &operator>>(std::istream &stream, Triangle &tr) {
        for (auto v : { 0, 1, 2 })
            stream >> tr[v];
        return stream;
    }
};
