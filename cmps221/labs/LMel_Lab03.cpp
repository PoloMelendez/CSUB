//Leopoldo Melendez
//Lab03
//Sep 26, 2013
#include<iostream>
using namespace std;
int main()
{
    int number = 1;
   
    while(number!=0)
    {
	cout<<"Enter a number: ";
	cin>>number;

	if(number<0)
	{
	    cout<<"The number is negative"<<endl;
	    if(number%3==0)
		cout<<"The number is divisible by 3"<<endl;
	    if(number%5==0)
		cout<<"The number is divisible by 5"<<endl;
	}
	else if(number%3==0&&number!=0)
	{
	    cout<<"The number is divisible by 3"<<endl;
	    if(number%5==0&&number!=0)
		cout<<"The number is divisible by 5"<<endl;
	}
	else if(number%5==0&&number!=0)
	    cout<<"The number is divisible by 5"<<endl;
	else if(number!=0&&number!=0)
	    cout<<"Your input was worthless";
    }

    cout<<"We're done here"<<endl;


    return 0;
}
