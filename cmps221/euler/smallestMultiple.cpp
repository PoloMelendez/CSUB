//Leopoldo Melendez
//Lab or HW #
//Finds smallest multiple
#include<iostream>
using namespace std;
int main()
{
    bool isTrue = false;
    int number = 1;

    while(isTrue)
    {
	for(int i=1;i<=20;i++)
	{
	    if(number%i==0)
		isTrue=true;
	    else
	    {
		isTrue=false;
		break;
	    }
	}
	    if(!isTrue)
	    number++;
    }

    cout<<number<<endl;

    return 0;
}
