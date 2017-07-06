//Leopoldo Melendez
//Lab 09b 
//10-24-2013
#include<iostream>
#include<cctype>
#include<cstdlib>
#include<cstring>
using namespace std;

void decodetext(char [], char []);

int main()
{
    //Asks the user to enter a phone number and stores it
    //as a string. Calls the decodetext function and then
    //outputs the new string.
    char before[50];
    cout<<"Enter a phone number: ";
    cin.getline(before, 50);
    char after[50];
    decodetext(after, before);
    cout<<after<<endl;

    return 0;
}

//Goes through each index of the given string and checks
//to see if that index holds a number, hyphen, or letter character.
//If the character is a letter, goes through the switch statement
//and switches that letter with a number in a new string.
void decodetext(char after[], char before[])
{
    for(int i=0;i<strlen(before);i++)
    {
	if(isdigit(before[i]) || before[i]=='-')
	    after[i]=before[i];
	else if(isalpha(before[i]))
	{
	    switch(before[i])
	    {
		case 'a':
		case 'b':
		case 'c':
		case 'A':
		case 'B':
		case 'C':
		    after[i]='2';
		    break;
		case 'd':
		case 'e':
		case 'f':
		case 'D':
		case 'E':
		case 'F':    
		    after[i]= '3';
		    break;
		case 'g':
		case 'G':
		case 'h':
		case 'H':
		case 'i':
		case 'I':
		    after[i]= '4';
		    break;
		case 'j':
		case 'J':
		case 'k':
		case 'K':
		case 'l':
		case 'L':
		    after[i]='5';
		    break;
		case 'm':
		case 'M':
		case 'n':
		case 'N':
		case 'o':
		case 'O':
		    after[i]='6';
		    break;
		case 'p':
		case 'P':
		case 'q':
		case 'Q':
		case 'r':
		case 'R':
		case 's':
		case 'S':
		    after[i]='7';
		    break;
		case 't':
		case 'T':
		case 'u':
		case 'U':
		case 'v':
		case 'V':
		    after[i]='8';
		    break;
		case 'w':
		case 'W':
		case 'x':
		case 'X':
		case 'y':
		case 'Y':
		case 'z':
		case 'Z':
		    after[i]='9';
		    break;
	    }
	}
	else
	    after[i]=' ';
    }
}    

