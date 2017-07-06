#include<iostream>
#include<cstring>
#include<stdio.h>

using namespace std;

bool isNumber ( const char * s ) {
    for(int i = 0; s[i] != '\0'; i++)
	if( !isdigit( s[i]) ) return false;
    return true;
}

int main() {
    
    char s[101];

    cout << "Enter a string: ";
    cin.getline(s, 101);

    printf("String read: %s\n",s);

    char *pch = strtok(s, " ");

    while ( pch != NULL ) {
	if( isNumber( pch ) ) cout << pch << endl;
	pch = strtok( NULL, " ");
    }

    return 0;
}
