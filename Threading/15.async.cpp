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