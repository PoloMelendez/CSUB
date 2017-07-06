//Leopoldo Melendez
//Lab or HW #
//Checks for largest palindrome that is the product of 2 3 digit numbers
#include<iostream>
using namespace std;
int main()
{
    int n, reverse=0, rem,temp;
    int largestPalindrome;

    for(int i=999;i>=100;i--)
    {
	for(int j=999;j>=100;j--)
	{
	    n = i*j;
	    temp = n;
	    while(temp!=0)
	    {
	        rem=temp%10;
	        reverse=reverse*10+rem;
	        temp/=10;
            }  
  
    	    if(reverse==n && reverse>largestPalindrome)
	    {
	        cout<<largestPalindrome;
		largestPalindrome = reverse;
	    }
	}
    }

    cout<<largestPalindrome<<endl;


    return 0;
}
