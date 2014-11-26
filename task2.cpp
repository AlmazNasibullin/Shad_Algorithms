#include <iostream>
#include <vector>
#include <algorithm>
#include <list>

class SegmentOfMemeoryCells
{
public:
    SegmentOfMemeoryCells();
    SegmentOfMemeoryCells(int start, int length, int indexInHeap, bool free);
    int getLength() const;
    int getStart() const;
    int getIndexInHeap() const;
    void setIndexInHeap();
    bool isFree() const;
    void makeFree();
private:
    int start_;
    int length_;
    int indexInHeap_;
    bool free_;
};

std::vector<int> readQueries();

template <class T, class CallBack, class Comparator>
class Heap
{
public:
    Heap();
    void insert(const T& key);
    void erase(int index);
    T extractMax();
    T& getMax() const;
    int getHeapSize() const;
    bool empty() const;

private:
    void swap(int firstIndex, int secondIndex);
    void heapify(int index);

private:
    std::vector<T> elements;
    CallBack callBackForSegments;
};

class CallBackForSegments
{
public:
    void operator()(SegmentOfMemeoryCells& segmentOfMemeoryCells, int newIndexInHeap);
};

// compares two free segments on length
// if length of first free segment is less than length of second returns true
// if length of first free segment is more than length of second returns false
// otherwise similarly compares starts
class ComparatorForSegmentsOnLength
{
public:
    bool operator()(const std::list<SegmentOfMemeoryCells>::iterator& first,
    const std::list<SegmentOfMemeoryCells>::iterator& second);
};

class MemoryManager
{
public:
    MemoryManager(int memorySize);
    int allocateMemory(int query);
    int freeMemory(int query);
private:
    std::list<SegmentOfMemeoryCells> allSegments;
    Heap<std::list<SegmentOfMemeoryCells>::iterator, CallBackForSegments,
        ComparatorForSegmentsOnLength> freeSegments;
};

std::vector<int> runMemoryManagerOnQueries(const std::vector<int>& queries, int memorySize);

int main()
{
    int memorySize;
    std::cin >> memorySize;
    std::vector<int> queries = readQueries();
    std::vector<int> answers = runMemoryManagerOnQueries(queries, memorySize);
    for (int answer: answers) {
        std::cout << answer << "\n";
    }
    return 0;
}
