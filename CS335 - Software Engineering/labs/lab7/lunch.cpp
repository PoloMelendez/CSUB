/*
*Author: Greg Hefley
* Lab  07
* 
*The purpose of this code it to select where I should go for lunch 
*/


#include<iostream>
#include<string>
#include "restaurants.h"

ofstream LOGFILE;

int main()
{
    LOGFILE.open("Lab07.log",ios::trunc);
    if(LOGFILE.is_open())
    {
	LOGFILE << POS_IN_PROGRAM <<"start of logging" << endl;
    }
    else
    {
	cout<<"Unable to open file";
	return 1;
    }

    cout<<"\n\n";
    restaurants r1;
    r1.run();

    cout<<"You should go to " << r1.selection() << "\n";

    return 0;
}




