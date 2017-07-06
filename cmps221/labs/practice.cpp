//Leopoldo Melendez
//Lab or HW #
//Date
#include<iostream>
#include<cctype>//for using the isalpha() isdigit() etc
#include<cstdlib>//c-strings
#include<cstring>//normal strings
#include<limits>
using namespace std;
int main()
{
    int numbers[3][3]={1,2,3,4,5,6,7,8,9};

    cout<<numbers[2][2]<<endl;

    string names[3][3]={"polo","asd","dsa","fgh","hgf","hjk","yui","iuy","qwer"};

    cout<<names[2][2][3]<<endl;


    return 0;
}
