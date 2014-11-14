#include <iostream>
#include <vector>
#include <algorithm>
#include <list>

class Segment
{
public:
    Segment();
    Segment(int start, int length, int indexInHeap, bool free);
    int getLength() const;
    int getStart() const;
    int getIndex() const;
    void setIndex();
    bool isFree() const;
    void makeFree();
private:
    int start_;
    int length_;
    int indexInHeap_;
    bool free_;
};

std::vector<int> readQueries(int* memorySize);

template <class T, class CallBack, class Comparator>
class Heap
{
public:
    Heap();
    void insert(const T& key);
    void erase(int index);
    T& extractMax();
    T getMax() const;
    int getHeapSize() const;
    bool empty() const;
private:
    std::vector<T> elements;
    CallBack callBack;

    void swap(int firstIndex, int secondIndex);
    void heapify(int index);
};

class CallBackForSegments
{
public:
    void operator()(Segment& segment, int newIndexInHeap);
};

// compares two free segments on length
// if length of first free segment is less than length of second returns true
// if length of first free segment is more than length of second returns false
// otherwise similarly compares starts
class ComparatorForSegments
{
public:
    bool operator()(const std::list<Segment>::iterator& first,
    const std::list<Segment>::iterator& second);
};

class MemoryManager
{
public:
    MemoryManager(int memorySize);
    int allocateMemory(int query);
    int freeMemory(int query);
private:
    std::list<Segment> allSegments;
    Heap<std::list<Segment>::iterator, CallBackForSegments, ComparatorForSegments> freeSegments;
    std::vector<std::list<Segment>::iterator> infoOfAllocatedQueries;
    std::vector<bool> isAllocated;
};

std::vector<int> parseQueriesForAllocationOrFreeMemory(const std::vector<int>& queries,
    int memorySize);

int main()
{
    int memorySize;
    std::vector<int> queries = readQueries(&memorySize);
    std::vector<int> answers = parseQueriesForAllocationOrFreeMemory(queries, memorySize);
    for (int answer: answers) {
        std::cout << answer << "\n";
    }
    return 0;
}
