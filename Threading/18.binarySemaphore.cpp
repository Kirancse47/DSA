#include<iostream>
#include<thread>
#include<semaphore>
#include<chrono>

using namespace std;

std::binary_semaphore smphSignalMainToThread{0},smphSignalThreadToMain{0};

void threadProc(){
    smphSignalMainToThread.acquire();
    cout<<"[thread] got the signal\n";

    this_thread::sleep_for(3s);
    using namespace std::literals;
    cout<<"[thread] send the signal\n";
    smphSignalThreadToMain.release();  
}

int main(){
    thread threadWorker(threadProc);
    cout<<"[main] send the signal\n";
    smphSignalMainToThread.release();
    smphSignalThreadToMain.acquire();
    cout<<"[main] got the signal\n";
    threadWorker.join();
    return 0;
}