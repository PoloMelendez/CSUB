//Leopoldo Melendez
//Lab or HW #
//Multiples of 3 and 5
#include<iostream>
using namespace std;
int main()
{
    int sum=0;
    for(int i=1;i<1000;i++)
    {
	if(i%3==0 || i%5==0)
	{
	    cout<<i<<endl;
	    sum=sum+i;
	}
	else
	    continue;
    }
    cout<<"Sum from 1 - 1000 is equal to "<<sum<<endl;

    return 0;
}
