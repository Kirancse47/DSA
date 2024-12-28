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