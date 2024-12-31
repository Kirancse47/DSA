/*
User will give input that how many threads should created and how many characters should be printed using each thread, and what is the string from which characters should be printed.

This video is about thread synchronisation and how it works. 
*/

#include<iostream>
#include<string>
#include<vector>
#include<condition_variable>
#include<thread>
#include<mutex>
#include<chrono>
using namespace std;

class MyPrinter{
    private:
        string str;
        int char_count;
        int thread_count;
        vector<thread> threads;
        vector<thread::id> thread_ids;
        int thread_id;
        int allowed_thread;
        mutex mx;
        condition_variable cv;
        int next_char;
    public:
        MyPrinter(string s,int cc,int tc){
            str=s;
            char_count=cc;
            thread_count=tc;
            thread_id=0;
            next_char=0;
            allowed_thread=0;
        }
        int getCurrntThreadId(const thread::id& id){
            int thread_id=0;
            for(auto& e : thread_ids){
                if(id == e) return thread_id;
                thread_id++;
            }
            return -1;
        }
        void run(){
            for(int i=0;i<thread_count;i++){
                thread t(&MyPrinter::printThread,this);
                cout<<"Thread "<<t.get_id()<<" is "<<i<<endl;
                thread_ids.push_back(t.get_id());
                threads.push_back(std::move(t));
            }
            for(int i=0;i<thread_count;i++){
                threads[i].join();
            }
        }
        void waitForAllThreadInit(){
            while(1){
                if(thread_count==thread_ids.size()) return;
            }
        }
        void printThread(){
            while(1){
                waitForAllThreadInit();
                this_thread::sleep_for(chrono::milliseconds(1000));
                unique_lock<mutex> lock(mx);
                cv.wait(lock, [this] { return this_thread::get_id() == thread_ids[allowed_thread]; });
                printChars();
                allowed_thread++;
                if(allowed_thread==thread_count) allowed_thread=0;
                if(next_char >= str.length()) next_char -= str.length();
                lock.unlock();
                cv.notify_all();
            }
        }
        void printChars(){
            cout<<"ThreadId "<<getCurrntThreadId(this_thread::get_id())<<" : ";
            int print_count=0;
            for(int i=next_char; i < str.length() && print_count < char_count; i++){
                cout<<str[i];
                print_count++;
            }
            if(print_count<char_count){
                for(int i=0;i<char_count-print_count;i++){
                    cout<<str[i];
                }
            }
            next_char=next_char+char_count;
            cout<<endl;
        }

};

int main(int argc , char *argv[]){
    if(argc !=4) {
        cout<<"Please provide 3 arguments - a string, char count & thread count"<<endl;
        return 1;
    }
    string str = argv[1];
    int char_count = atoi(argv[2]);
    int thread_count = atoi(argv[3]);
    MyPrinter p(str,char_count,thread_count);
    p.run();
    return 0;
}