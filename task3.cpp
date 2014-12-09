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
    HashFunction();
    HashFunction(long long nonzeroReminderOfPrimeModule, long long reminderOfPrimeModule,
            long long reModule, long long primeModule = primeNumber);
    long long operator()(int number) const;

private:
    long long nonzeroReminderOfPrimeModule_;
    long long reminderOfPrimeModule_;
    long long primeModule_;
    long long reModule_;
};

class NoCollisionsQuadraticMemoryHashTable
{
public:
    explicit NoCollisionsQuadraticMemoryHashTable(const std::vector<size_t>& indexes);
    void initialize(const std::vector<int>& data);
    bool contains(long long element) const;

private:
    std::vector<long long> elements_;
    std::vector<size_t> indexes_;
    HashFunction hashFunction_;
};

class FixedSet
{
public:
    FixedSet();
    void initialize(const std::vector<int>& data);
    bool contains(int element) const;

private:
    HashFunction hashFunction_;
    std::vector<NoCollisionsQuadraticMemoryHashTable> noCollisionsQuadraticMemoryHashTables_;
};

std::vector<int> readNumbers();

std::vector<std::string> processRequests(const std::vector<int>& requests, const FixedSet& fixedSet);

template <typename T>
void print(const std::vector<T>& array);

int main()
{
    std::vector<int> numbers = readNumbers();
    std::vector<int> requests = readNumbers();
    FixedSet fixedSet;
    fixedSet.initialize(numbers);
    std::vector<std::string> result = processRequests(requests, fixedSet);
    print(result);
    return 0;
}
