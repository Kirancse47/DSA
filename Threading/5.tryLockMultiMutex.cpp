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