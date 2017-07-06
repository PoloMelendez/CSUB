#include<iostream>
using namespace std;
int main()
{
    const float PI = 3.14159265;
    float radius;
    cout<<"Enter the radius: ";
    cin>>radius;
    
    if(radius>0)
	cout<<"Area of circle with radius "<<radius<<" is "<<PI*radius*radius<<endl;
    else
	cout<<"oops!"<<endl;


    return 0;
}
