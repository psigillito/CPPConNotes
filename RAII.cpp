// RAII.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

/*resource anything that requires manual mgmt i.e.allocated mem, posix file halnde, mutex locks

raii - resource allocation is initialization

*/
//Here is a naive implementation. copy is correct and works fine. issue is when naive vector 
// goes out of scope. 
class NaiveVector {

    int* ptr_;
    size_t size_;

public: 
    NaiveVector() : ptr_(nullptr), size_(0) {}
    void push_back(int newVal)
    {
        //copy the old ptr 
        // push back the new value 
        int* new_ptr = new int[size_ + 1];
        std::copy(ptr_, ptr_ + size_, new_ptr);
        delete[] ptr_;
        ptr_ = new_ptr;
        ptr_[size_++] = newVal;
    }
};

void someFunc()
{
    NaiveVector vec;
    vec.push_back(1);
    vec.push_back(2);
    //at this point when leaving need to delete ptr. memory leak 
};

/***********************/
//Now has a destructor but still has a bug   
class NaiveVectorDestructor {

    size_t size_;

public:
    int* ptr_;
    NaiveVectorDestructor() : ptr_(nullptr), size_(0) {}
    ~NaiveVectorDestructor() { delete[] ptr_; }
    void push_back(int newVal)
    {
        //copy the old ptr 
        // push back the new value 
        int* new_ptr = new int[size_ + 1];
        std::copy(ptr_, ptr_ + size_, new_ptr);
        delete[] ptr_;
        ptr_ = new_ptr;
        ptr_[size_++] = newVal;
    }
};

void someFunc2()
{
    NaiveVectorDestructor vec;
    vec.push_back(1);
    {
        //using default copy ctor, now both pointing to the same value. 
        NaiveVectorDestructor w = vec;
    }
    //at this point w is out of scope, destructor has been called and so vecs 
    //ptr is empty. Double Delete. 
    
    //since this has been freed, now undefined behavior. 
    std::cout << vec.ptr_[0] << "\n";
    //need copt ctor if destructor to avoid double frees
};

/*******************************/
/*
Initialization is not assignment 
*/
void example() {
    NaiveVector x;
    NaiveVector w = x; //this is initialization (ctor) of new obj. Uses copy ctor. 

    NaiveVector z;
    z = x; //this is an assignment to an existing object. Uses assignment op. 
}


//Now has a destructor but still has a bug   
class NaiveVectorCopy {

    size_t size_;

public:
    int* ptr_;
    NaiveVectorCopy() : ptr_(nullptr), size_(0) {}
    ~NaiveVectorCopy() { delete[] ptr_; }
    //add copy ctor
    NaiveVectorCopy(NaiveVectorCopy& ) = default;
    //add assignment operator 
    NaiveVectorCopy& operator=(NaiveVectorCopy& rhs)
    {
        //copy swap idiom. Use copy ctor then swap pointer and size 
        //make a complete copy of rhs 
        NaiveVectorCopy copy = rhs;
        copy.swap(*this);
        return *this;
    }

    void swap(NaiveVectorCopy& rh) {};

    void push_back(int newVal)
    {
        //copy the old ptr 
        // push back the new value 
        int* new_ptr = new int[size_ + 1];
        std::copy(ptr_, ptr_ + size_, new_ptr);
        delete[] ptr_;
        ptr_ = new_ptr;
        ptr_[size_++] = newVal;
    }

};


/*Rule of Three */
/*

If class directly manages resource i.e. new'd pointer. 
probs need 3 explicity member functions; 
destructor - free resource. 
copy ctor - copy resource
copy assignment operator - free left hand resource and copy right hand one 
*/

/*
All of cleanup code should go in destructor 
i.e. throw catch does stack unwinding, does not allow fuinctions to complete
just keeps destructing up the stack. 

jumps over the code into catch -- could step over the resource 

simply put: cleanup goes in destructor 

*/

struct RAIIPtr {
    int* ptr_;
    RAIIPtr(int* p) : ptr_(p) {};
    ~RAIIPtr() { delete[]ptr_; };
    //disallow copy ctor or assignment at compile time. 
    RAIIPtr(const RAIIPtr&) = delete;
    RAIIPtr& operator=(const RAIIPtr&) = delete;
};


/*Rule of Zero*/
/*

if does not manage any resources i.e. uses string and vector
strive to write no special memeber functions 
default them all. 
compuiler generate destructor, copy ctor, copy assignment operator
*/


/*Rule of 5 for correctness and performance on top of rule of 3
* include move constructor -- transfer ownership
* move assignment operator

we can steal what is in rhs because nobody cares our is looking at it 
naiveVector(NaiveVector&& rhs)
{
ptr_ = tsd::exchange(rhs.ptr_, nullptr);
size_ = std::exchange(rhs.size_, 0);
}

stl template types have move ctor included 
*/



int main()
{


    std::cout << "Hello World!\n";
}
