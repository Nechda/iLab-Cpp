#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <cstddef>
#include <iostream>
#include <type_traits>

namespace Linagl {

using Long_number = boost::multiprecision::cpp_int;
using Number_ext = boost::rational<Long_number>;

template <typename T>
struct Matrix;
Number_ext det_LUP(Matrix<Number_ext> mat);

template <typename T>
struct Matrix {
  public:
    using row_t = T *;
    template <typename U>
    using Mat_t = Matrix<U>;

    Matrix(size_t N, size_t M) : Height_(N), Width_(M) {
        if (N == 0 || M == 0)
            return;

        data_ = new T[N * M];
        rows_ = new T *[N];
        if (rows_ == nullptr || data_ == nullptr)
            return;

        for (size_t i = 0; i < N; i++)
            rows_[i] = &data_[i * M];
    }

    template <typename U>
    Matrix(const Mat_t<U> &rhs) : Height_(rhs.get_heigth()), Width_(rhs.get_width()) {
        data_ = new T[Height_ * Width_];
        rows_ = new T *[Height_];
        if (rows_ == nullptr || data_ == nullptr)
            return;

        for (size_t i = 0; i < Height_; i++)
            for (size_t j = 0; j < Width_; j++)
                data_[i * Width_ + j] = static_cast<T>(rhs[i][j]);

        for (size_t i = 0; i < Height_; i++)
            rows_[i] = &data_[i * Width_];
    }
    Matrix(const Mat_t<T> &rhs) : Height_(rhs.get_heigth()), Width_(rhs.get_width()) {
        data_ = new T[Height_ * Width_];
        rows_ = new T *[Height_];
        if (rows_ == nullptr || data_ == nullptr)
            return;

        std::copy(rhs.data_, rhs.data_ + Height_ * Width_, data_);

        for (size_t i = 0; i < Height_; i++)
            rows_[i] = &data_[i * Width_];
    }

    Matrix(Mat_t<T> &&rhs) : Height_(rhs.get_heigth()), Width_(rhs.get_width()) {
        data_ = rhs.data_;
        rows_ = rhs.rows_;

        rhs.data_ = nullptr;
        rhs.rows_ = nullptr;
    }

    Mat_t<T> &operator=(const Mat_t<T> &rhs) {
        if (this == &rhs)
            return *this;

        delete[] data_;
        delete[] rows_;

        Height_ = rhs.get_height();
        Width_ = rhs.get_width();

        data_ = new T[Height_ * Width_];
        rows_ = new T *[Height_];
        if (rows_ == nullptr || data_ == nullptr)
            return *this;

        std::copy(rhs.data_, rhs.data_ + Height_ * Width_, data_);
        for (size_t i = 0; i < Height_; i++)
            rows_[i] = &data_[i * Width_];

        return *this;
    }

    Mat_t<T> &operator=(Mat_t<T> &&rhs) {
        if (this == &rhs)
            return *this;

        Height_ = rhs.get_heigth();
        Width_ = rhs.get_width();
        data_ = rhs.data_;
        rows_ = rhs.rows_;

        rhs.data_ = nullptr;
        rhs.rows_ = nullptr;

        return *this;
    }

    ~Matrix() {
        delete[] rows_;
        delete[] data_;

        rows_ = nullptr;
        data_ = nullptr;
    }

    row_t &operator[](size_t idx) { return rows_[idx]; }
    const row_t &operator[](size_t idx) const { return rows_[idx]; }

    Number_ext det() const {
        if (Width_ != Height_)
            return Number_ext{};
        return det_LUP(Mat_t<Number_ext>{*this});
    }

    void swap_row(size_t i, size_t j) {
        if (i == j)
            return;
        std::swap(rows_[i], rows_[j]);
    }

    size_t get_width() const { return Width_; }
    size_t get_heigth() const { return Height_; }

  private:
    const size_t Height_;
    const size_t Width_;
    T *data_ = nullptr;
    row_t *rows_ = nullptr;
};

Number_ext det_LUP(Matrix<Number_ext> mat) {
    const size_t N = mat.get_heigth();

    auto &C = mat;
    size_t n_swaps = 0;

    for (size_t i = 0; i < N; i++) {
        Number_ext pivot_val = -1;
        size_t pivot = -1;
        for (size_t row = i; row < N; row++) {
            if (boost::abs(C[row][i]) > pivot_val) {
                pivot_val = boost::abs(C[row][i]);
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

    Number_ext res = n_swaps % 2 == 0 ? 1 : -1;
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
