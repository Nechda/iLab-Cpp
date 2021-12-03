#pragma once

namespace mm {

template <typename T>
void construct(T *p, const T &rhs) {
    new (p) T(rhs);
}

template <typename T>
void destroy(T *p) {
    p->~T();
}

template <typename It_t>
void destroy(It_t first, It_t last) noexcept {
    while (first != last) {
        destroy(&*first);
        first++;
    }
}

} // namespace mm