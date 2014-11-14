#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <utility>

class PerfectHashingTable
{
public:
    explicit PerfectHashingTable(long long primeNumber);
    void build(const std::vector<long long>& data);
    bool contains(long long element);
private:
    long long primeNumber_;
    long long firstCoeficient;
    long long secondCoeficient;
    std::vector<std::vector<long long> > hashingTable;
    std::vector<std::pair<long long, long long> > coeficients;


    void buildFirstHash(const std::vector<long long>& data);
    void buildSecondHash();
};

class FixedSet {
public:
    explicit FixedSet(long long primeNumber);
    void initialize(const std::vector<long long>& numbers);
    bool contains(long long number);
private:
    PerfectHashingTable perfectHashingTable;
};

std::vector<long long> readInputNumbers();

std::vector<long long> readInputRequests();

void parsingRequests(const std::vector<long long>& requests, const FixedSet& fixedSet);

int main()
{
    long long int primeNumber = 17179869209;
    std::vector<long long> numbers = readInputNumbers();
    std::vector<long long> requests = readInputRequests();
    FixedSet fixedSet(primeNumber);
    fixedSet.initialize(numbers);
    parsingRequests(requests, fixedSet);
    return 0;
}
