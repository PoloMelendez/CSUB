//Leopoldo Melendez
//Lab 12
//11-14-13
#include<iostream>
#include<cstdlib>
#include<cstring>
using namespace std;

void sumyay(int, int, int&);
void swapshouldnotwork(int*, int*);

int main()
{
    int days[12];

    for(int i=0;i<12;i++)
    {
	days[i] = 9;
	cout<<days[i]<<endl;
    }

    int *pointer = days;

    for(int i = 0;i<12;i++)
    {
	*(pointer+i) = 12;
	cout<<days[i]<<endl;
    }

    for(int i = 0;i<12;i++)
    {
	cout<<&days[i]<<endl<<endl;
    }

    int sum;
    int a = 1;
    int b = 2;

    sumyay(a, b, sum);

    cout<<sum<<endl;

    swapshouldnotwork(&a, &b);

    cout<<a<<endl<<b<<endl;

    char name[] = "John Smith";
    char *needle;
    needle = strstr(name," ");
    if(needle == NULL)
	cout<<"Not Found"<<endl;
    else
    {
	cout<<static_cast<void*>(name)<<endl;
	cout<<"Found it at"<<endl;
	cout<<static_cast<void*>(&needle)<<endl;
    }

    char str[]="We are the champions";
    char *p;
    p=strtok(str," ");
    while(p!=NULL)
    {
	cout<<p<<endl;
	p=strtok(NULL," ");
    }

    int answer = 5%12;
    cout<<answer<<endl;


    return 0;    
}

void sumyay(int a, int b, int &sum)
{
    sum = a + b;
}

void swapshouldnotwork(int* x, int* y)
{
    int c = *x;
    *x = *y;
    *y = c;
}
    
