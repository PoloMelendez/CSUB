//Leopoldo Melendez
//i
//HW 05
//10-28-2013
#include<iostream>
#include<cctype>
#include<cstdlib>
#include<cstring>
#include<limits>
using namespace std;

int min_uppers, min_lowers, pwd_length;

int get_int(char [], int, int);
int validate_password(char []);

int main()
{
    char prompt1[] = "How many upper case numbers are required (1-3)? ";
    char prompt2[] = "How many lower case numbers are required (1-3)? ";
    char prompt3[] = "How long should the password be (6-20)? ";
    char password[21];

    min_uppers = get_int(prompt1, 1, 3);
    min_lowers = get_int(prompt2, 1, 3);
    pwd_length = get_int(prompt3, 6, 20);
    cout<<endl<<"Ok..."<<endl;

    cout<<"Enter your password please: ";
    cin.ignore();
    cin.getline(password, 100);
    
    int value = validate_password(password);

    while(value!=0)
    {
	switch(value)
	{
            case 1:
		cout<<"That password is invalid, you must have at least "<<min_uppers<<" upper case characters."<<endl;
		break;
	    case 2:
		cout<<"That password is invalid, you must have at least "<<min_lowers<<" lower case characters."<<endl;
		break;
	    case 3:
		cout<<"Your password is too short..."<<endl;
	        break;
	    case 4:
		cout<<"Your password is too long..."<<endl;
		break;
	}
	cout<<"Enter your password: ";
	cin.getline(password, 100);
	value = validate_password(password);
    }

    cout<<"That password is OK"<<endl;

    return 0;
}

int get_int(char prompt[], int min, int max)
{
    int input = 0;
    while(input<min || input>max)
    {
	cout<<prompt;
	cin>>input;
    }
    return input;
}

int validate_password(char pwd[])
{
    int uppers=0, lowers=0;
    
    for(int i=0;i<strlen(pwd);i++)
    {
	if(isupper(pwd[i]))
	    uppers++;
	else if(islower(pwd[i]))
	    lowers++;
    }
    
    if(uppers<min_uppers)
	return 1;
    else if(lowers<min_lowers)
	return 2;
    else if(strlen(pwd)<pwd_length)
	return 3;
    else if(strlen(pwd)>pwd_length)
	return 4;
    else
	return 0;
}
