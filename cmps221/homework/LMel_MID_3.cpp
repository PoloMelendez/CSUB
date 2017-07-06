//Leopoldo Melendez
//MidTerm Problem 2
//11-7-2013
#include<iostream>
#include<fstream>
using namespace std;

bool headerprompt(string);

int main()
{
    int boxes;
    
    cout<<"Enter a single integer: ";
    cin>>boxes;
    
    for(int i=1;i<=boxes;i++)
    {
	for(int j=1;j<=boxes;j++)
	{
	    cout<<"*";
	}
	cout<<endl;
    }

    return 0;
}
