#include <iostream>
#include <algorithm>

template <typename T>
struct TNode {
    T value_;
    TNode *prev_;
    TNode *next_;

    TNode(const T &value) : value_(value), prev_(nullptr), next_(nullptr) {}

    TNode(const T &value, TNode<T> *prev, TNode<T> *next) : value_(value),
        prev_(prev), next_(next) {}
    
    TNode(const TNode<T> &node) : value_(node.value_), prev_(node.prev_),
        next_(node.next_) {}
    
    TNode& operator =(const TNode<T> &node) {
        if (*this != node) {
            value_ = node.value_;
            prev_ = node.value_;
            next_ = node.next_;
        }
        return *this;
    }
};

template <class T, class Allocator = std::allocator<T> >
class TList {
private:
    typedef typename Allocator::template rebind<TNode<T> >::other TNodeAllocator;
    typedef typename Allocator::template rebind<T>::other TAllocator;
    
    TNodeAllocator nodeAllocator_;
    TAllocator allocator_;
    
    TNode<T> *begin_;
    TNode<T> *end_;
    size_t size_;

private:    
    TNode<T>* createNode(const T &value) {
        TNode<T> *node = nodeAllocator_.allocate(1);
        try {
            allocator_.construct(&node->value_, value);
        } catch (...) {
            nodeAllocator_.deallocate(node, 1);
            __throw_exception_again;
        }
        return node;
    }

    TNode<T>* createNode() {
        TNode<T> *node = nodeAllocator_.allocate(1);
        try {
            allocator_.construct(&node->value_);
        } catch(...) {
            nodeAllocator_.deallocate(node, 1);
            __throw_exception_again;
        }
        return node;
    }

private:
    template <class U, class V>
    void construct(U u, V v, std::true_type) {
        for (U i = 0; i < u; ++i) {
            push_back(v);
        }
    }
    
    template <class U, class V>
    void construct(U u, V v, std::false_type) {
        for (; u != v; ++u) {
            push_back(*u);
        }
    }

public:
    class TListIterator {
    private:
        TNode<T> *node_;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef std::ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T& reference;

        TListIterator() : node_(nullptr) {}
        
        TListIterator(TNode<T> *node) : node_(node) {}
        
        TListIterator& operator ++() {
            node_ = node_->next_;
            return *this;
        }
        
        TListIterator operator ++(int) {
            TListIterator itr = *this;
            node_ = node_->next_;
            return itr;
        }
        
        TListIterator& operator --() {
            node_ = node_->prev_;
            return *this;
        }
        
        TListIterator operator --(int) {
            TListIterator itr = *this;
            node_ = node_->prev_;
            return itr;
        }
        
        T& operator *() {
            return node_->value_;
        }
        
        bool operator ==(const TListIterator &itr) {
            return node_ == itr.node_;
        }
        
        bool operator !=(const TListIterator &itr) {
            return node_ != itr.node_;
        }
        
        TListIterator& operator =(const TListIterator &itr) {
            if (*this != itr) {
                node_ = itr.node_;
            }
            return *this;
        }

        TListIterator next() {
            TListIterator itr = *this;
            ++itr;
            return itr;
        }

        TListIterator prev() {
            TListIterator itr = *this;
            ++itr;
            return itr;
        }

        TNode<T>* getNode() {
            return node_;
        }
    };

    class TConstListIterator {
    private:
        TNode<T> *node_;

    public:

        TConstListIterator() : node_(nullptr) {}
        
        TConstListIterator(TNode<T> *node) : node_(node) {}
        
        TConstListIterator& operator ++(){
            node_ = node_->next_;
            return *this;
        }
        
        TConstListIterator operator ++(int){
            TConstListIterator itr = *this;
            node_ = node_->next_;
            return itr;
        }
        
        TConstListIterator& operator --(){
            node_ = node_->prev_;
            return *this;
        }
        
        TConstListIterator operator --(int){
            TConstListIterator itr = *this;
            node_ = node_->prev_;
            return itr;
        }

        const T& operator *(){
            return node_->value_;
        }
        
        bool operator ==(const TConstListIterator &itr) {
            return node_ == itr.node_;
        }
        
        bool operator !=(const TConstListIterator &itr) {
            return node_ != itr.node_;
        }
        
        TConstListIterator& operator =(TConstListIterator &itr) {
            if (*this != itr) {
                node_ = itr.node_;
            }
            return *this;
        }
    };

public:
    typedef T value_type;
    typedef Allocator allocator_type;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef TConstListIterator const_iterator;
    typedef TListIterator iterator;
    
    TList() {
        begin_ = nullptr;
        end_ = nullptr;
        size_ = 0;
    }

    template <class U, class V>
    TList(U u, V v) {
        begin_ = nullptr;
        end_ = nullptr;
        size_ = 0;
        construct(u, v, std::is_integral<U>());
    }

    TList(const TList<T, Allocator> &list) {
        begin_ = nullptr;
        end_ = nullptr;
        size_ = 0;
        for (TConstListIterator it = list.begin(); it != list.end(); ++it) {
            push_back(*it);
        }
    }

    ~TList() noexcept {
        clear();
    }
    
    bool operator ==(const TList<T, Allocator> &list) const {
        if (size_ != list.size()) {
            return false;
        }
        TConstListIterator first = begin();
        TConstListIterator second = list.begin();
        for (; first != end(); ++first, ++second) {
            if (*first != *second) {
                return false;
            }
        }
        return true;
    }
    
    bool operator !=(const TList<T, Allocator> &list) const {
        return !(*this == list);
    }
    
    TList<T, Allocator>& operator =(const TList<T, Allocator> &list) {
        if (list != *this) {
            TConstListIterator it = list.begin();
            for (; it != list.end(); ++it) {
                push_back(*it);
            }
        }
        return *this;
    }
    
    TConstListIterator begin() const noexcept {
        return TConstListIterator(begin_);
    }
    
    TConstListIterator end() const noexcept {
        return TConstListIterator(nullptr);
    }
    
    TListIterator begin() noexcept {
        return TListIterator(begin_);
    }
    
    TListIterator end() noexcept {
        return TListIterator(nullptr);
    }
    
    bool empty() const noexcept {
        return size_ == 0;
    }
    
    size_t size() const noexcept {
        return size_;
    }
    
    T& front() {
        return begin_->value_;
    }
    
    T& back() {
        return end_->value_;
    }
    
    const T& front() const {
        return begin_->value_;
    }
    
    const T& back() const {
        return end_->value_;
    }
    
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last) {
        clear();
        insert(begin_, first, last);
    }
    
    void push_front(const T &value) {
        TNode<T> *temp = createNode(value);
        temp->prev_ = nullptr;
        temp->next_ = begin_;
        if (size_ == 0) {
            begin_ = temp;
            end_ = temp;
        } else {
            begin_->prev_ = temp;
            begin_ = temp;
        }
        ++size_;
    }
    
    void pop_front() noexcept {
        if (size_ == 1) {
            nodeAllocator_.destroy(begin_);
            nodeAllocator_.deallocate(begin_, 1);
            begin_ = nullptr;
            end_ = nullptr;
        } else {
            begin_ = begin_->next_;
            nodeAllocator_.destroy(begin_->prev_);
            nodeAllocator_.deallocate(begin_->prev_, 1);
            begin_->prev_ = nullptr;
        }
        --size_;
    }
    
    void push_back(const T &value) {
        TNode<T> *temp = createNode(value);
        temp->prev_ = end_;
        temp->next_ = nullptr;
        if (size_ == 0) {
            begin_ = temp;
            end_ = temp;
        } else {
            end_->next_ = temp;
            end_ = temp;
        }
        ++size_;
    }
    
    void pop_back() noexcept {
        if (size_ == 1) {
            nodeAllocator_.destroy(begin_);
            nodeAllocator_.deallocate(begin_, 1);
            begin_ = nullptr;
            end_ = nullptr;
        } else {
            end_ = end_->prev_;
            nodeAllocator_.destroy(end_->next_);
            nodeAllocator_.deallocate(end_->next_, 1);
            end_->next_ = nullptr;
        }
        --size_;
    }
    
    TListIterator insert(TListIterator position, const T &value) {
       if (position == begin()) {
            push_front(value);
            return begin();
       }
       if (position == end()) {
            push_back(value);
            return TListIterator(end_);
       }
       TNode<T> *curNode = position.getNode();
       TNode<T> *prevNode = curNode->prev_;
       TNode<T>* newNode = createNode(value);
       newNode->prev_ = prevNode;
       newNode->next_ = curNode;
       prevNode->next_ = newNode;
       curNode->prev_ = newNode;
       ++size_;
       return TListIterator(newNode);
    }

    template <class InputIterator>
    TListIterator insert(TListIterator position, InputIterator first,
            InputIterator last) {
        TListIterator firstInserted = insert(position, *first);
        ++first;
        for (; first != last; ++first) {
            insert(position, *first);
        }
        return firstInserted;
    }
    
    TListIterator erase(TListIterator position) noexcept {
        if (position == begin()) {
            pop_front();
            return begin();
        }
        if (position.next() == end()) {
            pop_back();
            return end();
        }
        TNode<T> *curNode = position.getNode();
        TNode<T> *prevNode = curNode->prev_;
        TNode<T> *nextNode = curNode->next_;
        prevNode->next_ = nextNode;
        nextNode->prev_ = prevNode;
        nodeAllocator_.destroy(curNode);
        nodeAllocator_.deallocate(curNode, 1);
        --size_;
        return TListIterator(nextNode);
    }
    
    TListIterator erase(TListIterator first, TListIterator last) noexcept {
        for (; first != last; ++first) {
            first = erase(first);
        }
        return last;
    }
    
    void swap(TList<T, Allocator> &list) {
        std::swap(begin_, list.begin_);
        std::swap(end_, list.end_);
        std::swap(size_, list.size_);
    }
    
    void resize(size_t n, const T &value) {
        size_t counter = size_;
        if (n <= size_) {
            size_t counter = 0;
            for (; counter < size - n; ++counter) {
                pop_back();
            }
        } else {
            size_t counter = 0;
            for (; counter < n - size; ++counter) {
                push_back(value);
            }
        }
    }
    
    void clear() noexcept {
        while (size_ > 0) {
            pop_back();
        }
    }
    
    void splice(TConstListIterator position, TList<T, Allocator> &list) {
        size_t counter = list.size();
        for (; counter > 0; --counter) {
            insert(position, list.front());
            list.pop_front();
        }
    }
    
    void reverse() noexcept {
        TNode<T> *curNode = begin_;
        while (curNode != nullptr) {
            std::swap(curNode->prev_, curNode->next_);
            curNode = curNode->prev_;
        }
        std::swap(begin_, end_);
    }
    
    void sort() {
        if (size_ < 2) {
            return;
        }
        TListIterator last(end_);
        for (; begin() != last; --last) {
            for (TListIterator first = begin(); first != last; ++first) {
                if (*first.next() < *first) {
                    std::iter_swap(first.next(), first);
                }
            }
        }
    }
    
    void merge(TList<T, Allocator> &list) {
        if (&list == this) {
            return;
        }
        TListIterator firstIter = begin();
        TListIterator secondIter = list.begin();
        while (firstIter != end() && secondIter != list.end()) {
            if (*firstIter < *secondIter) {
                ++firstIter;
            } else {
                insert(firstIter, *secondIter);
                list.erase(secondIter);
                secondIter = list.begin();
            }
        }
        while (secondIter != end()) {
            insert(firstIter, *secondIter);
            list.erase(secondIter);
            secondIter = list.begin();
        }
    }
    
    void unique() {
        if (size_ < 2) {
            return;
        }
        TListIterator first = begin();
        TListIterator next = first.next();
        while (next != end()) {
            if (*next == *first) {
                next = erase(next);
            } else {
                first = next++;
            }
        }
    }
};