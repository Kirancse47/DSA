/*
Topic : Condition Variable In C++

CV are used for two purpose :-
A. Notify other threads
B. Waiting for some conditions

1. Condition variables allows running threads to wait on some conditions and once those conditions are met the waiting thread is notified using: 
   a. notify_one();
   b. notify_all();
2. You need mutex to use condition variable.
3. If some thread want to wait on some condition then it has to do these things:
    a. Acquire the mutex lock using unique_lock<mutex> lock(m).
    b. Execute wait, wait_for, or wait_until. The wait operations atomically release the mutex and suspend the executioj of the threaed.
    c. When the condition variable is notified, the thread is awakened, and the mutex is atomically reacquied.
       The thread should then check the condition and resume waiting if the wake up was spurious.

4. Condition variable is used to synchronise two or more threads.
5. Best use case of condition variable is Producer/Consumer problem.
*/

#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

mutex m;
condition_variable cv;
long balance=0;

void addMoney(int money){
    lock_guard<mutex> lg(m);
    balance+=money;
    cout<<"Amount added! Current Balance : "<<balance<<endl;
    cv.notify_one();
}

void withdrowMoney(int money){
    unique_lock<mutex> ul(m);
    cv.wait(ul, []{ return (balance!=0) ? true : false; });  // if this is false then it release the unique_lock and wait for notify, after notify it will aquire the lock again.
    if(balance>=money){
        balance-=money;
        cout<<"Amount Deducted : "<<money<<endl;
    }else{
        cout<<"Amount Can't Be Deducted, Current Balance is less than "<<money<<endl;
    }
    cout<<"Current Balance is : "<<balance<<endl;

}

int main(){
    thread t1(withdrowMoney,700);
    //this_thread::sleep_for(chrono::seconds(2));
    thread t2(addMoney,500);
    t1.join();
    t2.join();
    return 0;
}