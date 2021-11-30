#include "Matrix.hpp"
#include <cassert>
#include <array>

void t_matrix() {
    size_t n_tests = 0, matrix_size = 0;
    std::cin >> n_tests >> matrix_size;

    Linagl::Matrix<int> mat{matrix_size, matrix_size};
    while (n_tests--) {
        ssize_t det = 0;
        std::cin >> mat;
        std::cin >> det;

        auto eval_det = mat.det_integer();
        decltype(eval_det) correct_det{det};
        if (eval_det == correct_det) {
            std::cout << "Ok ";
            continue;
        }

        std::cout << "\n[Failed]\n" << mat;
        std::cout << "Evaluated det = " << eval_det << std::endl;
        std::cout << "  Correct det = " << correct_det << std::endl;
        break;
    }
}

template <typename F>
int try_wrapper(F action) {
    try {
        action();
        std::cout << "-- NO EXCEPTIONS --" << std::endl;
        return 0;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}

void t_exceptions() {
    size_t n_tests_with_ex = 0;
    std::cout << "[ Exception tests ]" << std::endl;

    /*
        1. Invalid matrix size-tests
    */
    n_tests_with_ex += try_wrapper([]() { Linagl::Matrix<int> mat(0, 0); });
    n_tests_with_ex += try_wrapper([]() { Linagl::Matrix<int> mat(0, 1); });
    n_tests_with_ex += try_wrapper([]() { Linagl::Matrix<int> mat(1, 0); });

    /*
        2. Triggering all types of ctors
    */
    n_tests_with_ex += try_wrapper([]() {
        Linagl::Matrix<int> mat(2, 2);

        int diff = 0;
        for (size_t y = 0; y < mat.get_heigth(); y++)
            for (size_t x = 0; x < mat.get_width(); x++)
                diff |= mat[y][x];

        assert(!diff);
    });
    n_tests_with_ex += try_wrapper([]() {
        Linagl::Matrix<int> mat(2, 2);
        for (size_t y = 0; y < mat.get_heigth(); y++)
            for (size_t x = 0; x < mat.get_width(); x++)
                mat[y][x] = 3 * y + x + 1;

        Linagl::Matrix<int> other(mat);

        int diff = 0;
        for (size_t y = 0; y < mat.get_heigth(); y++)
            for (size_t x = 0; x < mat.get_width(); x++)
                diff |= mat[y][x] ^ other[y][x];

        assert(!diff);
    });
    n_tests_with_ex += try_wrapper([]() {
        Linagl::Matrix<float> mat(2, 2);
        for (size_t y = 0; y < mat.get_heigth(); y++)
            for (size_t x = 0; x < mat.get_width(); x++)
                mat[y][x] = 3 * y + x + 1;

        Linagl::Matrix<int> other(mat);

        int diff = 0;
        for (size_t y = 0; y < mat.get_heigth(); y++)
            for (size_t x = 0; x < mat.get_width(); x++)
                diff |= (int)mat[y][x] ^ other[y][x];

        assert(!diff);
    });
    n_tests_with_ex += try_wrapper([]() {
        auto get_matrix = []() {
            Linagl::Matrix<int> mat(2, 2);
            for (size_t y = 0; y < mat.get_heigth(); y++)
                for (size_t x = 0; x < mat.get_width(); x++)
                    mat[y][x] = 3 * y + x + 1;
            return mat;
        };

        Linagl::Matrix<int> other(get_matrix());
        Linagl::Matrix<int> mat = other;

        int diff = 0;
        for (size_t y = 0; y < mat.get_heigth(); y++)
            for (size_t x = 0; x < mat.get_width(); x++)
                diff |= mat[y][x] ^ other[y][x];

        assert(!diff);
    });

    /*
        3. Trigger assign operators
    */
    n_tests_with_ex += try_wrapper([]() {
        Linagl::Matrix<int> long_lived(2, 2);
        Linagl::Matrix<float> mat(2, 2);
        mat[0][0] = 3.0;
        mat[1][1] = 3.0;
        long_lived = mat;
        assert(long_lived[0][0] == 3 && long_lived[1][1] == 3);
    });
    n_tests_with_ex += try_wrapper([]() {
        Linagl::Matrix<int> long_lived(2, 2);
        Linagl::Matrix<int> mat(2, 2);
        long_lived[0][0] = 1;
        long_lived[1][1] = 1;

        mat[0][0] = 3;
        mat[1][1] = 3;
        long_lived = std::move(mat);
        assert(long_lived[0][0] == 3 && long_lived[1][1] == 3);
        assert(mat[0][0] == 1 && mat[1][1] == 1);
    });
    std::cout << "[ End exception testing ]" << std::endl;
}

void eval_det_int() {
    size_t matrix_size;
    std::cin >> matrix_size;

    Linagl::Matrix<int> mat{matrix_size, matrix_size};
    std::cin >> mat;
    std::cout << mat.det_integer() << std::endl;
}

void eval_det_real() {
    size_t matrix_size;
    std::cin >> matrix_size;

    Linagl::Matrix<double> mat{matrix_size, matrix_size};
    std::cin >> mat;

    auto det = mat.det_real();
    auto ten_pow = Linagl::Long_real(10);
    for(int i = 0; i < 4; i++)
        ten_pow *= ten_pow;

    det = boost::multiprecision::round(det * ten_pow) / ten_pow;
    std::cout << det.str() << std::endl;
}

int main() {
    try {
#ifndef TEST
        eval_det_real();
#else
        t_exceptions();
        t_matrix();
#endif
    } catch (const std::exception &e) {
        std::cout << "Proga wants to sleep ... reason:" << std::endl;
        std::cout << e.what() << std::endl;
    }
    return 0;
}
