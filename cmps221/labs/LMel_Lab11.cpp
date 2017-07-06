//Leopoldo Melendez
//Lab 11
//11-12-13
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<fstream>
using namespace std;

//
//Structure that holds two strings and an int for keeping track of airport
//information and to fill up a structure array.
struct information
{
    string code;
    string city;
    int terminals;
};

int main()
{
    string input;

    //Creates an array of size 10, fills the array up with the contents of
    //the airports.txt file.
    information airports[10];

    ifstream ifs("airports.txt");
    for(int i=0;i<5;i++)
    {
	ifs>>airports[i].code>>airports[i].city>>airports[i].terminals;
    }

    ifs.close();

    //Asks the user for an airport code and goes through the array
    //to find a match. If a match is found the remaining information
    //is given, if it is not found an error message is output.
    //Repeats until the user enters Quit.
    while(input!="QUIT" && input!="quit" && input!= "Quit")
    {
    cout<<"What is the airport code? ";
    cin>>input;

    bool exists;
    for(int i=0;i<5;i++)
    {
	if(airports[i].code==input)
	{
	    cout<<airports[i].code<<" in "<<airports[i].city<<" has "<<airports[i].terminals<<" terminals"<<endl;
	    exists = true;
	    break;
	}
	if(input == "quit" || input == "Quit" || input == "QUIT")
	{
	    exists = true;
	    break;
	}
	else
	    exists = false;
    }

    if(exists == false)
	cout<<"Oops - couldn't find "<<input<<endl;
    }

    return 0;
}
