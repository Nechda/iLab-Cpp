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

namespace Linagl {

using Long_number = boost::multiprecision::cpp_int;
using Long_real = boost::multiprecision::cpp_dec_float_50;
using Number_ext = boost::rational<Long_number>;

template <typename T>
struct Matrix;
template <typename Number_t>
Number_t det_LUP(Matrix<Number_t> mat);

template <typename T>
struct Matrix {
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

    Matrix(size_t N, size_t M) : Height_(N), Width_(M) {
        if (Height_ == 0 || Width_ == 0)
            throw std::runtime_error("One or both matrix size are equal zero");

        // operator new cat throw
        data_ = new T[Height_ * Width_];

        // safe allocate memory for row_permutation_ array
        try {
            row_perm_ = new size_t[Height_];
            std::iota(row_perm_, row_perm_ + Height_, 0);
        } catch (...) {
            delete[] data_;
            throw;
        }
    }

    template <typename U>
    Matrix(const Mat_t<U> &rhs) : Height_(rhs.get_heigth()), Width_(rhs.get_width()) {
        if (Height_ == 0 || Width_ == 0)
            throw std::runtime_error("One or both matrix size are equal zero");

        // operator new can throw
        data_ = new T[Height_ * Width_];
        try {
            // catch another exceptions
            for (size_t i = 0; i < Height_; i++)
                for (size_t j = 0; j < Width_; j++)
                    data_[i * Width_ + j] = static_cast<T>(rhs[i][j]);
        } catch (...) {
            // clean memory and rethrow
            delete[] data_;
            throw;
        }

        // safe allocate memory for row_permutation_ array
        try {
            row_perm_ = new size_t[Height_];
            std::iota(row_perm_, row_perm_ + Height_, 0);
        } catch (...) {
            delete[] data_;
            throw;
        }
    }
    Matrix(const Mat_t<T> &rhs) : Height_(rhs.Height_), Width_(rhs.Width_) {
        if (Height_ == 0 || Width_ == 0)
            throw std::runtime_error("One or both matrix size are equal zero");

        // operator new can throw
        data_ = new T[Height_ * Width_];
        try {
            // catch another exceptions
            for (size_t i = 0; i < Height_; i++)
                for (size_t j = 0; j < Width_; j++)
                    data_[i * Width_ + j] = rhs[i][j];
        } catch (...) {
            // clean memory and rethrow
            delete[] data_;
            throw;
        }

        // safe allocate memory for row_permutation_ array
        try {
            row_perm_ = new size_t[Height_];
            std::iota(row_perm_, row_perm_ + Height_, 0);
        } catch (...) {
            delete[] data_;
            throw;
        }
    }

    Matrix(Mat_t<T> &&rhs) : Height_(rhs.Height_), Width_(rhs.Width_), data_(rhs.data_), row_perm_(rhs.row_perm_) {
        rhs.data_ = nullptr;
        rhs.row_perm_ = nullptr;
    }

    Mat_t<T> &operator=(const Mat_t<T> &rhs) {
        if (this == &rhs)
            return *this;
        Mat_t<T> tmp(rhs);
        this->swap(tmp);
        return *this;
    }

    Mat_t<T> &operator=(Mat_t<T> &&rhs) {
        if (this == &rhs)
            return *this;
        this->swap(rhs);
        return *this;
    }

    ~Matrix() {
        delete[] data_;
        delete[] row_perm_;
    }

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

    void swap_row(size_t i, size_t j) {
        if (i == j)
            return;
        std::swap(row_perm_[i], row_perm_[j]);
    }

    size_t get_width() const { return Width_; }
    size_t get_heigth() const { return Height_; }

  private:
    size_t Height_;
    size_t Width_;
    T *data_ = nullptr;
    size_t *row_perm_ = nullptr;

    void swap(Mat_t<T> &rhs) noexcept {
        std::swap(rhs.Width_, Width_);
        std::swap(rhs.Height_, Height_);
        std::swap(rhs.data_, data_);
        std::swap(rhs.row_perm_, row_perm_);
    }
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
