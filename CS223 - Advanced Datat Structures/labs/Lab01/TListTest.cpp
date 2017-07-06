/*cmps 223 
 *Leopoldo Melendez 
 *Assignment 1;
 *Date completed: January 9, 2014;
 * */
#include<iostream>
#include<"TList.h">
using namespace std;

int main(){
    TList<int> iLst(15);
    TList<double> dLst(20);
    
    //for (int i = 0; i < 10; i ++)
    //	iLst.put();

    iLst.show();
    dLst.show();

    return 0;
}
