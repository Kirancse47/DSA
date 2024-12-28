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