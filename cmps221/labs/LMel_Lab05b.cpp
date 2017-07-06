//Leopoldo Melendez
//Lab 05b
//10-3-2013
#include<iostream>
using namespace std;
int main()
{
    for(int i=1;i<=10;i++)
    {
	for(int j=1;j<=10;j++)
	{
	    if(i*j>60)
		break;
	    else
		cout<<i<<" x "<<j<<" = "<<i*j<<endl;
	}
	cout<<endl;
    }


    return 0;
}
