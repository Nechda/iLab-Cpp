#include "Matrix.hpp"

void t_matrix() {
    size_t n_tests = 0, matrix_size = 0;
    std::cin >> n_tests >> matrix_size;

    Linagl::Matrix<int> mat{matrix_size, matrix_size};
    while (n_tests--) {
        ssize_t det = 0;
        std::cin >> mat;
        std::cin >> det;

        auto eval_det = mat.det();
        decltype(eval_det) correct_det{det};
        if (eval_det == correct_det) {
            std::cout << "Ok ";
            continue;
        }

        std::cout << "\n[Failed]\n" << mat;
        std::cout << "Evaluated det = " << eval_det.numerator() << std::endl;
        std::cout << "  Correct det = " << correct_det.numerator() << std::endl;
        break;
    }
}

void eval_det() {
    size_t matrix_size;
    std::cin >> matrix_size;

    Linagl::Matrix<int> mat{matrix_size, matrix_size};
    std::cin >> mat;
    std::cout << mat.det().numerator() << std::endl;
}

int main() {
    #ifndef TEST
        eval_det();
    #else
        t_matrix();
    #endif
    return 0;
}
