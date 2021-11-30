#pragma once
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <type_traits>

namespace Linagl {

using Long_number = boost::multiprecision::cpp_int;
using Long_real = boost::multiprecision::cpp_dec_float_50;
using Number_ext = boost::rational<Long_number>;

template <typename T>
struct Matrix;
template<typename Number_t>
Number_t det_LUP(Matrix<Number_t> mat);

template <typename T>
struct Matrix {
  public:
    using row_t = T *;
    template <typename U>
    using Mat_t = Matrix<U>;

    Matrix(size_t N, size_t M) : Height_(N), Width_(M) {
        auto do_noting = []() {};
        memory_init(do_noting);
    }

    template <typename U>
    Matrix(const Mat_t<U> &rhs) : Height_(rhs.get_heigth()), Width_(rhs.get_width()) {
        auto copy_with_cast = [&]() {
            for (size_t i = 0; i < Height_; i++)
                for (size_t j = 0; j < Width_; j++)
                    data_[i * Width_ + j] = static_cast<T>(rhs[i][j]);
        };
        memory_init(copy_with_cast);
    }
    Matrix(const Mat_t<T> &rhs) : Height_(rhs.get_heigth()), Width_(rhs.get_width()) {
        auto raw_copy = [&]() { std::copy(rhs.data_, rhs.data_ + Height_ * Width_, data_); };
        memory_init(raw_copy);
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

        auto raw_copy = [&]() { std::copy(rhs.data_, rhs.data_ + Height_ * Width_, data_); };
        memory_init(raw_copy);

        return *this;
    }

    Mat_t<T> &operator=(Mat_t<T> &&rhs) {
        if (this == &rhs)
            return *this;

        std::swap(Height_, rhs.Height_);
        std::swap(Width_, rhs.Width_);
        std::swap(data_, rhs.data_);
        std::swap(rows_, rhs.rows_);

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

    void swap_row(size_t i, size_t j) {
        if (i == j)
            return;
        std::swap(rows_[i], rows_[j]);
    }

    size_t get_width() const { return Width_; }
    size_t get_heigth() const { return Height_; }

  private:
    size_t Height_;
    size_t Width_;
    T *data_ = nullptr;
    row_t *rows_ = nullptr;

    template <typename F>
    void memory_init(F copy_stage) {
        if (Height_ == 0 || Width_ == 0)
            throw std::runtime_error("One or both matrix size are equal zero");

        data_ = new T[Height_ * Width_];
        rows_ = new T *[Height_];
        if (rows_ == nullptr || data_ == nullptr)
            throw std::runtime_error("operator new return nullptr");

        try {
            copy_stage();
        } catch (std::exception &e) {
            std::throw_with_nested(e);
        }

        for (size_t i = 0; i < Height_; i++)
            rows_[i] = &data_[i * Width_];
    }
};

template<typename Number_t>
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
