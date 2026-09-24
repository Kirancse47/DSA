# C++ Multithreading — Q&A

> Source: `Kirancse47/DSA` -> `Threading/` — 25 files (real count)

Ordering: Chronological progression from basics (thread creation, lifecycle) to primitives (mutexes, CVs), ending with design patterns (thread pools) and classic concurrency problems.

## Cheat sheet
- **Compile flag:** Link with `-pthread` (e.g., `g++ -std=c++11 -pthread`).
- **`std::thread` creation:** Create using function pointers, lambdas, functors, member functions, or static members (see `1.multiThreading.cpp`).
- **`join` vs `detach`:** `join` blocks until completion; `detach` runs in background. Destroying a joinable thread terminates the program (`2.joinDetach.cpp`).
- **`std::mutex`:** Exclusive lock primitive for critical sections (`3.mutex.cpp`, `4.tryLock.cpp`, `5.tryLockMultiMutex.cpp`).
- **`std::recursive_mutex`:** Allows the same thread to lock repeatedly (e.g., in recursion), needs matching unlocks (`7.recursiveMutex.cpp`).
- **`std::lock_guard`:** Lightweight RAII wrapper for a mutex; locks on construction, unlocks on destruction (`8.lockGuard.cpp`).
- **`std::unique_lock`:** Flexible RAII wrapper supporting deferred locking, timed locks, and manual unlock (`9.uniqueLock.cpp`).
- **`std::scoped_lock` / `std::shared_lock`:** C++17 multiple-lock / C++14 read-write locks (use `std::lock` for multiple mutexes in C++11, see `13lockMultiMutex.cpp`).
- **`std::condition_variable`:** Blocks a thread until notified. Always use the predicate overload to avoid spurious wakeups (`10.conditionVariable.cpp`).
- **Producer-Consumer:** Classic problem solved via mutex+CV (`16.producerConsumer.cpp`) or C++20 `std::binary_semaphore` (`19.producerConsumerUsingSemaphore.cpp`).
- **`std::atomic` & memory orders:** Lock-free primitives for trivial types; prevents races on shared variables (alternative to mutex in `20.staticVariableInThreading.cpp`).
- **Futures & Promises:** `std::promise` sets a value/exception; `std::future` retrieves it (`14.promiseFuture.cpp`).
- **`std::async` & `std::packaged_task`:** Runs a function asynchronously and returns a `std::future` (`15.async.cpp`).
- **Deadlock:** Occurs when threads wait circularly (`11.deadLock.cpp`). Break by ordering locks uniformly (e.g., using `std::lock`).
- **`std::call_once`:** Ensures a callable is executed exactly once, safely, across multiple threads.
- **Thread Pools:** Reuses worker threads with a task queue to avoid thread creation overhead (`22.threadPool.cpp`).

---

## 1. 1.multiThreading.cpp

**Q:** How do you launch concurrent threads to perform parallel background computations?

**A:** `std::thread` objects initialized with callable targets.

**Explanation:**
- Creates two threads targeting independent functions (`findEven`, `findOdd`).
- Shared state is safely bypassed here by writing to separate global variables, avoiding race conditions.
- Demonstrates `join()` blocking the main thread until background threads complete.

**Complexity / cost:** `O(N)` time complexity per thread, no blocking synchronization overhead.

```cpp
/*
TOPIC: Introduction to thread in c++ (c++11)

QUESTIONS
1. What do you understand by thread and give one example in C++?

ANSWER
0. In every application there is a default thread which is main(), in side this we create other threads.
1. A thread is also known as lightweight process. Idea is achieve parallelism by dividing a process into multiple threads. 
   For example:
   (a) The browser has multiple tabs that can be different threads. 
   (b) MS Word must be using multiple threads, one thread to format the text, another thread to process inputs (spell checker)
   (c) Visual Studio code editor would be using threading for auto completing the code. (Intellicence)

WAYS TO CREATE THREADS IN C++11
1. Function Pointers
2. Lambda Functions
3. Functors
4. Member Functions
5. Static Member functions

to run : g++ -std=c++11 -pthread multiThreading.cpp
*/

#include<iostream>
#include<thread>
using namespace std;
typedef unsigned long long ull;

ull oddSum=0;
ull evenSum=0;

void findEven(ull start,ull end){
    for(ull i=start;i<=end;i++){
        if(!(i&1)) evenSum+=i;
    }
}

void findOdd(ull start,ull end){
    for(ull i=start;i<=end;i++){
        if((i&1)) oddSum+=i;
    }
}

int main(){
    ull start=0, end=1900000;
    thread t1(findEven,start,end);
    thread t2(findOdd,start,end);
    t1.join();
    t2.join();
    cout<<"OddSum : "<<oddSum<<endl;
    cout<<"EvenSum : "<<evenSum<<endl;
    return 0;
}
```

---

## 2. 2.joinDetach.cpp

**Q:** How do you manage a thread's execution lifecycle and prevent program termination upon object destruction?

**A:** `std::thread::join()` or `std::thread::detach()`.

**Explanation:**
- Shows waiting for thread completion (`join()`) versus abandoning the thread to run independently (`detach()`).
- Prevents `std::terminate` by ensuring threads are either joined or detached before they fall out of scope.
- Uses `joinable()` to safely check state before lifecycle actions.

**Complexity / cost:** `join()` blocks the calling thread until completion; `detach()` is non-blocking (fire-and-forget).

```cpp
/*
In this video we will learn about what is the Use Of join(), detach() and joinable() In Thread In C++ (C++11)

JOIN NOTES
0. Once a thread is started we wait for this thread to finish by calling join() function on thread object.
1. Double join will result into program termination.
2. If needed we should check thread is joinable before joining. ( using joinable() function)

DETACH NOTES
0. This is used to detach newly created thread from the parent thread.
1. Always check before detaching a thread that it is joinable otherwise we may end up double detaching and 
   double detach() will result into program termination.
2. If we have detached thread and main function is returning then the detached thread execution is suspended.

NOTES:
Either join() or detach() should be called on thread object, otherwise during thread objects destructor it will 
terminate the program. Because inside destructor it checks if thread is still joinable? if yes then it terminates the program.
*/

#include<iostream>
#include<thread>
#include<chrono>
using namespace std;

void run(int count){
    while(count-- > 0) cout<<"CppNuts"<<endl;
    // this_thread::sleep_for(chrono::seconds(5));
}

int main(){
    thread t1(run, 10);
    cout<<"main()"<<endl;

    //t1.join();
    //if(t1.joinable()) t1.join();
    t1.detach();

    cout<<"main() after"<<endl;
    this_thread::sleep_for(chrono::seconds(5));
    return 0;
}
```

---

## 3. 3.mutex.cpp

**Q:** How do you prevent race conditions when multiple threads mutate a shared data structure?

**A:** `std::mutex` with manual `lock()` and `unlock()`.

**Explanation:**
- The shared state is a global integer `myMoney`.
- Uses a basic `std::mutex` to protect the critical section (the increment operation).
- Eliminates the data race that would occur if both threads read and wrote simultaneously.

**Complexity / cost:** High contention overhead on heavy access; thread blocks fully while mutex is locked.

```cpp
/*
Topic : Mutex In C++ Threading

1. Race Condition : 
    a) Race condition is a situation where two or more threads/process happend to change a common data at the same time.
    b) If threre is a race condition then we have to protect it and the protected section is called critical section/region.
2. Mutex : 
    a) Mutex is used to avoid race condition.
    b) We use lock(), unlock() on mutex to avoid race condition.
*/

#include<iostream>
#include<thread>
#include<mutex>
using namespace std;
mutex m;
int myMoney=0;

void inMoney(){
    m.lock();
    myMoney++;
    m.unlock();
}

int main(){
    thread t1(inMoney);
    thread t2(inMoney);

    t1.join();
    t2.join();
    cout<<"myMoney = "<<myMoney<<endl;
    return 0;
}
```

---

## 4. 4.tryLock.cpp

**Q:** How can a thread attempt to acquire a lock without blocking indefinitely if it is already held?

**A:** `std::mutex::try_lock()`.

**Explanation:**
- The critical section is protected but the threads simply bypass the operation if `try_lock()` fails.
- Avoids thread sleeping/blocking overhead at the cost of potentially skipping updates.
- Highlights that `try_lock()` on an already-owned standard mutex yields undefined behavior.

**Complexity / cost:** Non-blocking check; fails fast, reducing context-switch overhead but may cause starvation.

```cpp
/*Topic : mutex:try_lock() On Mutex In C++ Threading.
0. try_lock() Tries to lock the mutex. Returns immediatrly. On successful lock acquisitions returns true otherwise returns false.
1. If try_lock() is not able to lock mutex, then it doesn't get blocked that's why it is called non-blocking.
2. If try_lock is called again by the same thread which owns the mutex, the behavior is undefined. It is a dead lock situation with undefined behaviour.
   (if you want to be able to lock the same mutex by same thread more than one time the go for recursive_mutex).
*/

#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

mutex m;

int kiran=0;

void fun(){
    for(int i=0;i<1000;i++){
        if(m.try_lock()){
            kiran++;
            m.unlock();
        }
    }
}

int main(){
    thread t1(fun);
    thread t2(fun);
    t1.join();
    t2.join();
    cout<<"Final cout = "<<kiran<<endl;
    return 0;
}
```

---

## 5. 5.tryLockMultiMutex.cpp

**Q:** How do you attempt to lock multiple mutexes simultaneously without risking a deadlock?

**A:** `std::try_lock()` (variadic).

**Explanation:**
- Consumer thread tries to lock `m1` and `m2` in one non-blocking call.
- If it fails, it releases any partially acquired locks, avoiding circular wait deadlocks.
- The shared state involves two independent variables (`X` and `Y`) protected by separate mutexes.

**Complexity / cost:** Non-blocking multiple acquisition; safe from deadlock but requires polling (busy-wait).

```cpp
/*

Topic : Lock Multiple Thread in single cmd.
1. try_lock() tries to lock all the lockable objects passed in it one by one in given order.
    Syntax : try_lock(m1,m2,m3,m4...mn)

2. on success this function return -1 otherwise it will return 0-based mutex index number which it could not lock.
3. if it fails to lock any of the mutex then it will release all the mutex it locked before.
4. if a call to try_lock results in an exception,unlock is called for any locked objects before rethrowing.
To run this code : g++ -std=c++11 -pthread tryLockMultiMutex.cpp
*/

#include<iostream>
#include<thread>
#include<mutex>
#include<chrono>
using namespace std;

int X=0,Y=0;
mutex m1,m2;

void doSomeWorkFor1Sec(int seconds){
    this_thread::sleep_for(chrono::seconds(seconds));
}

void incrementXY(int &XorY, mutex &m, const char* desc){
    for(int i=0;i<5;i++){
        m.lock();
        XorY++;
        cout<<desc<<XorY<<endl;
        m.unlock();
        doSomeWorkFor1Sec(1);
    }
}

void consumeXY(){
    int useCount=5;
    int XplusY=0;
    while(1){
        int lockResult=try_lock(m1,m2);
        if(lockResult==-1){
            if(X!=0 && Y!=0){
                useCount--;
                XplusY+=X+Y;
                X=0;
                Y=0;
                cout<<"XplusY "<<XplusY<<endl;
            }
            m1.unlock();
            m2.unlock();
            if(useCount==0) break;
        }
    }
}

int main(){
    thread t1(incrementXY, ref(X), ref(m1), "X ");
    thread t2(incrementXY, ref(Y), ref(m2), "Y ");
    thread t3(consumeXY);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}
```

---

## 6. 6.timedMutex.cpp

**Q:** How do you attempt to acquire a lock with a strict timeout to prevent indefinite waiting?

**A:** `std::timed_mutex` with `try_lock_for()` or `try_lock_until()`.

**Explanation:**
- The critical section is guarded by a timed mutex.
- Threads wait up to a specific duration; if the lock isn't acquired, they handle the failure gracefully.
- Avoids deadlocks or infinite stalls when resources are held for too long by other threads.

**Complexity / cost:** Blocking bounded by timeout; higher overhead than standard mutex due to clock checks.

```cpp
/*
Topic : Timed Mutex In C++ Threading.

0. time_mutex is blocked till timeout_time ot the lock is aquired and returns ture if success otherwise false.
1. Member Function :
   a. lock
   b. try_lock
   c. try_lock_for    ----\  These two functions makes it different from mutex.
   d. try_lock_until  ----/
   e. unlock

   To run : g++ -std=c++11 -pthread timeMutex.cpp
*/

#include<iostream>
#include<thread>
#include<mutex>
#include<chrono>
using namespace std;

int amt=0;
timed_mutex tm;


/*
// Example of try_lock_for() :
//   Waits until specified timeout duration has elapsed or the lock is acuqired, whichever comes first.
//   On successful lock acquisition returns true, otherwise returns false.

void increment(int i){
    if(tm.try_lock_for(chrono::seconds(1))){
        amt++;
        this_thread::sleep_for(chrono::seconds(2));
        cout<<"Thread "<<i<<" Entered"<<endl;
        tm.unlock();
    }else{
        cout<<"Thread "<<i<<" Couldn't Enter"<<endl;
    }
}
*/


// Example of try_lock_until() :
//   Waits until specified timeout duration has elapsed or the lock is acuqired, whichever comes first.
//   On successful lock acquisition returns true, otherwise returns false.

void increment(int i){
    auto now=chrono::steady_clock::now();
    if(tm.try_lock_until(now + chrono::seconds(1))){
        amt++;
        this_thread::sleep_for(chrono::seconds(2));
        cout<<"Thread "<<i<<" Entered"<<endl;
        tm.unlock();
    }else{
        cout<<"Thread "<<i<<" Couldn't Enter"<<endl;
    }
}


int main(){
    thread t1(increment,1);
    thread t2(increment,2);

    t1.join();
    t2.join();

    cout<<amt<<endl;
    return 0;
}
```

---

## 7. 7.recursiveMutex.cpp

**Q:** How do you allow a single thread to acquire the same lock multiple times, such as in recursive functions?

**A:** `std::recursive_mutex`.

**Explanation:**
- The recursive function locks the mutex on every call stack level.
- The shared state `buffer` is safely mutated without self-deadlocking the calling thread.
- Emphasizes that every `lock()` call must be matched by a corresponding `unlock()`.

**Complexity / cost:** Higher system overhead than `std::mutex`; requires tracking lock ownership count.

```cpp
/*
Topic : Recusive Mutex in C++

0. It is same as mutex but, same thread can lock one mutex multiple time using recusive_mutex.
1. If thread T1 first call lock/try_lock on recursive mutex m1, then m1 is locked by T1, now as T2 is running in recursion T1 can call lock/try_lock 
    any number of times there is no issue.
2. But if T1 have aquired 10 times lock/try_lock on mutex m1 then thread T1 will have to unlock it 10 times otherwise no other thread will be able to lock mutex m1.
3. How many time we can lock recursive_mutex is not defined but when that number reachesand if we were calling lock(), it will return std::system_error OR
    if we were calling try_lcok() then it will return false.

Bottom Line:
0. It is similar to mutex but hacve extrra facitility that it can be locked multiple time by same thread.
1. If we can avoid recursive_mutex then we should because it btings overhead to the system.
2. It can be used in loops also.

*/

#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

int buffer=0;
recursive_mutex rm;

//Example with Recursion
void recursion(char c,int loopFor){
    if(loopFor<0) return;
    rm.lock();
    cout<<"Thread"<<c<<" "<<buffer++<<endl;
    recursion(c,--loopFor);
    rm.unlock();
    cout<<"Unlockd by thread"<<c<<endl;
}

int main(){
    thread t1(recursion,'1',10);
    thread t2(recursion,'2',10);
    t1.join();
    t2.join();
    return 0;
}

/*
//Example with loop
int main(){
    for(int i=0;i<5;i++){
        rm.lock();
        cout<<"locked "<<i<<endl;
    }
    for(int i=0;i<5;i++){
        rm.unlock();
        cout<<"Unlocked "<<i<<endl;
    }
}
*/
```

---

## 8. 8.lockGuard.cpp

**Q:** How do you guarantee a mutex is released when exiting a scope, even if an exception is thrown?

**A:** `std::lock_guard`.

**Explanation:**
- Uses RAII to bind the lock acquisition to object initialization and release to destruction.
- The shared state `buffer` is protected inside the loop.
- Prevents accidental deadlocks caused by forgetting to call `unlock()`.

**Complexity / cost:** Lightweight RAII wrapper, zero-cost abstraction over manual lock/unlock.

```cpp
/*
Topic : lock_guard in C++

1. It is very light weight weapper for owning mutex on scoped basis.
1. It aquires mutex lock the moment you create the object of lock_guard.
2. It automatically removes the lcok while goes out of scope.
3. You can not explicitily unlock the lock_guard.
4. You can not copy lock_guard.
*/

#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

mutex m1;
int buffer=0;

void task(const char* c,int loopFor){
    lock_guard<mutex> lock(m1);
    for(int i=0;i<loopFor;i++){
        buffer++;
        cout<<c<<buffer<<endl;
    }
}

int main(){
    thread t1(task,"T1 ",10);
    thread t2(task,"T2 ",10);
    t1.join();
    t2.join();
    return 0;
}
```

---

## 9. 9.uniqueLock.cpp

**Q:** How do you manage a lock that requires delayed locking, manual unlocking, or transfer of ownership?

**A:** `std::unique_lock`.

**Explanation:**
- Acts as a versatile RAII wrapper around `std::mutex`.
- Allows deferred locking (`std::defer_lock`) and explicit manual `unlock()` before scope exit.
- Compatible with condition variables (unlike `std::lock_guard`).

**Complexity / cost:** Slightly heavier than `lock_guard` because it must maintain an internal boolean flag for ownership state.

```cpp
/*
Topic : unique_lock In C++
Few important points to remember about the unique_lock is as follows:
1. The class unique_lock is a mutex ownership wrapper.
2. It Allows:
    a. Can Have Different Locking Strategies
    b. time-constrained attempts at locking (try_lock_for, try_lock_until)
    c. recursive locking
    d. transfer of lock ownership (move not copy)
    e. condition variables. (See this in coming videos)

Locking Strategies
   TYPE           EFFECTS(S)
1. defer_lock  do not acquire ownership of the mutex.
2. try_to_lock  try to acquire ownership of the mutex without blocking.
3. adopt_lock  assume the calling thread already has ownership of the mutex.
*/

#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

mutex m1;
int buffer = 0;

/*
For defer lock:-
unique_lock<mutex> lock(m1,defer_lock);  //Does not call lock on mutex m1, because used defer_lock .
// we can also write no of line codes before lock.
lock.lock();   //But then we will have to explicitly tell to lock when ever we want lock mutex m1.
*/

void task(const char* c,int loopFor){
    unique_lock<mutex> lock(m1);  //Automatically calls lock on mutex m1.
    // m1.lock();
    for(int i=0;i<loopFor;i++){
        buffer++;
        cout<<c<<buffer<<endl;
    }
    // m1.unlock();
}

int main(){
    thread t1(task,"T1 ",10);
    thread t2(task,"T2 ",10);
    t1.join();
    t2.join();
    return 0;
}
```

---

## 10. 10.conditionVariable.cpp

**Q:** How do you make a thread wait efficiently until a specific condition is met, without busy-waiting?

**A:** `std::condition_variable` with `wait()` and `notify_one()`.

**Explanation:**
- The consumer thread uses `wait()` with a lambda predicate to avoid spurious wakeups.
- Uses `std::unique_lock` since the condition variable must unlock the mutex while sleeping and re-acquire it upon waking.
- Safely synchronizes the `addMoney` (producer) and `withdrowMoney` (consumer) flows over the shared `balance`.

**Complexity / cost:** Puts waiting threads to sleep (zero CPU usage); minimal overhead on notification.

```cpp
/*
Topic : Condition Variable In C++

CV are used for two purpose :-
A. Notify other threads
B. Waiting for some conditions

1. Condition variables allows running threads to wait on some conditions and once those conditions are met the waiting thread is notified using: 
   a. notify_one();
   b. notify_all();
2. You need mutex to use condition variable.
3. If some thread want to wait on some condition then it has to do these things:
    a. Acquire the mutex lock using unique_lock<mutex> lock(m).
    b. Execute wait, wait_for, or wait_until. The wait operations atomically release the mutex and suspend the executioj of the threaed.
    c. When the condition variable is notified, the thread is awakened, and the mutex is atomically reacquied.
       The thread should then check the condition and resume waiting if the wake up was spurious.

4. Condition variable is used to synchronise two or more threads.
5. Best use case of condition variable is Producer/Consumer problem.
*/

#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

mutex m;
condition_variable cv;
long balance=0;

void addMoney(int money){
    lock_guard<mutex> lg(m);
    balance+=money;
    cout<<"Amount added! Current Balance : "<<balance<<endl;
    cv.notify_one();
}

void withdrowMoney(int money){
    unique_lock<mutex> ul(m);
    cv.wait(ul, []{ return (balance!=0) ? true : false; });  // if this is false then it release the unique_lock and wait for notify, after notify it will aquire the lock again.
    if(balance>=money){
        balance-=money;
        cout<<"Amount Deducted : "<<money<<endl;
    }else{
        cout<<"Amount Can't Be Deducted, Current Balance is less than "<<money<<endl;
    }
    cout<<"Current Balance is : "<<balance<<endl;

}

int main(){
    thread t1(withdrowMoney,700);
    //this_thread::sleep_for(chrono::seconds(2));
    thread t2(addMoney,500);
    t1.join();
    t2.join();
    return 0;
}
```

---

## 11. 11.deadLock.cpp

**Q:** How does a circular wait deadlock occur when acquiring multiple locks?

**A:** Inconsistent lock ordering across threads.

**Explanation:**
- `task1` acquires `m1` then `m2`; `task2` acquires `m2` then `m1`.
- If both threads execute their first lock simultaneously, neither can proceed.
- Demonstrates a classic circular dependency avoiding progress indefinitely.

**Complexity / cost:** Total system halt (infinite blocking) when the deadlock condition is hit.

```cpp
#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

int buffer=0;
mutex m1,m2;

void task1(){
    m1.lock();
    m2.lock();
    cout<<"Critical Section of T1"<<endl;
    m1.unlock();
    m2.unlock();
}

void task2(){
    m2.lock();
    m1.lock();
    cout<<"Critical Section of T2"<<endl;
    m2.unlock();
    m1.unlock();
}

//when t1 reaches to m1 and t2 reaches to m2 at same time the there will be deadlock.

int main(){
    thread t1(task1);
    thread t2(task2);
    t1.join();
    t2.join();
}
```

---

## 12. 12.threadSynchronisation.cpp

**Q:** How do you establish basic thread synchronization to safely increment a shared counter?

**A:** `std::mutex`.

**Explanation:**
- Identical concept to basic mutex usage, protecting a `balance` variable.
- Ensures the `balance += val` operation is atomic relative to other threads.
- Prevents race conditions during the read-modify-write CPU instruction cycle.

**Complexity / cost:** Blocks concurrent execution entirely at the critical section; `O(1)` lock/unlock per operation.

```cpp
/*
Topic : Thread or Process Synchronization

Few important points to remember for "Thread OR Process Synchronisation" is as follows:
1.0 Thread Or Process synchronize to access critical section.
2.0 Critical section is one or collection of program statements which should be executed by only one thread or process at a time.
If there is any doubt regarding the video, please let me know in the comment section i would definitely try to solve it.
*/

#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

int balance=0;
mutex m;

void addMoney(int val){
    m.lock();
    balance+=val;
    m.unlock();
}

int main(){
    thread t1(addMoney,100);
    thread t2(addMoney,200);
    t1.join();
    t2.join();
    cout<<"Final Balance : "<<balance<<endl;
}
```

---

## 13. 13lockMultiMutex.cpp

**Q:** How do you acquire multiple mutexes atomically without risking a circular-wait deadlock?

**A:** `std::lock()` (variadic).

**Explanation:**
- Uses the standard library's deadlock-avoidance algorithm to lock `m1` and `m2`.
- The threads can request the locks in any order (`m1, m2` vs `m2, m1`) without deadlocking.
- Once acquired, manual `unlock()` (or adoption via RAII) is required.

**Complexity / cost:** Blocking operation; internally uses backoff/retry algorithms to guarantee deadlock freedom.

```cpp
/*
sTopi : td::lock() In C++11 It is used to lock multiple mutex at the same time.

And the syntax is as follow:
std::lock(m1, m2, m3, m4);
1. All arguments are locked via a sequence of calls to lock(),  try_lock(), or unlock() on each argument.
2. Order of locking is not defined (it will try to lock provided mutex in any order and ensure that
    there is no deadlock).
3. It is a blocking call.

[Example:0] -- No deadlock.
    Thread 1                    Thread 2
    std::lock(m1,m2);           std::lock(m1,m2);

[Example:1] -- No deadlock. 

    Thread 1                    Thread 2
    std::lock(m1, m2);          std::lock(m2, m1);

[Example:2] -- No deadlock. 

    Thread 1                    Thread 2
    std::lock(m1, m2, m3, m4);  std::lock(m3, m4);
                                std::lock(m1, m2);

[Example:3] -- Yes, the below can deadlock. 

    Thread 1                    Thread 2
    std::lock(m1,m2);           std::lock(m3,m4);
    std::lock(m3,m4);           std::lock(m1,m2);

*/

#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

mutex m1,m2;

void task1(){
    lock(m1,m2);
    cout<<"Task1-Critical Section"<<endl;
    m1.unlock();
    m2.unlock();
}
void task2(){
    lock(m2,m1);
    cout<<"Task2-Critical Section"<<endl;
    m2.unlock();
    m1.unlock();
}

int main(){
    thread t1(task1);
    thread t2(task2);
    t1.join();
    t2.join();
    return 0;
}
```

---

## 14. 14.promiseFuture.cpp

**Q:** How do you pass a value or exception from a background thread to a waiting thread?

**A:** `std::promise` and `std::future`.

**Explanation:**
- The worker thread sets the result using `promise::set_value()`.
- The main thread retrieves the result using `future::get()`.
- Synchronizes data transfer transparently without needing a manual mutex or condition variable.

**Complexity / cost:** `get()` blocks until the value is ready; minimal synchronization overhead internally.

```cpp
/*
Topic : future and promise in Threading.

Notes :
1. promise 
    a. Used to set values or exceptions.
2. future 
    a. Used to get values from promise.
    b. Ask promise if the value is available.
    c. Wait for the promise.
*/

#include<iostream>
#include<thread>
#include<mutex>
#include<future>
#include<algorithm>
#include<chrono>
using namespace std;
using namespace std::chrono;
typedef long int ull;

void findOdd(promise<ull>&& OddSumPromise, ull start, ull end) {
    ull OddSum = 0;
    for(ull i=start;i<=end;i++){
        if(i & 1){
            OddSum+=i;
        }
    }
    OddSumPromise.set_value(OddSum);
}

int main(){
    ull start=0, end=1900000000;
    promise<ull> OddSum;
    future<ull> OddFuture = OddSum.get_future();

    cout<<"Thread Created"<<endl;
    thread t1(findOdd,std::move(OddSum),start,end);

    cout<<"Waiting for Result!!"<<endl;

    cout<<"OddSum : "<<OddFuture.get()<<endl;

    cout<<"Completed!!"<<endl;
    t1.join();
    return 0;
}
```

---

## 15. 15.async.cpp

**Q:** How do you execute a function asynchronously and retrieve its result without explicitly managing thread objects?

**A:** `std::async`.

**Explanation:**
- Launches `findOdd` as a background task and returns a `std::future`.
- Shows `std::launch::deferred` (lazy execution on `get()`) vs `std::launch::async` (immediate parallel execution).
- Automates the creation of the thread and the promise/future plumbing.

**Complexity / cost:** Eliminates manual thread management overhead; task dispatch relies on internal runtime implementation (often thread pools).

```cpp
/*
Topic : async

Notes :
1. It runs a function asynchronously (potentially in a new thread) and returns a future that will hold the result.
2. There are three launch policies for creating task:
    a. launch::async
    b. launch::deffered
    c. launch::async | launch::deffered
3. We can send future and lambda function as callback to async, it will work the same.
Working :
1. It automatically creates a thread (Or picks from internal thread pool) and a promise object for us.
2. Then passed promise object to thread function are returns the associated future object.
3. When our passed argument function exits then its value will be set in this promise object.
    so eventually return value will be available in future object.
*/

#include<iostream>
#include<thread>
#include<mutex>
#include<future>
#include<algorithm>
#include<chrono>
using namespace std;
using namespace std::chrono;
typedef long int ull;

ull findOdd(ull start, ull end) {
    ull OddSum = 0;
    for(ull i=start;i<=end;i++){
        if(i & 1){
            OddSum+=i;
        }
    }
    return OddSum;
}

int main(){
    ull start=0, end=1900000000;

    cout<<"Thread Created if policy is launched::async!"<<endl;
    future<ull> OddSum = async(launch::deferred,findOdd,start,end);  // it will run in same thread,whhen the .get() is called the fun task will run.
    // future<ull> OddSum = async(launch::async,findOdd,start,end); //it will create different task and run paralally.

    cout<<"Waiting for Result!!"<<endl;
    cout<<"OddSum : "<<OddSum.get()<<endl;

    cout<<"Completed!!"<<endl;
    return 0;
}
```

---

## 16. 16.producerConsumer.cpp

**Q:** How do you implement a thread-safe bounded producer-consumer queue?

**A:** `std::mutex` paired with `std::condition_variable`.

**Explanation:**
- Uses a shared `std::queue<int>` protected by a `std::mutex`.
- The producer waits if the buffer is full; the consumer waits if the buffer is empty.
- Both use `cond.wait()` with a predicate to safely handle spurious wakeups and state mutations.

**Complexity / cost:** Threads sleep efficiently when blocked; lock contention scales with push/pop frequency.

```cpp
#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

mutex mu;
condition_variable cond;
const unsigned int maxBufferSize = 50;
queue<int> buffer;

void producer(int val){
    while(val){
        unique_lock<mutex> locker(mu);
        cond.wait(locker, [](){ return buffer.size() < maxBufferSize; });
        buffer.push(val);
        cout<<"Producer: "<<val<<endl;
        val--;
        locker.unlock();
        cond.notify_one();
    }
}

void consumer(){
    while(true){
        unique_lock<mutex> locker(mu);
        cond.wait(locker, [](){ return buffer.size() > 0;});
        int val = buffer.front();
        buffer.pop();
        cout<<"Consumed: "<<val<<endl;
        locker.unlock();
        cond.notify_one();
    }
}

int main(){
    thread t1(producer,100);
    thread t2(consumer);
    t2.join();
    t1.join();
    return 0;
}
```

---

## 17. 17.threadProblem.cpp

**Q:** How do you coordinate multiple threads to take strict alternating turns accessing a resource?

**A:** `std::condition_variable` with a strict turn-based predicate.

**Explanation:**
- Threads wait on `cv.wait()` until `allowed_thread` matches their mapped index.
- A single mutex guards the shared cursor (`next_char`) and the turn-taking state.
- Wakes all threads via `notify_all()` so the correct next thread can evaluate its predicate and proceed.

**Complexity / cost:** `notify_all()` incurs a "thundering herd" overhead; `O(N)` wakeups where only 1 thread succeeds.

```cpp
/*
User will give input that how many threads should created and how many characters should be printed using each thread, and what is the string from which characters should be printed.

This video is about thread synchronisation and how it works. 
*/

#include<iostream>
#include<string>
#include<vector>
#include<condition_variable>
#include<thread>
#include<mutex>
#include<chrono>
using namespace std;

class MyPrinter{
    private:
        string str;
        int char_count;
        int thread_count;
        vector<thread> threads;
        vector<thread::id> thread_ids;
        int thread_id;
        int allowed_thread;
        mutex mx;
        condition_variable cv;
        int next_char;
    public:
        MyPrinter(string s,int cc,int tc){
            str=s;
            char_count=cc;
            thread_count=tc;
            thread_id=0;
            next_char=0;
            allowed_thread=0;
        }
        int getCurrntThreadId(const thread::id& id){
            int thread_id=0;
            for(auto& e : thread_ids){
                if(id == e) return thread_id;
                thread_id++;
            }
            return -1;
        }
        void run(){
            for(int i=0;i<thread_count;i++){
                thread t(&MyPrinter::printThread,this);
                cout<<"Thread "<<t.get_id()<<" is "<<i<<endl;
                thread_ids.push_back(t.get_id());
                threads.push_back(std::move(t));
            }
            for(int i=0;i<thread_count;i++){
                threads[i].join();
            }
        }
        void waitForAllThreadInit(){
            while(1){
                if(thread_count==thread_ids.size()) return;
            }
        }
        void printThread(){
            while(1){
                waitForAllThreadInit();
                this_thread::sleep_for(chrono::milliseconds(1000));
                unique_lock<mutex> lock(mx);
                cv.wait(lock, [this] { return this_thread::get_id() == thread_ids[allowed_thread]; });
                printChars();
                allowed_thread++;
                if(allowed_thread==thread_count) allowed_thread=0;
                if(next_char >= str.length()) next_char -= str.length();
                lock.unlock();
                cv.notify_all();
            }
        }
        void printChars(){
            cout<<"ThreadId "<<getCurrntThreadId(this_thread::get_id())<<" : ";
            int print_count=0;
            for(int i=next_char; i < str.length() && print_count < char_count; i++){
                cout<<str[i];
                print_count++;
            }
            if(print_count<char_count){
                for(int i=0;i<char_count-print_count;i++){
                    cout<<str[i];
                }
            }
            next_char=next_char+char_count;
            cout<<endl;
        }

};

int main(int argc , char *argv[]){
    if(argc !=4) {
        cout<<"Please provide 3 arguments - a string, char count & thread count"<<endl;
        return 1;
    }
    string str = argv[1];
    int char_count = atoi(argv[2]);
    int thread_count = atoi(argv[3]);
    MyPrinter p(str,char_count,thread_count);
    p.run();
    return 0;
}
```

---

## 18. 18.binarySemaphore.cpp

**Q:** How do you signal between two threads using a lightweight C++20 synchronization primitive?

**A:** `std::binary_semaphore` (C++20).

**Explanation:**
- Main thread signals the worker to start via `release()`.
- Worker thread sleeps, then signals the main thread via `release()`.
- Avoids the boilerplate of a mutex and condition variable for simple notification flags.

**Complexity / cost:** Extremely lightweight, often implemented using efficient lock-free atomic wait operations (futex on Linux).

```cpp
#include<iostream>
#include<thread>
#include<semaphore>
#include<chrono>

using namespace std;

std::binary_semaphore smphSignalMainToThread{0},smphSignalThreadToMain{0};

void threadProc(){
    smphSignalMainToThread.acquire();
    cout<<"[thread] got the signal\n";

    this_thread::sleep_for(3s);
    using namespace std::literals;
    cout<<"[thread] send the signal\n";
    smphSignalThreadToMain.release();  
}

int main(){
    thread threadWorker(threadProc);
    cout<<"[main] send the signal\n";
    smphSignalMainToThread.release();
    smphSignalThreadToMain.acquire();
    cout<<"[main] got the signal\n";
    threadWorker.join();
    return 0;
}
```

---

## 19. 19.producerConsumerUsingSemaphore.cpp

**Q:** How do you synchronize a strict alternating producer-consumer pipeline without a mutex?

**A:** Paired `std::binary_semaphore` objects.

**Explanation:**
- Uses `signal_to_producer` (initialized to 1) and `signal_to_consumer` (initialized to 0).
- Producer acquires its semaphore, fills the buffer, and releases the consumer's semaphore.
- Consumer acquires its semaphore, empties the buffer, and releases the producer's semaphore.

**Complexity / cost:** Strict ping-pong synchronization; completely lock-free communication via semaphores.

```cpp
#include <chrono>
#include <iostream>
#include <semaphore>
#include <thread>
using namespace std::chrono;
using namespace std;

std::binary_semaphore 
signal_to_producer{1}, 
signal_to_consumer{0};

#define buff_size 5
int buff[buff_size];

void producer() {
    while(1){ 
      signal_to_producer.acquire();
      cout<<"Produced = ";
      for(int i=0; i<buff_size; ++i) {
        buff[i] = i*i;  // task of producer
        cout << buff[i] << " " << std::flush;
        std::this_thread::sleep_for(milliseconds(200));
      }
      cout << endl;
      signal_to_consumer.release();
    }
}

void consumer(){ 
    while(1){ 
      signal_to_consumer.acquire();
      cout<<"Consumed = ";
      for(int i=buff_size-1; i>=0; --i) {
        cout << buff[i] << " " << std::flush;
        buff[i] = 0; // task of consumer
        std::this_thread::sleep_for(milliseconds(200));
      }
      cout << endl; cout << endl; 
      signal_to_producer.release();
    }
} 

int main()
{
    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);

    std::cout << "[main] Got the signal\n"; // response message
    producer_thread.join();
    consumer_thread.join();
    return 0;
}
```

---

## 20. 20.staticVariableInThreading.cpp

**Q:** How do you safely mutate a static or global variable across multiple threads?

**A:** `std::mutex` (or `std::atomic`).

**Explanation:**
- The shared state is a statically scoped integer `s`.
- Uses manual `lock()` and `unlock()` around the increment operation.
- Prevents race conditions that would otherwise corrupt the counter.

**Complexity / cost:** Heavy lock contention on a single static memory address; an `atomic<int>` would be far cheaper here.

```cpp
#include <iostream>
#include <thread>
#include <mutex>
using namespace std;
static int s = 0;

mutex m;
void fun() {
    int count = 100000;
    m.lock();
    while(count--) {
        ++s;
    }
    m.unlock();
    cout << s << endl;
}

int main() {
    std::thread t1(fun);
    std::thread t2(fun);
    t1.join();
    t2.join();
}
```

---

## 21. 21.raceCondition.cpp

**Q:** What happens when multiple threads execute a read-modify-write cycle on shared memory simultaneously?

**A:** Data corruption (race condition), prevented by `std::mutex`.

**Explanation:**
- Identical to previous files, showing a broken state if the mutex is commented out.
- The `counter` requires protection because `++counter` compiles to multiple CPU instructions (load, increment, store).
- Solved by wrapping the increment in a critical section.

**Complexity / cost:** Explicit serialization of loop iterations; throughput drops to single-threaded performance.

```cpp
#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

int counter;
int times = 1000000;
std::mutex m;

void run() {
    for(int i=0; i<times; ++i) {
      m.lock();
      ++counter;
      m.unlock();
    }
}

int main() {
    
    std::thread mythread1(run);
    std::thread mythread2(run);

    mythread1.join();
    mythread2.join();
    
    cout << counter << endl;
    return 0;
}
```

---

## 22. 22.threadPool.cpp

**Q:** How do you avoid the overhead of spawning and destroying a thread for every background task?

**A:** Thread Pool (Worker queue using `std::condition_variable` and `std::mutex`).

**Explanation:**
- Creates a fixed number of persistent worker threads holding a wait loop.
- Tasks (`std::function<void()>`) are pushed into a shared `std::queue`.
- A condition variable wakes up idle workers to dequeue and execute tasks asynchronously.

**Complexity / cost:** High initial creation cost, but amortizes to `O(1)` cheap queue operations for task dispatching.

```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <sstream>
#include <mutex>
#include <condition_variable>

class ThreadPool {
private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;

  std::mutex queueMutex;
  std::condition_variable condition;
  bool stop;
  
public:
  ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
      workers.emplace_back([this] {
        for (;;) {
          std::unique_lock<std::mutex> lock(queueMutex);
          condition.wait(lock, [this] { return stop || !tasks.empty(); });
          if (stop && tasks.empty())
            return;
          auto task = std::move(tasks.front()); // Excract task from tasks list.
          tasks.pop(); // Remove task from list as going to execute it.
          lock.unlock(); // Unlock mutex, so another thread can accept the tasks.
          task();  // Run The Task
        }
      });
    }
  }

  template<class F>
  void enqueue(F&& task) {
    std::unique_lock<std::mutex> lock(queueMutex);
    tasks.emplace(std::forward<F>(task));
    lock.unlock();
    condition.notify_one();
  }

  ~ThreadPool() {
    std::unique_lock<std::mutex> lock(queueMutex);
    stop = true;
    lock.unlock();
    condition.notify_all();
    for (std::thread& worker : workers)
      worker.join();  
  }

};

std::string get_thread_id() {
  auto myid = std::this_thread::get_id();
  std::stringstream ss;
  ss << myid;
  std::string mystr = ss.str();
  return mystr;
}

int main() {

  ThreadPool pool(8); // Create a pool with N number of worker threads

  std::cout << "Thread Pool Created\n";
  std::cout << "Enqueue (Assign) some tasks \n";

  for (int i = 0; i < 40; ++i) {
    pool.enqueue([i] {
      printf("Task %d %s executed by thread \n", i, get_thread_id().c_str());
      std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate some work
    });
  }

  // Main thread continues doing other things 
  // while the tasks are executed in the background

  return 0;
}
```

---

## 23. 23. 1114. Print in Order

**Q:** How do you enforce a strict chronological execution order (First -> Second -> Third) among independent threads?

**A:** `std::mutex` (blocking) or `std::condition_variable` (state-driven).

**Explanation:**
- Solution 1 uses pre-locked mutexes to intentionally block threads 2 and 3 until thread 1 unlocks them (relies on undefined behaviour in standard C++ if unlocked by a different thread, but common in puzzles).
- Solution 2 uses a safe `condition_variable` tracking a `state` variable (1, 2, or 3) with `notify_all()`.
- Ensures chronological correctness despite unpredictable OS thread scheduling.

**Complexity / cost:** Solution 1 blocks; Solution 2 sleeps efficiently but incurs `notify_all()` broadcast overhead.

```cpp
Q : 
Suppose we have a class:

public class Foo {
  public void first() { print("first"); }
  public void second() { print("second"); }
  public void third() { print("third"); }
}
The same instance of Foo will be passed to three different threads. Thread A will call first(), thread B will call second(), and thread C will call third(). Design a mechanism and modify the program to ensure that second() is executed after first(), and third() is executed after second().

#Sol1 uning mutex only...

class Foo {
    mutex m1,m2,m3;
public:
    Foo() {
        m2.lock();
        m3.lock();
    }
    void first(function<void()> printFirst) {
        m1.lock();
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
        m1.unlock();
        m2.unlock();
    }
    void second(function<void()> printSecond) {
        m2.lock();
        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();
        m2.unlock();
        m3.unlock();
    }
    void third(function<void()> printThird) {
        m3.lock();
        // printThird() outputs "third". Do not change or remove this line.
        printThird();
        m3.unlock();
    }
};


#Sol2 usning mutex and condition variable...

class Foo {
    mutex m;
    condition_variable cv;
    int state=1;
public:
    Foo() { 
    }
    void first(function<void()> printFirst) {
        unique_lock<mutex> lock(m);
        // printFirst() outputs "first". Do not change or remove this line.
        state=2;
        printFirst();
        cv.notify_all();
    }
    void second(function<void()> printSecond) {
        unique_lock<mutex> lock(m);
        // printSecond() outputs "second". Do not change or remove this line.
        cv.wait(lock,[&](){ return state==2; });
        state=3;
        printSecond();
        cv.notify_all();
    }
    void third(function<void()> printThird) {
        unique_lock<mutex> lock(m);
        cv.wait(lock,[&](){ return state==3; });
        // printThird() outputs "third". Do not change or remove this line.
        printThird();
    }
};
```

---

## 24. 24.Odd-Even Printing with 2 Threads

**Q:** How do you alternate execution precisely between two threads printing a sequence?

**A:** `std::condition_variable` with a toggling boolean predicate.

**Explanation:**
- The shared state is the `oddTurn` boolean flag.
- The odd thread waits for `oddTurn == true`; the even thread waits for `!oddTurn`.
- After printing, each thread flips the flag and calls `notify_all()` to wake the partner.

**Complexity / cost:** `O(N)` synchronized context switches; minimal CPU waste but maximum thread switching overhead.

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

mutex m;
condition_variable cv;
bool oddTurn = true;

void printOdd(int n) {
    for (int i = 1; i <= n; i += 2) {
        unique_lock<mutex> lock(m);
        cv.wait(lock, [] { return oddTurn; });
        cout << i << " ";
        oddTurn = false;
        cv.notify_all();
    }
}

void printEven(int n) {
    for (int i = 2; i <= n; i += 2) {
        unique_lock<mutex> lock(m);
        cv.wait(lock, [] { return !oddTurn; });
        cout << i << " ";
        oddTurn = true;
        cv.notify_all();
    }
}

int main() {
    int n = 10;
    thread t1(printOdd, n), t2(printEven, n);
    t1.join(); t2.join();
}
```

---

## 25. 25. 1116. Print Zero Even Odd

**Q:** How do you coordinate three threads to interleave specific state constraints (zero, even, odd) in a unified sequence?

**A:** `std::condition_variable` checking complex combined state predicates.

**Explanation:**
- The shared state involves a counter `x` and a boolean toggle `z` (for zero's turn).
- The `zero` thread waits for `z == true`.
- The `even`/`odd` threads wait for `z == false` and check the modulo of `x`.
- Employs `notify_all()` heavily to awaken all participants to evaluate their respective conditions.

**Complexity / cost:** High synchronization overhead due to `notify_all()` waking threads that immediately go back to sleep if their modulo check fails.

```cpp
#Q:
You have a function printNumber that can be called with an integer parameter and prints it to the console.

For example, calling printNumber(7) prints 7 to the console.
You are given an instance of the class ZeroEvenOdd that has three functions: zero, even, and odd. The same instance of ZeroEvenOdd will be passed to three different threads:

Thread A: calls zero() that should only output 0's.
Thread B: calls even() that should only output even numbers.
Thread C: calls odd() that should only output odd numbers.
Modify the given class to output the series "010203040506..." where the length of the series must be 2n.

#Sol using mutes and condition variable :

class ZeroEvenOdd {
private:
    int n;
    int x=1;
    bool z=true;
    mutex m;
    condition_variable cv;
public:
    ZeroEvenOdd(int n) {
        this->n=n;
    }
    // printNumber(x) outputs "x", where x is an integer.
    void zero(function<void(int)> printNumber) {
        while(true){
        unique_lock<mutex> lock(m);
        cv.wait(lock,[&](){ return z || x>n; });
        if(x>n) {cv.notify_all();break;}
        printNumber(0);
        z=!z;
        cv.notify_all();
        }
    }
    void even(function<void(int)> printNumber) {
        while(true){
        unique_lock<mutex> lock(m);
        cv.wait(lock,[&](){ return (z==false && x%2==0)|| x>n; });
        if(x>n) { cv.notify_all(); break; }
        printNumber(x);
        x++;
        z=!z;
        cv.notify_all();
        }
    }
    void odd(function<void(int)> printNumber) {
        while(true){
        unique_lock<mutex> lock(m);
        cv.wait(lock,[&](){ return (z==false && x%2==1)|| x>n; });
        if(x>n) {cv.notify_all(); break; }
        printNumber(x);
        x++;
        z=!z;
        cv.notify_all();
        }
    }
};
```