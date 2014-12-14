#include <iostream>
#include <vector>
#include <list>
#include <iterator>
#include <cassert>
#include <random>
#include <algorithm>

template <typename T, typename V>
bool binarySearch(T begin, T end, const V& val)
{
    auto distance = std::distance(begin, end);
    if (distance == 0) {
        return false;
    }
    while (distance > 1) {
        T mid = begin;
        std::advance(mid, distance / 2);
        if (val < *mid) {
            end = mid;
        } else {
            begin = mid;
        }
        distance = std::distance(begin, end);
    }
    if (*begin == val) {
        return true;
    }
    return false;
}

void testBinarySearchOnVector()
{
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<int> sizeDistribution(0, 50);
    std::uniform_int_distribution<int> numbersDistribution(-100, 100);
    for (int i = 0; i < 100000; ++i) {
        int size = sizeDistribution(generator);
        std::vector<int> vector(size);
        for (int j = 0; j < size; ++j) {
            vector[j] = numbersDistribution(generator);
        }
        std::sort(vector.begin(), vector.end());
        for (int k = 0; k < 100; ++k) {
            int number = numbersDistribution(generator);
            assert(std::binary_search(vector.begin(), vector.end(), number)
                == binarySearch(vector.begin(), vector.end(), number));
        }
    }
}

void testBinarySearchOnList()
{
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<int> sizeDistribution(0, 100);
    std::uniform_int_distribution<int> numbersDistribution(-200, 200);
    for (int i = 0; i < 10000; ++i) {
        int size = sizeDistribution(generator);
        std::list<int> list;
        for (int j = 0; j < size; ++j) {
            list.push_back(numbersDistribution(generator));
        }
        list.sort();
        for (int k = 0; k < 100; ++k) {
            int number = numbersDistribution(generator);
            assert(std::binary_search(list.begin(), list.end(), number)
                == binarySearch(list.begin(), list.end(), number));
        }
    }
}

void testBinarySearchOnArray()
{
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<int> sizeDistribution(0, 70);
    std::uniform_int_distribution<int> numbersDistribution(-150, 200);
    for (int i = 0; i < 100000; ++i) {
        int size = sizeDistribution(generator);
        int* array = new int[size];
        for (int j = 0; j < size; ++j) {
            array[j] = numbersDistribution(generator);
        }
        std::sort(array, array + size);
        for (int k = 0; k < 100; ++k) {
            int number = numbersDistribution(generator);
            assert(std::binary_search(array, array + size, number)
                == binarySearch(array, array + size, number));
        }
        delete [] array;
    }
}

int main()
{
    testBinarySearchOnVector();
    std::cout << "Test on vector passed\n";
    testBinarySearchOnList();
    std::cout << "Test on list passed\n";
    testBinarySearchOnArray();
    std::cout << "Test on array passed\n";
    return 0;
}
