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