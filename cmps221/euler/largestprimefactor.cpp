//Leopoldo Melendez
//Lab or HW #
//Date
#include<iostream>
using namespace std;
int main()
{
    int number = 49;
    int factor = 1;

    for(int i=1;i<=number;i++)
    {
	if( number%i==1)
	{
	    for(int j=1;j<=i;j++)
	    {
		bool isPrime=false;
		if(i%j==1)
		{
		    isPrime = true;
		}
		if(isPrime)
		    cout<<i<<endl;
		else
		    break;
	    }
	}
    }


    return 0;
}
