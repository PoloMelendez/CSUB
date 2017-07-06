//Leopoldo Melendez
//HW 03b
//10-2-2013
#include<iostream>
#include<cmath>
using namespace std;
int main()
{
    //counts through from 3 to 300, using odd numbers only
    //also declares the boolean isPrime to be true, and if after the inner for loop is executed
    //and isPrime is still true, the current number will be printed
    for(int number1=3;number1<300;number1=number1+2)
    {
	bool isPrime=true;
	//divides the current number being tested by every positive number less then itself
	//if any of these numbers divide into the number, isPrime is declared false and the loop breaks
	//if none of the numbers divide into the number, isPrime is unchanged and the loop ends
	for(int number2=2;number2<=number1-1;number2++)
	{
	   if(number1%number2 ==0)
	   {
	       isPrime=false;
	       break;
	   }
	}
	if(isPrime)
	    cout<<number1<<" ";
    }
    cout<<endl;

    return 0;
}
