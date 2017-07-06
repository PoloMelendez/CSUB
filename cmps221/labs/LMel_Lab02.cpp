//Leopoldo Melendez
//Lab02
#include<iostream>
using namespace std;
int main()
{
    int number;

    cout<<"Please enter a number between 1 and 10: ";
    cin>>number;

    if(number>=0 && number<=10)
	cout<<"You entered "<<number<<endl<<endl;
    else
	cout<<"Error: "<<number<<" is not a number between 1 and 10"<<endl<<endl;

    cout<<"The size of an int is "<<sizeof(int)<<" bytes"<<endl;
    cout<<"The size of an float is "<<sizeof(float)<<" bytes"<<endl;
    cout<<"The size of an double is "<<sizeof(double)<<" bytes"<<endl;
    cout<<"The size of an char is "<<sizeof(char)<<" byte"<<endl;


    return 0;
}
