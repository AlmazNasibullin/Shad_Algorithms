#include <iostream>
#include <vector>
#include <algorithm>
#include <list>

class Cut
{
public:
    Cut();
    Cut(int start, int length, bool free);
    int getLength() const;
    int getStart() const;
    int isFree() const;
    void makeFree();
private:
    int start_;
    int length_;
    bool free_;
};

std::vector<int> readInputData(int* memorySize, int* numberOfQueries);

// is used to store index in the heap of free cut
struct CutAndIndex
{
    CutAndIndex(Cut cut, int index);
    Cut cut_;
    int index_;
};

// compares two free cuts on length
// if length of first free cut is less than length of second returns true
// if length of first free cut is more than length of second returns false
// otherwise similarly compares starts
bool compareFreeCutsOnLength(const std::list<CutAndIndex>::iterator firstFreeCut,
    const std::list<CutAndIndex>::iterator& secondFreeCut);

class HeapOfFreeCuts
{
public:
    HeapOfFreeCuts();
    void swap(int firstIndex, int secondIndex);
    void increaseKey(int index, std::list<CutAndIndex>::iterator key);
    void insert(std::list<CutAndIndex>::iterator key);
    void erase(int index);
    void heapify(int index);
    std::list<CutAndIndex>::iterator extractMax();
    std::list<CutAndIndex>::iterator max();
    int getHeapSize() const;
    bool empty() const;
private:
    std::vector<std::list<CutAndIndex>::iterator> freeCuts;
    int heapSize;
};

void allocateMemory(const std::vector<int>& queries, int queryNumber, HeapOfFreeCuts& freeCuts,
    std::vector<int>& answers, std::vector<std::list<CutAndIndex>::iterator>& infoOfAllocatedQueries,
    std::vector<bool>& isAllocated);

void freeMemory(const std::vector<int>& queries, int queryNumber, HeapOfFreeCuts& freeCuts,
    std::vector<int>& answers, std::vector<std::list<CutAndIndex>::iterator>& infoOfAllocatedQueries,
    std::vector<bool>& isAllocated);

std::vector<int> parseQueriesForAllocationOrFreeMemory(const std::vector<int>& queries,
    int memorySize, int numberOfQueries);

int main()
{
    int memorySize, numberOfQueries;
    std::vector<int> queries = readInputData(&memorySize, &numberOfQueries);
    std::vector<int> answers = parseQueriesForAllocationOrFreeMemory(queries, memorySize,
        numberOfQueries);
    for (int answer: answers) {
        std::cout << answer << "\n";
    }
    return 0;
}
