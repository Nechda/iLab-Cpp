#pragma once
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <cstddef>
#include <exception>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <type_traits>

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

namespace Linagl {

using Long_number = boost::multiprecision::cpp_int;
using Long_real = boost::multiprecision::cpp_dec_float_50;
using Number_ext = boost::rational<Long_number>;

template <typename T>
struct array_container {
    typedef array_container<T> self_type;

  protected:
    array_container(const self_type &) = delete;
    array_container &operator=(const self_type &) = delete;

    array_container(self_type &&rhs) noexcept : data_(rhs.data_), size_(rhs.size_), used_(rhs.used_) {
        rhs.data_ = nullptr;
        rhs.size_ = 0;
        rhs.used_ = 0;
    }
    array_container &operator=(self_type &&rhs) noexcept {
        std::swap(data_, rhs.data_);
        std::swap(size_, rhs.size_);
        std::swap(used_, rhs.used_);
        return *this;
    }

    array_container(size_t size = 0)
        : data_(size == 0 ? nullptr : static_cast<T *>(::operator new(sizeof(T) * size))), size_(size) {}

    ~array_container() {
        destroy(data_, data_ + used_);
        ::operator delete(data_);
    }

    T *data_ = nullptr;
    size_t size_ = 0;
    size_t used_ = 0;
};

template <typename T>
struct Matrix;
template <typename Number_t>
Number_t det_LUP(Matrix<Number_t> mat);

template <typename T>
struct Matrix : private array_container<T> {
    using array_container<T>::data_;
    using array_container<T>::size_;
    using array_container<T>::used_;

  public:
    struct row_t {
      public:
        row_t(T *ptr) : ptr_{ptr} {};
        T &operator[](size_t idx) { return ptr_[idx]; }
        const T &operator[](size_t idx) const { return ptr_[idx]; }

      private:
        T *ptr_;
    };
    template <typename U>
    using Mat_t = Matrix<U>;

    Matrix(size_t N, size_t M) : array_container<T>(N * M), Height_(N), Width_(M), row_perm_(N) {
        for (size_t i = 0; i < Height_; i++)
            for (size_t j = 0; j < Width_; j++) {
                auto place = data_ + Width_ * i + j;
                construct(place, T{});
                used_++;
            }
        std::iota(row_perm_.begin(), row_perm_.end(), 0);
    }

    template <typename U>
    Matrix(const Mat_t<U> &rhs)
        : array_container<T>(rhs.get_heigth() * rhs.get_width()), Height_(rhs.get_heigth()), Width_(rhs.get_width()),
          row_perm_(rhs.get_heigth()) {
        for (size_t i = 0; i < Height_; i++)
            for (size_t j = 0; j < Width_; j++) {
                auto place = data_ + Width_ * i + j;
                auto tmp = static_cast<T>(rhs[i][j]);
                construct(place, tmp);
                used_++;
            }
        std::iota(row_perm_.begin(), row_perm_.end(), 0);
    }
    Matrix(const Mat_t<T> &rhs)
        : array_container<T>(rhs.get_heigth() * rhs.get_width()), Height_(rhs.Height_), Width_(rhs.Width_),
          row_perm_(rhs.Height_) {
        for (size_t i = 0; i < Height_; i++)
            for (size_t j = 0; j < Width_; j++) {
                auto place = data_ + Width_ * i + j;
                construct(place, rhs[i][j]);
                used_++;
            }
        std::iota(row_perm_.begin(), row_perm_.end(), 0);
    }
    Mat_t<T> &operator=(const Mat_t<T> &rhs) {
        Mat_t<T> tmp(rhs);
        std::swap(*this, tmp);
        return *this;
    }

    Matrix(Mat_t<T> &&rhs) = default;
    Mat_t<T> &operator=(Mat_t<T> &&rhs) = default;

    row_t operator[](size_t idx) { return row_t{data_ + Width_ * row_perm_[idx]}; }
    const row_t operator[](size_t idx) const { return row_t{data_ + Width_ * row_perm_[idx]}; }

    Long_number det_integer() const {
        if (Width_ != Height_)
            return Long_number{};
        try {
            auto res = det_LUP(Mat_t<Number_ext>{*this});
            return res.numerator();
        } catch (std::exception &e) {
            std::throw_with_nested(e);
        }
    }

    Long_real det_real() const {
        if (Width_ != Height_)
            return Long_real{};
        try {
            auto res = det_LUP(Mat_t<Long_real>{*this});
            return res;
        } catch (std::exception &e) {
            std::throw_with_nested(e);
        }
    }

    void swap_row(size_t i, size_t j) noexcept {
        if (i == j)
            return;
        std::swap(row_perm_[i], row_perm_[j]);
    }

    size_t get_width() const { return Width_; }
    size_t get_heigth() const { return Height_; }

  private:
    size_t Height_;
    size_t Width_;
    std::vector<size_t> row_perm_;
};

template <typename Number_t>
Number_t det_LUP(Matrix<Number_t> mat) {
    using boost::abs;
    using boost::multiprecision::abs;
    const size_t N = mat.get_heigth();

    auto &C = mat;
    size_t n_swaps = 0;

    for (size_t i = 0; i < N; i++) {
        Number_t pivot_val = -1;
        size_t pivot = -1;
        for (size_t row = i; row < N; row++) {
            if (abs(C[row][i]) > pivot_val) {
                pivot_val = abs(C[row][i]);
                pivot = row;
            }
        }
        if (!pivot_val)
            continue;

        C.swap_row(pivot, i);
        n_swaps += pivot != i;

        for (size_t j = i + 1; j < N; j++) {
            C[j][i] /= C[i][i];
            for (size_t k = i + 1; k < N; k++)
                C[j][k] -= C[j][i] * C[i][k];
        }
    }

    Number_t res = n_swaps % 2 == 0 ? 1 : -1;
    for (size_t i = 0; i < N; i++)
        res *= C[i][i];
    return res;
}

template <typename T>
std::ostream &operator<<(std::ostream &stream, const Matrix<T> &mat) {
    for (size_t i = 0; i < mat.get_heigth(); i++) {
        for (size_t j = 0; j < mat.get_width(); j++)
            stream << mat[i][j] << " ";
        stream << std::endl;
    }
    return stream;
}

template <typename T>
std::istream &operator>>(std::istream &stream, Matrix<T> &mat) {
    for (size_t i = 0; i < mat.get_heigth(); i++)
        for (size_t j = 0; j < mat.get_width(); j++)
            stream >> mat[i][j];
    return stream;
}

} // namespace Linagl
