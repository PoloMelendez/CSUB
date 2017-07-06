//Leopoldo Melendez
//Lab 10
//10-31-2013
#include<iostream>
#include<cctype>
#include<cstdlib>
#include<cstring>
#include<limits>
#include<fstream>
using namespace std;

void parse_name(char [], char [], char []);

int main()
{
    //Initializes 3 arrays, full first and last, and sets their size.
    char full_name[40]={};
    string input;
    double doubleArray[5]={};
    char first_name[20]={};
    char last_name[20]={};
    double total;
    
    //Asks the user to input a name, and to enter 5 values. The name
    //is ran through parse_name and split into the first and last, then
    //the entered values and the first and last array are printed into
    //the Lab10 text file.
    cout<<"Enter the full name: ";
    cin.get(full_name, 40);

    ofstream ofs("Lab10.txt");

    for(int i= 0;i<5;i++)
    {
	cout<<"Enter value "<<i+1<<": ";
	cin>>input;
	doubleArray[i] = atof(input.c_str());
    }

    parse_name(full_name, first_name, last_name);

    ofs<<"First Name: "<<first_name<<endl;
    ofs<<"Last Name: "<<last_name<<endl;
    for(int i=0;i<5;i++)
    {
	ofs<<doubleArray[i]<<endl;
	total = total + doubleArray[i];
    }
    ofs<<total;

    ofs.close();

    return 0;
}

//Splits an array of strings into two smaller string arrays,
//divided when the ' ' character is found.
void parse_name(char full[], char first[], char last[])
{
    for(int i=0;i<strlen(full);i++)
    {
	if(full[i] == ' ')
	{
	    int k = 0;
	    for(int j=i+1;j<strlen(full);j++)
	    {
		last[k] = full[j];
		k++;
	    }
	    break;
	}
	first[i] = full[i];
    }
}
