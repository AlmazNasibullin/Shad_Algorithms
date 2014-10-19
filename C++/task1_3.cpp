#include <iostream>
#include <vector>
#include <random>
#include <chrono>

template <typename T>
void rotateToOne(T begin, T end, bool toRight)
{
    if (toRight) {
        --end;
        for (; end != begin; --end) {
            std::swap(*end, *(end - 1));
        }
    } else {
        for (; begin + 1 != end; ++begin) {
            std::swap(*begin, *(begin + 1));
        }
    }
}

template <typename T>
void rotate(T begin, T end, int k)
{
    if (k > 0) {
        for (int iteration = 0; iteration < k; ++iteration) {
            rotateToOne(begin, end, true);
        }
    } else {
        for (int iteration = 0; iteration < std::abs(k); ++iteration) {
            rotateToOne(begin, end, false);
        }
    }
}

template <typename T>
void print(T begin, T end)
{
    for (; begin != end; ++begin) {
        std::cout << *begin << " ";
    }
    std::cout << "\n";
}

void rotateVector()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(0, 9);
    const int size = 5;
    std::vector<int> vector(size);
    for (int iteration = 0; iteration < size; ++iteration) {
        vector[iteration] = distribution(generator);
    }
    print(vector.begin(), vector.end());
    for (int k = -2; k < 10; ++k) {
        std::vector<int> copyVector(vector);
        rotate(copyVector.begin(), copyVector.end(), k);
        std::cout << "k = " << k << " : ";
        print(copyVector.begin(), copyVector.end());
    }
}

void rotateArray()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(0, 9);
    const int size = 5;
    int array[size];
    for (int iteration = 0; iteration < size; ++iteration) {
        array[iteration] = distribution(generator);
    }
    print(&array[0], &array[0] + size);
    for (int k = -5; k < 7; ++k) {
        int copyArray[size];
        for (int iteration = 0; iteration < size; ++iteration) {
            copyArray[iteration] = array[iteration];
        }
        rotate(&copyArray[0], &copyArray[0] + size, k);
        std::cout << "k = " << k << " : ";
        print(&copyArray[0], &copyArray[0] + size);
    }
}

int main()
{
    std::cout << "vector:\n";
    rotateVector();
    std::cout << "\narray:\n";
    rotateArray();
    return 0;
}