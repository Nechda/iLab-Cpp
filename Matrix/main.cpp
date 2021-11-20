#include "Matrix.hpp"
#include <string>
#include <cmath>
#include <boost/rational.hpp>



void t_matrix() {
    Linagl::Mat<10,10,double> mat;

    size_t n_tests = 0;
    std::cin >> n_tests;

    while(n_tests--){
        int det = 0;
        std::cin >> mat;
        std::cin >> det;

        auto answ = std::round(mat.det());
        double check = det;

        if(std::abs(answ - det) < 0.01) {
            std::cout << "Ok ";
        } else {
            std::cout << "\n[Fail]\n" << mat;
            std::cout << "My det = " << answ << std::endl;
            std::cout << "Correct det = " << det << std::endl;
            break;
        }
    }
}


int main() {
    t_matrix();
    return 0;
}
