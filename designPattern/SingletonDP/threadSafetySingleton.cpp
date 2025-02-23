/*
There are basically three ways to achieve thread safety in singleton design pattern and that is:
1. Use mutex lock and unlock before and after checking if instance of singleton class is null or not, but there is a down side to this approach that is not good at performance side, we have to always apply mutex lock and unlock when getInstance is called.
2. Eager Instantiation (This is about instantiating singleton class by calling getInstance method early in main function so that before even threads start and race condition arise)
3. This approach is slight modification of 1st approach, this talks about keeping null check before mutex lock so that if once instance is available mutex lock would never be used and hence performance can be good.
*/

#include<iostream>

//Approch 1 (Mutx Lock/Unlock)...
class MySingleton{
public:
    static MySingleton * GetInstance(){
        //Lock Mutex
        if (m_pOnlyOneInstance == NULL){{
                m_pOnlyOneInstance = new MySingleton();
            }
        }
        //Unlock Mutex
        return m_pOnlyOneInstance;
    }
private:
    static MySingleton * m_pOnlyOneInstance;
    MySingleton();
}; 
//Problem : Performance is not good;


//Approch 2 (Eager Instantiation)...
class MySingleton{
public:
    static MySingleton * GetInstance(){
        //Lock Mutex
        if (m_pOnlyOneInstance == NULL){{
                m_pOnlyOneInstance = new MySingleton();
            }
        }
        //Unlock Mutex
        return m_pOnlyOneInstance;
    }
private:
    static MySingleton * m_pOnlyOneInstance;
    MySingleton();
}; 

int main(){
    MySingleton *s = new MySingleton::GetInstance();

    //Rest of your code where you start thread and other stuff
    .
    .
    return 0;
}
//Problem : Lazy Instantiation is not there.


//Approch 3 (Optimized Mutx Lock/Unlock)...
class MySingleton{
public:
    static MySingleton * GetInstance(){
        if(m_pOnlyOneInstance == NULL){{
            //Lock Mutex
            if(m_pOnlyOneInstance == NULL){{
                m_pOnlyOneInstance = new MySingleton();
            }
            //Unlock Mutex
            return m_pOnlyOneInstance;
        }
    }
private:
    static MySingleton * m_pOnlyOneInstance;
    MySingleton();
};