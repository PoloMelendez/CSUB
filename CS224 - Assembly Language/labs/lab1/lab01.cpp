// filename: lab01.cpp
// your job is to code this program in assembly
 
#include <iostream>
using namespace std;

char str1[20] = "The sum of ";
char str2[20] = " and ";
char str3[20] = " is ";
char str4[20] = ".\n";
int x = 5;
int y = 15;

int main()
{
   int z = x + y;
   cout << str1; 
   cout << x; 
   cout << str2; 
   cout << y; 
   cout << str3; 
   cout << z;
   cout << str4; 

   return 0;
}
