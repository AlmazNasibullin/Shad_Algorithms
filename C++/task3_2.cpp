#include <iostream>
#include <algorithm>
#include <assert.h>

template <class T>
class Deleter
{
public:
    virtual void free(T* pointer) = 0;
};

template <class T>
class DeleterForOneVariable: public Deleter<T>
{
public:
    void free(T* pointer)
    {
        delete pointer;
    }
};

template <class T>
class DeleterForArray: public Deleter<T>
{
public:
    void free(T* pointer)
    {
        delete [] pointer;
    }
};

template <class T>
class SharedPtr
{
public:
    explicit SharedPtr(T* pointer = 0): pointer_(pointer)
    {
        referenceCount_ = new int();
        *referenceCount_ = 1;
        deleter_ = new DeleterForOneVariable<T>(); 
    }
    SharedPtr(Deleter<T>* deleter, T* pointer = 0): deleter_(deleter), pointer_(pointer)
    {
        referenceCount_ = new int();
        *referenceCount_ = 1;
    }
    SharedPtr(SharedPtr<T>& other)
    {
        pointer_ = other.pointer_;
        referenceCount_ = other.referenceCount_;
        deleter_ = other.deleter_;
        ++(*referenceCount_);
    }
    void swap(SharedPtr<T>& other)
    {
        std::swap(pointer_, other.pointer_);
        std::swap(referenceCount_, other.referenceCount_);
        std::swap(deleter_, other.deleter_);
    }
    SharedPtr<T>& operator=(SharedPtr<T>& other)
    {
        if (pointer_ != other.pointer_) {
            if (pointer_ != nullptr) {
                if (*referenceCount_ == 1) {
                    deleter_->free(pointer_);
                    pointer_ = nullptr;
                    delete referenceCount_; 
                    delete deleter_;
                } else {
                    --*(referenceCount_);
                }
            }
            pointer_ = other.pointer_;
            referenceCount_ = other.referenceCount_;
            deleter_ = other.deleter_;
            ++*(other.referenceCount_);
        }
        return *this;
    }
    T& operator*() const
    {
        return *pointer_;
    }
    T* get() const
    {
        return pointer_;
    }
    int use_count() const
    {
        return *referenceCount_;
    }
    bool unique() const
    {
        return *referenceCount_ == 1;
    }
    operator bool() const
    {
        return get() != 0;
    }
    ~SharedPtr()
    {
        if (*referenceCount_ == 1) {
            if (pointer_ == nullptr) {
                deleter_->free(pointer_);
            }
            delete referenceCount_;
            delete deleter_;
        } else {
            pointer_ = nullptr;
            --(*referenceCount_);
        }
    }
    
private:    
    T* pointer_;
    int* referenceCount_;
    Deleter<T>* deleter_;
};

int main ()
{
    const int firstNumber = 99;
    const int secondNumber = 221;


    int* pointer = new int(firstNumber);
    SharedPtr<int> firstPtr(pointer);
    SharedPtr<int> secondPtr(firstPtr);


    assert(*firstPtr == firstNumber);
    assert(*secondPtr == firstNumber);
    assert(firstPtr.use_count() == 2);
    assert(secondPtr.use_count() == 2);


    SharedPtr<int> thirdPtr(new DeleterForOneVariable<int>, new int(secondNumber));
    firstPtr = thirdPtr;
    secondPtr = thirdPtr;


    assert(thirdPtr.use_count() == 3);
    assert(*firstPtr.get() == secondNumber);
    assert(*secondPtr == secondNumber);
    assert(*thirdPtr == secondNumber);
    assert(*pointer == 0);
    return 0;
}
