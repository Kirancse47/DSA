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