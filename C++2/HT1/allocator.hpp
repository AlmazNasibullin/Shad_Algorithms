#include <vector>

template <size_t ChunkSize>
class TFixedAllocator {
private:
    struct TNode {
        char data[ChunkSize];
        TNode *next;
    };

    TNode *free;
    std::vector<TNode*> pools;
    int size;
    const int MAX_SIZE = 1024;

    void newPool() {
        if (size < MAX_SIZE) {
            size *= 2;
        }
        free = new TNode[size];
        pools.push_back(free);
        for (int i = 0; i + 1 < size; ++i) {
            free[i].next = &free[i + 1];
        }
        free[size - 1].next = nullptr;
    }
    TFixedAllocator() {
        size = 1;
        newPool();
    }
    TFixedAllocator(const TFixedAllocator&);
public:
    static TFixedAllocator& instance() {
        static TFixedAllocator instance;
        return instance;
    }
    void* allocate() {
        if (!free) {
            newPool();
        }
        TNode *result = free;
        free = free->next;
        return static_cast<void*>(result);
    }
    void deallocate(void *elem) {
        TNode *node = static_cast<TNode*>(elem);
        node->next = free;
        free = node;
    }
    ~TFixedAllocator() {
        for (auto ptr : pools) {
            delete ptr;
        }
    }
};

template <class T>
class TFastAllocator {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;

    template <class Type>
    struct rebind {
        typedef TFastAllocator<Type> other;
    };

    TFastAllocator() noexcept {}

    T* allocate(size_t n) throw(std::bad_alloc) {
        if (n == 1) {
            return static_cast<T*>(TFixedAllocator<sizeof(T)>::instance()
                .allocate());
        } else if (n <= 16) {
            return static_cast<T*>(TFixedAllocator<16 * sizeof(T)>::instance()
                .allocate());
        } else if (n <= 64) {
            return static_cast<T*>(TFixedAllocator<64 * sizeof(T)>::instance()
                .allocate());
        } else if (n <= 256) {
            return static_cast<T*>(TFixedAllocator<256 * sizeof(T)>::instance()
                .allocate());
        } else {
            return static_cast<T*>(operator new(n * sizeof(T)));
        }
    }

    void deallocate(T *p, size_t n) {
        if (n == 1) {
            TFixedAllocator<sizeof(T)>::instance()
                .deallocate(static_cast<void*>(p));
        } else if (n <= 16) {
            TFixedAllocator<16 * sizeof(T)>::instance()
                .deallocate(static_cast<void*>(p));
        } else if (n <= 64) {
            TFixedAllocator<64 * sizeof(T)>::instance()
                .deallocate(static_cast<void*>(p));
        } else if (n <= 256) {
            TFixedAllocator<256 * sizeof(T)>::instance()
                .deallocate(static_cast<void*>(p));
        } else {
            operator delete(p);
        }
    }

    void construct(T *p, const T &t) {
        new((T*)p) T(t);
    }

    void destroy(pointer p) {
        p->~T();
    }
};
