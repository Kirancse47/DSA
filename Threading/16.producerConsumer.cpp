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