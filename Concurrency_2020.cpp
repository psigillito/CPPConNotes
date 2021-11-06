#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <shared_mutex>
#include <latch>
/*
Concurrency Presentation CPP Con 2020

Hardware can reorder access 
Could store values in differnt caches
C++11 memory model 
    all threads must sych when writing with all other r/w to that var else UN 
*/
void concurrency_basics()
{
    /* c++11 STD::THREAD*/
    //pass function or lambda into thread. Starts immediately 
    std::thread my_thread = std::thread([]() {puts("hello from thread"); });

    puts("Hello from main thread");
    //main thread says it will stop until thread is done with work and ready to join back 
    my_thread.join();

    //SYNCH USING JOIN
    int result{ 0 };
    std::thread threadB = std::thread([&result]() 
        { ++result; printf("Thread result now: %d\n", result); });
    
    //join is the synching before doing a read so main thread gets 1. 
    threadB.join();
    printf("After join result now: %d\n", result);

    //ATOMIC
    //std::atomic<> fixes the PHYSICAL data race. you can still have semantic data races but no UB
    using SC = std::chrono::steady_clock;
    auto deadline = SC::now() + std::chrono::seconds(1);
    std::atomic<int> atomic_counter{ 0 };

    std::thread atomicA = std::thread([&atomic_counter, &deadline]() {
        while (SC::now() < deadline)
        {
            printf("A good %d\n", ++atomic_counter);
        };
        });

   while (SC::now() < deadline)
   {
    printf("B good %d\n", ++atomic_counter);
   };
   atomicA.join();
   
}


//LOGICAL SYCH PRIMATIVES
/*
* a 'busy wait' while loop is a bad solution i.e. while(!ready)
* is, still doing something that wastes resources by checking over and over
* compiler could optimize the while check to only check once, uh oh
*
* std::mutex (mutual exclusion) -- basic mutex behavior, holder gets access
* mtx.lock(), mtx.unlock()
* will wait on .lock()
* used often as a lock on data
* before accessing i.e. get_token() or numTokens()
*
* STD::LOCK_GUARD<STD::MUTEX> lk(muutex_name
* What if the thread that locked the mutex then throws an exception and does not
* execute the unlock()? To avoid issue ue RAII principal.
* Instead of mtx.lock, create a lock_guard class that will unlock during destruction.
* If error thrown, destruction. If exit function, destruction. *Added benefit of not calling .unlock()
* lock_guard cannot be returned
* For that use unique_lock<mutex>, can have a function that returns onwership of a mutex lock
* scoped_lock<...> can take multiple locks and will not result in a dead lock
*/

std::mutex mtx;
struct Token
{
    int value{ 0 };
};
std::vector<Token> tokens{};

//get token that could deadlock mutex if exception thrown before unlock 
Token getToken()
{
    mtx.lock();
    if (tokens.empty())
        tokens.push_back(Token());
    Token t = std::move(tokens.back());
    tokens.pop_back();
    mtx.unlock();
    return t;
}

//RAII to avoid the deadlock exception. 
Token fixedGetToken()
{
    std::lock_guard<std::mutex> lk(mtx);
    if (tokens.empty())
        tokens.push_back(Token());
    Token t = std::move(tokens.back());
    tokens.pop_back();
    return t;
}

//returning a lock from a function example 
std::unique_lock<std::mutex> foo(std::unique_lock<std::mutex> lk)
{
    lk.unlock();
    return lk;
}


//condition_variable -- wait until i.e. limited number of tokens, wait until 1 is available
struct TokenPool
{
    std::vector<Token> tokens;
    std::mutex mtx;
    std::condition_variable cv;

    void returnToken(Token t)
    {
        std::unique_lock<std::mutex> lk(mtx);
        tokens.push_back(t);
        lk.unlock();

        //will notify 1 blocked thread of availability
        cv.notify_one();
        //will notify all of available 
        cv.notify_all();
    }

    Token getToken()
    {
        std::unique_lock<std::mutex> lk(mtx);
        while (tokens.empty())
        {
            cv.wait(lk);
        }
        Token t = std::move(tokens.back());
        tokens.pop_back();
        return t;
    }
};

//  PRODUCER/CONSUMER you are going to want to use a mutex + condition_variable 

// If multi threads wanting to initialize a static data member, can put a mutex and then check
// if already created but could be faster 
// primitice ONCE_FLAG -- first successfull thread will set flag to true, signaling other thread
// to not even try. If fails, other threads will be queued behind to try

struct logger {
    std::once_flag once;
    void example()
    {
        std::call_once(once, []() {puts("I get called successfully once"); });
    }
};

//C++ 17 reader writer lock -- called shared mutex
//if many threads are just reading, can all read at the same time then finally 1 thread
//can do writing. 

std::shared_mutex read_write_mutex;
//unique lock for writing, no other holders allowed 
std::unique_lock<std::shared_mutex> ulk(read_write_mutex);
//shared lock for reading, multiple readers 
//std::shared_lock<std::shared_mutex> slk(read_write_mutex);
 

//c++20 do std::latch, make all threads reach gate until all get there --used once 
// std::barrier<> resetable latch, begins new phase immediately i.e. if looping, each loop around
// will hit barrier 

//use atomic shared pointers for blue/green changes. i.e. have different threads use 
// a large expensive config file all at the same time 

//ASIO, TBB, Coroutines

int main()
{
    concurrency_basics();
    //auto token = getToken();
}

