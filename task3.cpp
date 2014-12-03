#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <utility>
#include <algorithm>

class HashFunction
{
public:
    HashFunction();
    HashFunction(long long firstCoeficient, long long secondCoeficient, long long secondModule);
    long long operator()(long long number) const;

public:
    static long long firstModule_; // prime number more than secondModule

private:
    long long firstCoeficient_;
    long long secondCoeficient_;
    long long secondModule_;
};

long long HashFunction::firstModule_ = 2147483053;

class InternalFixedSet
{
public:
    void initialize(const std::vector<long long>& data, const std::vector<size_t>& indexes);
    bool contains(long long element) const;

private:
    std::vector<long long> elements;
    HashFunction hashFunction;
};

class ExternalFixedSet
{
public:
    ExternalFixedSet() {}
    void initialize(const std::vector<long long>& data);
    bool contains(long long element) const;

private:
    HashFunction hashFunction;
    std::vector<InternalFixedSet> internalFixedSets;
};

std::vector<long long> readNumbers();

void processRequests(const std::vector<long long>& requests,
        const ExternalFixedSet& externalFixedSet);

int main()
{
    std::vector<long long> numbers = readNumbers();
    std::vector<long long> requests = readNumbers();
    ExternalFixedSet externalFixedSet;
    externalFixedSet.initialize(numbers);
    processRequests(requests, externalFixedSet);
    return 0;
}
