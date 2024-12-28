/*
TOPIC: Introduction to thread in c++ (c++11)

QUESTIONS
1. What do you understand by thread and give one example in C++?

ANSWER
0. In every application there is a default thread which is main(), in side this we create other threads.
1. A thread is also known as lightweight process. Idea is achieve parallelism by dividing a process into multiple threads. 
   For example:
   (a) The browser has multiple tabs that can be different threads. 
   (b) MS Word must be using multiple threads, one thread to format the text, another thread to process inputs (spell checker)
   (c) Visual Studio code editor would be using threading for auto completing the code. (Intellicence)

WAYS TO CREATE THREADS IN C++11
1. Function Pointers
2. Lambda Functions
3. Functors
4. Member Functions
5. Static Member functions

to run : g++ -std=c++11 -pthread multiThreading.cpp
*/

#include<iostream>
#include<thread>
using namespace std;
typedef unsigned long long ull;

ull oddSum=0;
ull evenSum=0;

void findEven(ull start,ull end){
    for(ull i=start;i<=end;i++){
        if(!(i&1)) evenSum+=i;
    }
}

void findOdd(ull start,ull end){
    for(ull i=start;i<=end;i++){
        if((i&1)) oddSum+=i;
    }
}

int main(){
    ull start=0, end=1900000;
    thread t1(findEven,start,end);
    thread t2(findOdd,start,end);
    t1.join();
    t2.join();
    cout<<"OddSum : "<<oddSum<<endl;
    cout<<"EvenSum : "<<evenSum<<endl;
    return 0;
}