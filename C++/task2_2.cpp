#include <iostream>
#include <vector>
#include <cassert>
#include "polynomial.h"

int main()
{
    Polynomial<double> zeroPolynomial(0);
    Polynomial<double> constant(1);
    assert(zeroPolynomial.degree() == -1);
    assert(constant.degree()== 0);


    std::vector<std::vector<double> > coefficients = {{1, 2, 1},
                                                     {1, 2, 1},
                                                     {1, 0, 1},
                                                     {0, 1},
                                                     {0, 1, 0, 1},
                                                     {1, 1}};
    std::vector<Polynomial<double> > polynomials;
    for (size_t i = 0; i < coefficients.size(); ++i) {
        polynomials.push_back(Polynomial<double>(coefficients[i].begin(), coefficients[i].end()));
    }


    assert(polynomials[0] == polynomials[1]);
    assert(polynomials[1] != polynomials[2]);


    std::cout << "(" << polynomials[1] << ") + (" << polynomials[2] << ") = " << polynomials[1]
        + polynomials[2] << "\n";
    std::cout << "(" << polynomials[1] << ") - (" << polynomials[2] << ") = " << polynomials[1]
        - polynomials[2] << "\n";
    std::cout << "(" << polynomials[2] << ") * (" << polynomials[3] << ") = " << polynomials[2]
        * polynomials[3] << "\n\n";


    assert(polynomials[0].degree() == 2);
    assert(polynomials[3].degree() == 1);


    std::cout << polynomials[3] << "\n";
    polynomials[3][0] = polynomials[3][0] + 1;
    std::cout << polynomials[3] << "\n";
    polynomials[3][5] = -2;
    std::cout << polynomials[3] << "\n";
    polynomials[3][5] = 0;
    std::cout << polynomials[3] << "\n\n";


    std::cout << "if x = 1  " << polynomials[0] << " = " << polynomials[0](1) << "\n";
    std::cout << "if x = -2 " << polynomials[0] << " = " << polynomials[0](-2) << "\n";
    std::cout << "if x = 0  " << polynomials[0] << " = " << polynomials[0](0) << "\n\n";


    std::cout << polynomials[3] << "\n";
    for (Polynomial<double>::PolynomialIterator polynomialIterator = polynomials[3].begin();
            polynomialIterator != polynomials[3].end(); ++polynomialIterator) {
        *polynomialIterator += 100;
    }
    std::cout << polynomials[3] << "\n\n";

    std::cout << "(" << polynomials[0] << ") / (" << polynomials[5] << ") = " << polynomials[0]
        / polynomials[5] << "\n";
    std::cout << "(" << polynomials[0] << ") % (" << polynomials[5] << ") = " << polynomials[0]
        % polynomials[5] << "\n\n";


    std::cout << "GCD(" << polynomials[0] << ", " << polynomials[1] << ") = " << (polynomials[0],
        polynomials[1]) << "\n";
    std::cout << "GCD(" << polynomials[1] << ", " << polynomials[2] << ") = " << (polynomials[1],
        polynomials[2]) << "\n";
    std::cout << "GCD(" << polynomials[2] << ", " << polynomials[4] << ") = " << (polynomials[2],
        polynomials[4]) << "\n";
    return 0;
}