/*
Topic : future and promise in Threading.

Notes :
1. promise 
    a. Used to set values or exceptions.
2. future 
    a. Used to get values from promise.
    b. Ask promise if the value is available.
    c. Wait for the promise.
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

void findOdd(promise<ull>&& OddSumPromise, ull start, ull end) {
    ull OddSum = 0;
    for(ull i=start;i<=end;i++){
        if(i & 1){
            OddSum+=i;
        }
    }
    OddSumPromise.set_value(OddSum);
}

int main(){
    ull start=0, end=1900000000;
    promise<ull> OddSum;
    future<ull> OddFuture = OddSum.get_future();

    cout<<"Thread Created"<<endl;
    thread t1(findOdd,std::move(OddSum),start,end);

    cout<<"Waiting for Result!!"<<endl;

    cout<<"OddSum : "<<OddFuture.get()<<endl;

    cout<<"Completed!!"<<endl;
    t1.join();
    return 0;
}