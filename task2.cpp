#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <utility>

class MemorySegment
{
public:
    MemorySegment() : start_(0), length_(0), indexInHeap_(0), free_(true) {}
    MemorySegment(int start, int length, int indexInHeap, bool free):
      start_(start), length_(length), indexInHeap_(indexInHeap), free_(free) {}
    int getLength() const
    {
        return length_;
    }
    int getStart() const
    {
        return start_;
    }
    int getIndexInHeap() const
    {
        return indexInHeap_;
    }
    void setStart(int start)
    {
        start_ = start;
    }
    int setLength(int length)
    {
        length_ = length;
    }
    void setIndexInHeap(int indexInHeap)
    {
        indexInHeap_ = indexInHeap;
    }
    bool isFree() const
    {
        return free_;
    }
    void makeFree()
    {
        free_ = true;
    }
private:
    int start_;
    int length_;
    int indexInHeap_;
    bool free_;
};

std::vector<int> readQueries()
{
    int numberOfQueries;
    std::cin >> numberOfQueries;
    std::vector<int> queries(numberOfQueries);
    for (int& query : queries) {
        std::cin >> query;
    }
    return queries;
}

template <class T, class PositionModifier, class Comparator>
class Heap
{
public:
    Heap() {}
    Heap(PositionModifier positionModifier, Comparator comparator) :
        positionModifier_(positionModifier), comparator_(comparator) {}
    void insert(const T& key)
    {
        int index = getHeapSize();
        elements_.push_back(key);
        positionModifier_(elements_[index], index);
        while (index > 0 && comparator_(elements_[getParentIndex(index)], elements_[index])) {
            swap(index, getParentIndex(index));
            index = getParentIndex(index);
        }
    }
    void erase(int index)
    {
        swap(index, getHeapSize() - 1);
        elements_.resize(getHeapSize() - 1);
        while (index > 0 && comparator_(elements_[getParentIndex(index)], elements_[index])) {
            swap(index, getParentIndex(index));
            index = getParentIndex(index);
        }
        heapify(index);
    }
    T extractMax()
    {
        T max = elements_[0];
        swap(0, getHeapSize() - 1);
        elements_.resize(getHeapSize() - 1);
        heapify(0);
        return max;
    }
    const T& getMax() const
    {
        return elements_[0];
    }
    int getHeapSize() const
    {
        return static_cast<int>(elements_.size());
    }
    bool empty() const
    {
        return elements_.empty();
    }

private:
    void swap(int firstIndex, int secondIndex)
    {
        std::swap(elements_[firstIndex], elements_[secondIndex]);
        positionModifier_(elements_[firstIndex], firstIndex);
        positionModifier_(elements_[secondIndex], secondIndex);
    }
    void heapify(int index)
    {
        int leftChild = getLeftChildIndex(index);
        int rightChild = getRightChildIndex(index);
        int largest = index;
        if (isThereLeftChild(index) && comparator_(elements_[index], elements_[leftChild])) {
            largest = leftChild;
        }
        if (isThereRightChild(index) && comparator_(elements_[largest], elements_[rightChild])) {
            largest = rightChild;
        }
        if (largest != index) {
            swap(index, largest);
            heapify(largest);
        }
    }
    int getParentIndex(int index) const
    {
        return (index - 1) / 2;
    }
    int getLeftChildIndex(int index) const
    {
        return 2 * index + 1;
    }
    int getRightChildIndex(int index) const
    {
        return 2 * index + 2;
    }
    bool isThereLeftChild(int index) const
    {
        return 2 * index + 1 < getHeapSize();
    }
    bool isThereRightChild(int index) const
    {
        return 2 * index + 2 < getHeapSize();
    }

private:
    std::vector<T> elements_;
    PositionModifier positionModifier_;
    Comparator comparator_;
};

class PositionModifierForSegments
{
public:
    void operator()(std::list<MemorySegment>::iterator memorySegment, int newIndexInHeap)
    {
        memorySegment->setIndexInHeap(newIndexInHeap);
    }
};

// compares two free segments on length
// if length of first free segment is less than length of second returns true
// if length of first free segment is more than length of second returns false
// otherwise similarly compares starts
class ComparatorForSegmentsOnLength
{
public:
    bool operator()(const std::list<MemorySegment>::iterator& first,
        const std::list<MemorySegment>::iterator& second)
    {
        if (first->getLength() == second->getLength()) {
            return first->getStart() > second->getStart();
        }
        return first->getLength() < second->getLength();
    }
};

class MemoryManager
{
public:
    explicit MemoryManager(int memorySize)
    {
        allSegments.push_back(MemorySegment(0, memorySize, 0, true));
        freeSegments.insert(allSegments.begin());
    }
    std::pair<int, std::list<MemorySegment>::iterator> allocateMemory(int lengthToAllocate)
    {
        if (!freeSegments.empty() &&  freeSegments.getMax()->getLength() >= lengthToAllocate) {
            std::list<MemorySegment>::iterator max = freeSegments.extractMax();
            int answer = max->getStart() + 1;
            std::list<MemorySegment>::iterator allocated = allSegments.insert(max,
                MemorySegment(max->getStart(), lengthToAllocate, -1, false));
            if (max->getLength() - lengthToAllocate > 0) {
                std::list<MemorySegment>::iterator newInserted = allSegments.insert(max,
                    MemorySegment(max->getStart() + lengthToAllocate, max->getLength()
                        - lengthToAllocate, freeSegments.getHeapSize(), true));
                freeSegments.insert(newInserted);
            }
            allSegments.erase(max);
            return std::make_pair(answer, allocated);
        }
        return std::make_pair(-1, allSegments.end());
    }
    void freeMemory(std::list<MemorySegment>::iterator allocated)
    {
        allocated->makeFree();
        freeSegments.insert(allocated);
        mergeFreeSegmentWithPreviousFreeSegment(allocated);
        if (nextSegmentIsFree(allocated)) {
            mergeFreeSegmentWithPreviousFreeSegment(std::next(allocated, 1));
        }
    }

private:
    bool previousSegmentIsFree(std::list<MemorySegment>::iterator segment) const
    {
        return segment != allSegments.begin() && std::prev(segment, 1)->isFree();
    }
    bool nextSegmentIsFree(std::list<MemorySegment>::iterator segment) const
    {
        return std::next(segment, 1) != allSegments.end() && std::next(segment, 1)->isFree();
    }
    void mergeFreeSegmentWithPreviousFreeSegment(std::list<MemorySegment>::iterator segment)
    {
        if (previousSegmentIsFree(segment)) {
            std::list<MemorySegment>::iterator previous = std::prev(segment, 1);
            segment->setStart(previous->getStart());
            segment->setLength(previous->getLength() + segment->getLength());
            freeSegments.erase(previous->getIndexInHeap());
            freeSegments.erase(segment->getIndexInHeap());
            freeSegments.insert(segment);
            allSegments.erase(previous);
        }
    }

private:
    std::list<MemorySegment> allSegments;
    Heap<std::list<MemorySegment>::iterator, PositionModifierForSegments,
        ComparatorForSegmentsOnLength> freeSegments;
};

std::vector<int> runMemoryManagerOnQueries(const std::vector<int>& queries, int memorySize)
{
    MemoryManager memoryManager(memorySize);
    std::vector<std::list<MemorySegment>::iterator> infoOfAllocatedQueries(queries.size());
    std::vector<bool> isAllocated(queries.size(), false);
    std::vector<int> answers;


    for (size_t index = 0; index < queries.size(); ++index)
    {
        if (queries[index] > 0) {
            std::pair<int, std::list<MemorySegment>::iterator> answer =
                memoryManager.allocateMemory(queries[index]);
            answers.push_back(answer.first);
            if (answer.first != -1) {
                isAllocated[index] = true;
                infoOfAllocatedQueries[index] = answer.second;
            }
        } else {
            int query = std::abs(queries[index]);
            if (isAllocated[query - 1]) {
                std::list<MemorySegment>::iterator allocated = infoOfAllocatedQueries[query - 1];
                memoryManager.freeMemory(allocated);
            }
        }
    }
    return answers;
}

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
