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