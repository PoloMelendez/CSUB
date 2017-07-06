//Leopoldo Melendez
//MidTerm Problem 2
//11-7-2013
#include<iostream>
#include<fstream>
using namespace std;

bool headerprompt(string);

int main()
{
    string file;
    string cstdlib = "cstdlib";
    string cstring = "cstring";
    string fstream = "fstream";
    string cmath = "cmath";
    string iomanip = "iomanip";
    bool choice1, choice2, choice3, choice4, choice5;

    cout<<"What is the name of your target source file? ";
    cin>>file;
    
    choice1 = headerprompt(cstdlib);
    choice2 = headerprompt(cstring);
    choice3 = headerprompt(fstream);
    choice4 = headerprompt(cmath);
    choice5 = headerprompt(iomanip);

    ofstream ifs(file.c_str());
    ifs<<"#include<iostream>"<<endl;
    if(choice1 == true)
	ifs<<"#include<cstdlib>"<<endl;
    if(choice2 == true)
	ifs<<"#include<cstring>"<<endl;
    if(choice3 == true)
	ifs<<"#include<fstream>"<<endl;
    if(choice4 == true)
	ifs<<"#include<cmath>"<<endl;
    if(choice5 == true)
	ifs<<"#include<iomanip>"<<endl;
    ifs<<endl<<"using namespace std;"<<endl;
    ifs<<endl<<"int main()"<<endl;
    ifs<<"{"<<endl<<endl;
    ifs<<"\treturn 0;"<<endl;
    ifs<<"}";
    ifs.close();

    cout<<"Your file "<<file<<" is ready."<<endl;


    return 0;
}

bool headerprompt(string header)
{
    char selection;
    bool choice;
    cout<<"include "<<header<<"? ";
    cin>>selection;
    if(selection == 'y')
	choice = true;
    else if(selection == 'n')
	choice = false;

    return choice;
   }
