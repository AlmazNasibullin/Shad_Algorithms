#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <utility>
#include <algorithm>
#include <string>

const long long primeNumber = 2147483053;

class HashFunction
{
public:
    HashFunction() {}
    HashFunction(long long linearCoefficient, long long constant, long long module,
            long long primeModule = primeNumber)
    {
        linearCoefficient_ = linearCoefficient;
        constant_ = constant;
        primeModule_ = primeModule;
        module_ = module;
    }
    long long operator()(int number) const
    {
        long long result = static_cast<long long>(number) % primeModule_;
        if (result < 0) {
            result += primeModule_;
        }
        return ((linearCoefficient_ *  result + constant_) % primeModule_) % module_;
    }

private:
    long long linearCoefficient_;
    long long constant_;
    long long primeModule_;
    long long module_;
};

class NoCollisionsQuadraticMemoryHashTable
{
public:
    NoCollisionsQuadraticMemoryHashTable() {}
    void initialize(const std::vector<int>& data)
    {
        if (data.empty()) {
            return;
        }
        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::uniform_int_distribution<long long> firstDistribution(1, primeNumber);
        std::uniform_int_distribution<long long> secondDistribution(0, primeNumber);
        long long size = static_cast<long long>(data.size() * data.size());
        elements_.resize(size);
        bool stop = false;
        while (!stop) {
            hashFunction_ = HashFunction(firstDistribution(generator),
                secondDistribution(generator), size);
            std::fill(elements_.begin(), elements_.end(), std::numeric_limits<int>::max());
            bool again = false;
            for (size_t i = 0; i < data.size(); ++i) {
                long long index = hashFunction_(data[i]);
                if (elements_[index] == std::numeric_limits<int>::max()) {
                    elements_[index] = data[i];
                } else {
                    again = true;
                    break;
                }
            }
            if (!again) {
                stop = true;
            }
        }
    }
    bool contains(int element) const
    {
        if (elements_.empty()) {
            return false;
        }
        return elements_[hashFunction_(element)] == element;
    }

private:
    std::vector<int> elements_;
    HashFunction hashFunction_;
};

class FixedSet
{
public:
    FixedSet() {}
    void initialize(const std::vector<int>& data)
    {
        std::vector<std::vector<int>> numbers(data.size());
        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::uniform_int_distribution<long long> firstDistribution(1, primeNumber - 1);
        std::uniform_int_distribution<long long> secondDistribution(0, primeNumber - 1);
        long long size = static_cast<long long>(data.size());
        bool stop = false;
        while (!stop) {
            hashFunction_ = HashFunction(firstDistribution(generator),
                secondDistribution(generator), size);
            for (size_t i = 0; i < numbers.size(); ++i) {
                numbers[i].resize(0);
            }
            for (size_t i = 0; i < data.size(); ++i) {
                long long index = hashFunction_(data[i]);
                numbers[index].push_back(data[i]);
            }
            long long sumOfSquaresOfLength = 0;
            for (size_t i = 0; i < numbers.size(); ++i) {
                sumOfSquaresOfLength += static_cast<long long>(numbers[i].size())
                    * static_cast<long long>(numbers[i].size());
            }
            if (sumOfSquaresOfLength < 3 * static_cast<long long>(numbers.size())) {
                stop = true;
            }
        }
        for (size_t i = 0; i < numbers.size(); ++i) {
            hashTables_.push_back(NoCollisionsQuadraticMemoryHashTable());
            hashTables_[i].initialize(numbers[i]);
        }
    }
    bool contains(int element) const
    {
        if (hashTables_.empty()) {
            return false;
        }
        long long index = hashFunction_(element);
        return hashTables_[index].contains(element);
    }

private:
    HashFunction hashFunction_;
    std::vector<NoCollisionsQuadraticMemoryHashTable> hashTables_;
};

std::vector<int> readNumbers()
{
    int amountOfNumbers;
    std::cin >> amountOfNumbers;
    std::vector<int> numbers(amountOfNumbers);
    for (int& number : numbers) {
        std::cin >> number;
    }
    return numbers;
}

std::vector<bool> processRequests(const std::vector<int>& requests, const FixedSet& fixedSet)
{
    std::vector<bool> result;
    for (int request : requests) {
        result.push_back(fixedSet.contains(request));
    }
    return result;
}

void printResponses(const std::vector<bool>& result)
{
    for (bool answer : result) {
        if (answer) {
            std::cout << "Yes\n";
        } else {
            std::cout << "No\n";
        }
    }
}

int main()
{
    std::vector<int> numbers = readNumbers();
    std::vector<int> requests = readNumbers();
    FixedSet fixedSet;
    fixedSet.initialize(numbers);
    std::vector<bool> result = processRequests(requests, fixedSet);
    printResponses(result);
    return 0;
}
