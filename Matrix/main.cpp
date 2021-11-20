#include "Matrix.hpp"
#include <string>
#include <cmath>



void t_matrix() {
    Linagl::Mat<10, 10, long long> mat;

    size_t n_tests = 0;
    std::cin >> n_tests;

    while(n_tests--) {
        long long det = 0;
        std::cin >> mat;
        std::cin >> det;

        auto eval_det = mat.det();
        decltype(eval_det) correct_det{det};
        if(eval_det == correct_det) {
            std::cout << "Ok ";
        } else {
            std::cout << "\n[Failed]\n" << mat;
            std::cout << "Evaluated det = " << eval_det << std::endl;
            std::cout << "  Correct det = " << correct_det << std::endl;
            break;
        }
    }
}


int main() {
    t_matrix();
    return 0;
}
