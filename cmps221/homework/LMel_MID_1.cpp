//Leopoldo Melendez
//MidTerm Program 1
//11-7-2013
#include<iostream>
#include<cstdlib>
#include<cmath>

using namespace std;
int main()
{
    const double PI = 3.14159;

    double sine[361];

    for(int angle=0;angle<361;angle++)
    {
	sine[angle] = sin(angle * PI / 180);
    }

    cout<<"sin30 = "<<sine[30]<<endl;
    cout<<"sin45 = "<<sine[45]<<endl;
    cout<<"sin60 = "<<sine[60]<<endl;

    return 0;
}
