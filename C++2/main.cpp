#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <list>
#include "list.hpp"
#include "allocator.hpp"

template <class InputIterator>
void print(InputIterator first, InputIterator last) {
    for (; first != last; ++first) {
        std::cout << *first << " ";
    }
    std::cout << std::endl;
}

void testListMethods() {
    std::cout << "***Testing list methods***\n";
    
    std::vector<int> v(3, 5);
    TList<int> list1(v.begin(), v.end());
    TList<int> list2 = list1;

    list1.push_back(7);
    list1.push_back(7);
    list1.push_back(2);
    
    std::cout << "Unsorted list1: ";
    print(list1.begin(), list1.end());
    list1.sort();
    std::cout << "Sorted list1: ";
    print(list1.begin(), list1.end());
    
    list1.erase(list1.begin().next());
    std::cout << "List1: deleted second elelement ";
    print(list1.begin(), list1.end());

    list1.unique();
    std::cout << "List1: unique elelements ";
    print(list1.begin(), list1.end());
    
    list1.reverse();
    std::cout << "Reversed list1: ";
    print(list1.begin(), list1.end());

    list1.insert(++list1.begin(), 4);
    std::cout << "List1: insert elelement ";
    print(list1.begin(), list1.end());

    list2.push_front(77);
    list2.push_back(11);
    list2.sort();
    std::cout << "Sorted list2: ";
    print(list2.begin(), list2.end());

    TList<int> list3;
    list3.push_back(22);
    list3.push_back(1);
    list3.push_front(10);
    list3.push_front(-2);
    list3.sort();
    std::cout << "Sorted list3: ";
    print(list3.begin(), list3.end());

    list2.merge(list3);
    std::cout << "Merged list2 and list3: ";
    print(list2.begin(), list2.end());
    
    std::cout << "**Testing list methods**\n";
}

void testListWithSTLAlgorithms() {
    std::cout << "***Testing STL algorithms on list***\n";
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(-100, 100);

    TList<int> list;
    for (int i = 0; i < 15; ++i) {
        list.push_back(distribution(generator));
    }
    std::cout << "List: ";
    print(list.begin(), list.end());

    struct Incrementor {
        void operator ()(int &i) {
            i += 5;
        }
    } incrementor;
    std::for_each(list.begin(), list.end(), incrementor);
    std::cout << "Added 5 to each list element: ";
    std::cout << "Before sorting: ";
    if (std::is_sorted(list.begin(), list.end())) {
        std::cout << "list is sorted\n";
    } else {
        std::cout << "list is not sorted";
    }

    for (int i = -50; i < 50; ++i) {
        if (std::find(list.begin(), list.end(), i) != list.end()) {
            std::cout << "List contains: " << i << std::endl;
        }
    }

    std::vector<int> toVector;
    std::copy(list.begin(), list.end(), std::back_inserter(toVector));
    std::cout << "Copy of list: ";
    print(toVector.begin(), toVector.end());

    std::cout << "Max element of list: ";
    std::cout << *std::max_element(list.begin(), list.end()) << std::endl;
    std::cout << "Min element of list: ";
    std::cout << *std::min_element(list.begin(), list.end()) << std::endl;

    std::cout << "Before sorting: ";
    print(list.begin(), list.end());
    if (std::is_sorted(list.begin(), list.end())) {
        std::cout << "List is sorted\n";
    } else {
        std::cout << "List is not sorted\n";
    }
    list.sort();
    std::cout << "After sorting: ";
    print(list.begin(), list.end());
    if (std::is_sorted(list.begin(), list.end())) {
        std::cout << "List is sorted\n";
    } else {
        std::cout << "List is not sorted\n";
    }


    std::cout << "Lower bound for 10: ";
    std::cout << *std::lower_bound(list.begin(), list.end(), 10) << std::endl;
    std::cout << "Upper bound for 10: ";
    std::cout << *std::upper_bound(list.begin(), list.end(), 10) << std::endl;

    std::cout << "**Testing STL algorithms on list**\n";
}

template <class List>
void runList(List &list, std::string listName) {
    const int iterationCount = 50000000;
    clock_t time = clock();

    for (int i = 0; i < iterationCount; ++i) {
        list.push_back(i);
    }
    for (int i = 0; i < iterationCount / 2; ++i) {
        list.pop_front();
    }
    for (int i = 0; i < iterationCount / 2; ++i) {
        list.push_front(i);
    }
    for (int i = 0; i < iterationCount; ++i) {
        list.pop_back();
    }
    std::cout << listName << ": ";
    std::cout << static_cast<double>(clock() - time) / CLOCKS_PER_SEC << "\n";
}

void testAllocators() {
    std::cout << "***Testing lists with different allocators***\n";

    TList<int, TFastAllocator<int> > tlistFast;
    TList<int> tlist;
    runList(tlistFast, "TList with TFastAllocator");
    runList(tlist, "TList with default allocator");

    std::list<int, TFastAllocator<int> > listFast;
    std::list<int> list;
    runList(listFast, "std::list with TFastAllocator");
    runList(list, "std::list with default allocator");

    std::cout << "**Testing lists with different allocators**\n";
}

int main() {
    testListMethods();
    std::cout << std::endl;
    testListWithSTLAlgorithms();
    std::cout << std::endl;
    testAllocators();
}