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