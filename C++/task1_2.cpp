#include <iostream>
#include <vector>
#include <random>
#include <chrono>

int gcdex(int firstNumber, int secondNumber, int& firstCoefficient, int& secondCoefficient)
{
    // I used single-letter variables as I couldn't find suitable not single-letter names
    int p = 1;
    int q = 0;
    int r = 0;
    int s = 1;
    while (firstNumber != 0 && secondNumber != 0) {
        if (firstNumber >= secondNumber) {
            firstNumber = firstNumber - secondNumber;
            p -= r;
            q -= s;
        } else {
            secondNumber = secondNumber - firstNumber;
            r -= p;
            s -= q;
        }
    }
    if (firstNumber != 0) {
        firstCoefficient = p;
        secondCoefficient = q;
        return firstNumber;
    } else {
        firstCoefficient = r;
        secondCoefficient = s;
        return secondNumber;
    }
}

int findInverseElementInRing(int elementInRing, int module)
{
    int firstCoefficient, secondCoefficient;
    if (gcdex(elementInRing, module, firstCoefficient, secondCoefficient) != 1) {
        return -1;
    }
    if (firstCoefficient < 0) {
        firstCoefficient += module;
    }
    return firstCoefficient;
}

void exampleOfGCD(std::mt19937& generator)
{
    std::uniform_int_distribution<int> distribution(1, 15);
    const int numberOfIteration = 10;
    for (int iteration = 0; iteration < numberOfIteration; ++iteration) {
        int firstNumber = distribution(generator);
        int secondNumber = distribution(generator);
        int firstCoefficient, secondCoefficient;
        int gcd = gcdex(firstNumber, secondNumber, firstCoefficient, secondCoefficient);
        std:: cout << "GCD(" << firstNumber << ", " << secondNumber << ") = " << gcd << " = " <<
        firstNumber << " * " << firstCoefficient << " + " << secondNumber << 
        " * " << secondCoefficient << "\n";
    }
}

void exampleOfInverseElementInRing(std::mt19937& generator)
{
    std::uniform_int_distribution<int> firstDistribution(5, 20);
    const int numberOfIteration = 10;
    for (int iteration = 0; iteration < numberOfIteration; ++iteration) {
        int module = firstDistribution(generator);
        std::uniform_int_distribution<int> secondDistribution(0, module - 1);
        int elementInRing = secondDistribution(generator);
        int inverseElement = findInverseElementInRing(elementInRing, module);
        if (inverseElement == -1) {
            std::cout << "No inverse element to " << elementInRing << " modulo " << module << "\n";
        } else {
            std::cout << inverseElement << " is inverse element to " << elementInRing << " modulo " <<
            module << "\n";
        }
    }
}

int main()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    exampleOfGCD(generator);
    std::cout << "\n";
    exampleOfInverseElementInRing(generator);
    return 0;
}