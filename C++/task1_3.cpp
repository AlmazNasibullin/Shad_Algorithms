#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

template <typename T>
void inverse(T begin, T end)
{
    for (; end - begin > 0; ++begin, --end) {
        std::swap(*begin, *end);
    }
}

template <typename T>
void rotate(T begin, T end, int k)
{
    int length = end - begin + 1;
    k %= length;
    if (k < 0) {
        k += length;
    }
    if (k == 0) {
        return;
    }
    inverse(begin, end - k);
    inverse(end - k + 1, end);
    inverse(begin, end);
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
    const int size = 3;
    std::vector<int> vector(size);
    for (int iteration = 0; iteration < size; ++iteration) {
        vector[iteration] = distribution(generator);
    }
    print(vector.begin(), vector.end());
    for (int k = -5; k < 5; ++k) {
        std::vector<int> copyVector(vector);
        rotate(copyVector.begin(), copyVector.end() - 1, k);
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
    for (int k = -6; k < 6; ++k) {
        int copyArray[size];
        for (int iteration = 0; iteration < size; ++iteration) {
            copyArray[iteration] = array[iteration];
        }
        rotate(&copyArray[0], &copyArray[0] + size - 1, k);
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