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
