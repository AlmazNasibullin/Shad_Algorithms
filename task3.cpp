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
        long long result = ((linearCoefficient_ * static_cast<long long>(number) + constant_)
            % primeModule_) % module_;
        if (result < 0) {
            result += module_;
        }
        return result;
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
    void initialize(const std::vector<int>& data, const std::vector<size_t>& indexes)
    {
        if (indexes.empty()) {
            return;
        }
        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::uniform_int_distribution<long long> firstDistribution(1, primeNumber);
        std::uniform_int_distribution<long long> secondDistribution(0, primeNumber);
        long long size = static_cast<long long>(indexes.size() * indexes.size());
        elements_.resize(size);
        bool stop = false;
        while (!stop) {
            hashFunction_ = HashFunction(firstDistribution(generator),
                secondDistribution(generator), size);
            std::fill(elements_.begin(), elements_.end(), std::numeric_limits<long long>::max());
            bool again = false;
            for (size_t i = 0; i < indexes.size(); ++i) {
                long long index = hashFunction_(data[indexes[i]]);
                if (elements_[index] == std::numeric_limits<long long>::max()) {
                    elements_[index] = data[indexes[i]];
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
        return elements_[hashFunction_(element)] == element ? true : false;
    }

private:
    std::vector<long long> elements_;
    HashFunction hashFunction_;
};

class FixedSet
{
public:
    FixedSet() {}
    void initialize(const std::vector<int>& data)
    {
        std::vector<std::vector<size_t>> indexes(data.size());
        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::uniform_int_distribution<long long> firstDistribution(1, primeNumber - 1);
        std::uniform_int_distribution<long long> secondDistribution(0, primeNumber - 1);
        long long size = static_cast<long long>(data.size());
        bool stop = false;
        while (!stop) {
            hashFunction_ = HashFunction(firstDistribution(generator),
                secondDistribution(generator), size);
            for (std::vector<size_t>& index : indexes) {
                index.resize(0);
            }
            for (size_t i = 0; i < data.size(); ++i) {
                long long index = hashFunction_(data[i]);
                indexes[index].push_back(i);
            }
            size_t sumOfSquaresOfLength = 0;
            for (std::vector<size_t>& index : indexes) {
                sumOfSquaresOfLength += index.size() * index.size();
            }
            if (sumOfSquaresOfLength < 3 * indexes.size()) {
                stop = true;
            }
        }
        for (size_t i = 0; i < data.size(); ++i) {
            hashTables_.push_back(NoCollisionsQuadraticMemoryHashTable());
            hashTables_[i].initialize(data, indexes[i]);
        }
    }
    bool contains(int element) const
    {
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

void printFormattedResult(const std::vector<bool>& result)
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
    printFormattedResult(result);
    return 0;
}
