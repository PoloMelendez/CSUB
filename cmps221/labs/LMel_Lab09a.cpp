//Leopoldo Melendez
//Lab 09a
//10-24-2013
#include<iostream>
#include<cctype>
#include<cstdlib>
#include<string.h>
using namespace std;

bool ispalindrome(char []);

int main()
{
    char input[50];
    cout<<"Enter a string: ";
    cin.getline(input, 50);
    
    if(ispalindrome(input))
	cout<<input<<" is a palindrome"<<endl;
    else
	cout<<input<<" is not a palindrome"<<endl;

    return 0;
}

bool ispalindrome(char input[])
{
    int j=strlen(input) - 1;
    bool isPalindromic;
   
    for(int i=0;i<=strlen(input);i++)
    {
	if(input[i] == input[j])
	    isPalindromic = true;
	else
	{
	    isPalindromic = false;
	    break;
	}
	j--;
    }
    return isPalindromic;
}
	    

