#pragma once
#include <iostream>
#include <cstddef>
#include <array>
#include <type_traits>

namespace Linagl {

    template<size_t N__, size_t M__, typename T>
    struct Mat;

    template<size_t N>
    double det_LUP(Mat<N, N, double> mat) {
        auto& C = mat;
        size_t n_swaps = 0;

        for(ssize_t i = 0; i < N; i++) {
            double pivot_val = -1;
            ssize_t pivot = -1;
            for(ssize_t row = i; row < N; row++) {
                if(std::abs(C[row][i]) > pivot_val) {
                    pivot_val = std::abs(C[row][i]);
                    pivot = row;
                }
            }
            if(pivot_val < 1e-10)
                continue;

            C.swap_row(pivot, i);
            n_swaps += pivot != i;

            for(ssize_t j = i + 1; j < N; j++) {
                C[j][i] /= C[i][i];
                for(ssize_t k = i + 1; k < N; k++)
                    C[j][k] -= C[j][i] * C[i][k];
            
            }
        }

        double res = n_swaps % 2 == 0 ? 1.0 : -1.0;
        for(size_t i = 0; i < N; i++)
            res *= C[i][i];
        return res;
    }

    // impl matrix N x M
    template<size_t N__, size_t M__, typename T>
    struct Mat {
        public:
            using row_t = std::array<T, M__>;
            template<typename U>
            using Mat_t = Mat<N__, M__, U>;

            Mat() {
                for(auto& row : data_)
                    row = {};
            }
            template<typename U>
            Mat(const Mat_t<U>& rhs) {
                for(size_t i = 0; i < N__; i++)
                for(size_t j = 0; j < M__; j++)
                    data_[i][j] = static_cast<T>(rhs[i][j]);
            }
            Mat(const Mat_t<T>& rhs) {
                for(size_t i = 0; i < N__; i++)
                    data_[i] = rhs.data_[i];
            }
            Mat(Mat_t<T>&& rhs) {
                for(size_t i = 0; i < N__; i++)
                    data_[i] = std::move(rhs.data_[i]);
            }
            Mat_t<T>& operator=(const Mat_t<T>& rhs) {
                if(this == &rhs)
                    return *this;
                for(size_t i = 0; i < N__; i++)
                    data_[i] = rhs.data_[i];
                return *this;
            }
            Mat_t<T>& operator=(Mat_t<T>&& rhs) {
                if(this == &rhs)
                    return *this;
                for(size_t i = 0; i < N__; i++)
                    data_[i] = std::move(rhs.data_[i]);
                return *this;
            }
            ~Mat() = default;
            row_t& operator[](size_t idx) {
                return data_[idx];
            }
            const row_t& operator[](size_t idx) const {
                return data_[idx];
            }

            std::enable_if_t<
                N__ == M__ && std::is_arithmetic<T>::value,
                double
            > det() const {
                return det_LUP(Mat_t<double>(*this));
            }

            void swap_row(size_t i, size_t j) {
                if(i == j) return;
                std::swap(data_[i], data_[j]);
            }
        private:
            row_t data_[N__];

        template<size_t N_, size_t M_, typename T_>
        friend std::ostream& operator << (std::ostream& stream, const Mat<N_, M_, T_>& mat);
        template<size_t N_, size_t M_, typename T_>
        friend std::istream& operator >> (std::istream& stream, Mat<N_, M_, T_>& mat);
    };

    template<size_t N_, size_t M_, typename T_>
    std::ostream& operator << (std::ostream& stream, const Mat<N_, M_, T_>& mat) {
        for(const auto& row : mat.data_) {
            for(const auto& it : row)
                stream << it << " ";
            stream << std::endl;
        }
        return stream;
    }

    template<size_t N_, size_t M_, typename T_>
    std::istream& operator >> (std::istream& stream, Mat<N_, M_, T_>& mat) {
        for(auto& row : mat.data_) {
            for(auto& it : row)
                stream >> it;
        }
        return stream;
    }

} // namesapce Linagl
