//Leopoldo Melendez
//HW 03b
//10-2-2013
#include<iostream>
#include<cmath>
using namespace std;
int main()
{

    for(int number1=3;number1<300;number1++)
    {
	for(int number2=2;number2*number2<=number1;number2++)
	{
	   if(number1%number2 == 0)
	       break;
	   else if(number2+1 > sqrt(number1))
	       cout<<number1<<" ";
	}
    }
    cout<<endl;

    return 0;
}
