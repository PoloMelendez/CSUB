#include<iostream>

using namespace std;

bool isPalindrome(int);

int main()
{
    int palindrome, largest;

    for(int i=999;i>=100;i--)
    {
	for(int j=999;j>=100;j--)
	{
	    palindrome = i*j;
	    if(isPalindrome(palindrome) && palindrome>=largest)
	    {
		largest = palindrome;
		cout<<largest<<endl;
	    }
	}
    }
    return 0;
}

bool isPalindrome(int num)
{
    bool isPalindromic = true;
    int digit, reverse = 0, checknum;
    checknum = num;

    while(num!=0)
    {
	digit = num%10;
	num /= 10;
	reverse = reverse*10 + digit;
    }

    if(reverse == checknum)
	isPalindromic = true;
    else
	isPalindromic = false;
    return isPalindromic;
}
