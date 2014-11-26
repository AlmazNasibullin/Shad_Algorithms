#include <iostream>
#include <vector>
#include <algorithm>
#include <ostream>
#include <cassert>

template <class T>
class Polynomial
{
public:
    Polynomial()
    {
        coefficients_.push_back(T());
    }
    Polynomial(const T& coefficient)
    {
        coefficients_.push_back(coefficient);
    }
    template <class InputIterator>
    Polynomial(InputIterator begin, InputIterator end)
    {
        coefficients_ = std::vector<T>(begin, end);
        deleteZeroCoefficientsAtHigherDegrees();
    }
    Polynomial& operator=(const Polynomial& polynomial)
    {
        coefficients_ = polynomial.coefficients_;
        return *this;
    }
    bool operator==(const Polynomial& polynomial) const
    {
        return this->degree() == polynomial.degree() &&
                std::equal(coefficients_.begin(), coefficients_.end(),
                        polynomial.coefficients_.begin());
    }
    bool operator!=(const Polynomial& polynomial) const
    {
        return !(*this == polynomial);
    }
    Polynomial operator+(const Polynomial& polynomial) const
    {
        std::vector<T> sum(coefficients_);
        if (coefficients_.size() < polynomial.coefficients_.size()) {
            sum.resize(polynomial.coefficients_.size());
        }
        for (size_t index = 0; index < polynomial.coefficients_.size(); ++index) {
            sum[index] += polynomial[index];
        }
        Polynomial result(sum.begin(), sum.end());
        return result;
    }
    Polynomial operator+(T coefficient) const
    {
        return *this + Polynomial(coefficient);
    }
    Polynomial& operator+=(const Polynomial& polynomial)
    {
        *this = *this + polynomial;
        return *this;
    }
    Polynomial& operator+=(T coefficient)
    {
        *this += Polynomial(coefficient);
        return *this;
    }
    friend Polynomial operator+(T coefficient, const Polynomial& polynomial)
    {
        return Polynomial(coefficient) + polynomial;
    }
    Polynomial operator-(const Polynomial& polynomial) const
    {
        std::vector<T> diff(coefficients_);
        if (coefficients_.size() < polynomial.coefficients_.size()) {
            diff.resize(polynomial.coefficients_.size());
        }
        for (size_t index = 0; index < polynomial.coefficients_.size(); ++index) {
            diff[index] -= polynomial[index];
        }
        Polynomial result(diff.begin(), diff.end());
        return result;
    }
    Polynomial operator-(T coefficient) const
    {
        return *this - Polynomial(coefficient);
    }
    Polynomial& operator -= (const Polynomial& polynomial)
    {
        *this = *this - polynomial;
        return *this;
    }
    Polynomial& operator-=(T coefficient)
    {
        *this -= Polynomial(coefficient);
        return *this;
    }
    friend Polynomial operator-(T coefficient, const Polynomial& polynomial)
    {
        return Polynomial(coefficient) - polynomial;
    }
    Polynomial operator*(const Polynomial& polynomial) const
    {
        if (this->degree() == -1 || polynomial.degree() == -1) {
            return  Polynomial(T());
        }
        int degree = this->degree() + polynomial.degree();
        std::vector<T> product(degree + 1);
        for (int i = 0; i <= degree; ++i) {
            for (int j = 0; j <= i; ++j) {
                if (j <= this->degree() && i - j <= polynomial.degree()) {
                    product[i] += coefficients_[j] * polynomial[i - j];
                }
            }
        }
        return Polynomial(product.begin(), product.end());
    }
    Polynomial operator*(T coefficient) const
    {
        return *this * Polynomial(coefficient);
    }
    Polynomial& operator *= (const Polynomial& polynomial)
    {
        *this = *this * polynomial;
        return *this;
    }
    Polynomial& operator*=(T coefficient)
    {
        *this *= Polynomial(coefficient);
        return  *this;
    }
    friend Polynomial operator*(T coefficient, const Polynomial& polynomial)
    {
        return Polynomial(coefficient) * polynomial;
    }
    int degree() const
    {
        int degree = static_cast<int>(coefficients_.size()) - 1;
        if (degree == 0 && coefficients_[0] == T()) {
            return -1;
        }
        return degree;
    }


    class TProxy
    {
    public:
        TProxy(int degree, Polynomial<T>* polynomial) : degree_(degree), polynomial_(polynomial) {}
        TProxy& operator=(const T& value)
        {
            if (degree_ >= static_cast<int>(polynomial_->coefficients_.size())) {
                polynomial_->coefficients_.resize(degree_ + 1);
            }
            polynomial_->coefficients_.at(degree_) = value;
            polynomial_->deleteZeroCoefficientsAtHigherDegrees();
            return *this;
        }
        operator T() const
        {
            if (degree_ >= static_cast<int>(polynomial_->coefficients_.size())) {
                return T();
            }
            return polynomial_->coefficients_.at(degree_);
        }
        friend std::ostream& operator<<(std::ostream& output, const TProxy& tProxy)
        {
            if (tProxy.degree_ >= static_cast<int>(tProxy.polynomial_->coefficients_.size())) {
                output << T();
            } else {
                output << tProxy.polynomial_->coefficients_.at(tProxy.degree_);
            }
            return output;
        }

    private:
        int degree_;
        Polynomial<T>* polynomial_;
    };


    TProxy operator[](int degree)
    {
        assert(0 <= degree);
        return TProxy(degree, this);
    }
    T operator[](int degree) const
    {
        assert(0 <= degree);
        if (degree > this->degree()) {
            return T();
        }
        return coefficients_[degree];
    }
    T operator()(T point)
    {
        typename std::vector<T>::reverse_iterator iterator = coefficients_.rbegin();
        T value = *iterator;
        ++iterator;
        for (; iterator != coefficients_.rend(); ++iterator) {
            value = *iterator + point * value;
        }
        return value;
    }
    friend std::ostream& operator<<(std::ostream& output, const Polynomial<T>& polynomial)
    {
        int degree = polynomial.degree();
        if (degree == -1) {
            output << T();
            return output;
        }
        for (; degree >=0; --degree) {
            output << polynomial[degree] << "*x^" << degree;
            if (degree > 0) {
                output << " + ";
            }
        }
        return output;
    }


    class PolynomialIterator
    {
    public:
        PolynomialIterator(const typename std::vector<T>::iterator& iterator)
            : iterator_(iterator) {}
        PolynomialIterator& operator=(const PolynomialIterator& polynomialIterator)
        {
            iterator_ = polynomialIterator.iterator_;
            return *this;
        }
        PolynomialIterator& operator++()
        {
            ++iterator_;
            return *this;
        }
        PolynomialIterator operator++(int) const
        {
            PolynomialIterator polynomialIterator = *this;
            ++*this;
            return polynomialIterator;
        }
        PolynomialIterator& operator--()
        {
            --iterator_;
            return *this;
        }
        PolynomialIterator operator--(int) const
        {
            PolynomialIterator polynomialIterator = *this;
            --*this;
            return polynomialIterator;
        }
        T& operator*()
        {
            return *iterator_;
        }
        bool operator==(const PolynomialIterator& polynomialIterator) const
        {
            return iterator_ == polynomialIterator.iterator_;
        }
        bool operator!=(const PolynomialIterator& polynomialIterator) const
        {
            return iterator_ != polynomialIterator.iterator_;
        }

    private:
        typename std::vector<T>::iterator iterator_;
    };


    class PolynomialConstIterator
    {
    public:
        PolynomialConstIterator(const typename std::vector<T>::const_iterator& iterator)
            : iterator_(iterator) {}
        PolynomialConstIterator& operator=(const PolynomialConstIterator& polynomialConstIterator)
        {
            iterator_ = polynomialConstIterator.iterator_;
            return *this;
        }
        PolynomialConstIterator& operator++()
        {
            ++iterator_;
            return *this;
        }
        PolynomialConstIterator operator++(int) const
        {
            PolynomialConstIterator polynomialConstIterator = *this;
            ++*this;
            return polynomialConstIterator;
        }
        PolynomialConstIterator& operator--()
        {
            --iterator_;
            return *this;
        }
        PolynomialConstIterator operator--(int) const
        {
            PolynomialConstIterator polynomialConstIterator = *this;
            --*this;
            return polynomialConstIterator;
        }
        T& operator*()
        {
            return *iterator_;
        }
        bool operator==(const PolynomialConstIterator& polynomialConstIterator) const
        {
            return iterator_ == polynomialConstIterator.iterator_;
        }
        bool operator!=(const PolynomialConstIterator& polynomialConstIterator) const
        {
            return iterator_ != polynomialConstIterator.iterator_;
        }

    private:
        typename std::vector<T>::const_iterator iterator_;
    };


    PolynomialIterator begin()
    {
        return PolynomialIterator(coefficients_.begin());
    }
    PolynomialConstIterator begin() const
    {
        return PolynomialConstIterator(coefficients_.begin());
    }
    PolynomialIterator end()
    {
        return PolynomialIterator(coefficients_.end());
    }
    PolynomialConstIterator end() const
    {
        return  PolynomialConstIterator(coefficients_.end());
    }
    Polynomial operator/(const Polynomial& polynomial) const
    {
        return divide(polynomial, false);
    }
    Polynomial operator%(const Polynomial& polynomial) const
    {
        return divide(polynomial, true);
    }
    Polynomial operator,(const Polynomial& polynomial) const
    {
        if (this->degree() == -1 || polynomial.degree() == -1) {
            return Polynomial<T>(T());
        }
        Polynomial<T> gcd, rest;
        if (this->degree() > polynomial.degree()) {
            gcd = polynomial;
            rest = *this % polynomial;
        } else {
            gcd = *this;
            rest = polynomial % *this;
        }
        while (rest.degree() != -1) {
            std::swap(rest, gcd);
            rest = rest % gcd;
        }
        return gcd;
    }

private:
    std::vector<T> coefficients_;


    Polynomial divide(const Polynomial& polynomial, bool rest) const
    {
        int degreeFirst = this->degree();
        int degreeSecond = polynomial.degree();
        if (degreeSecond == -1) {
            return Polynomial(T());
        }
        if (degreeSecond > degreeFirst) {
            return rest ? *this : Polynomial(T());
        }
        std::vector<T> privateResult(degreeFirst - degreeSecond + 1);
        std::vector<T> dividendResult(coefficients_);
        for (int iteration = degreeFirst - degreeSecond; iteration >= 0 ; --iteration) {
            privateResult[iteration] = dividendResult[degreeFirst] / polynomial[degreeSecond];
            dividendResult.resize(degreeFirst);
            --degreeFirst;
            for (int index = degreeFirst; index > degreeFirst - degreeSecond; --index) {
                dividendResult[index] -= (privateResult[iteration] *
                    polynomial[degreeSecond - 1 + (index - degreeFirst)]);
            }
        }
        return rest ? Polynomial(dividendResult.begin(), dividendResult.end()) :
            Polynomial(privateResult.begin(), privateResult.end());
    }
    void deleteZeroCoefficientsAtHigherDegrees()
    {
        int toResize = static_cast<int>(coefficients_.size()) - 1;
        while (toResize > 0 && coefficients_[toResize] == T()) {
            --toResize;
        }
        coefficients_.resize(toResize + 1);
    }
};
