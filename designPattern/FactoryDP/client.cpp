#ifndef __io__
#define __io__

#include<iostream>
using namespace std;

#endif

#include "ToyFactory.cpp"

int main(){
    int type;
    while(1){
        cout<<"Enter type or type 0 for exit : ";
        cin>>type;
        if(!type) break;
        Toy *v = ToyFactory::createToy(type);
        if(v){
            v->showProduct();
            free(v);
        }else{
            cout << "invalid toy type please re-enter type" << endl;
        }
    }
    cout<<"Exit..";
    return 0;
}


