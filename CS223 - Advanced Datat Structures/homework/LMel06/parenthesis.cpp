#include<iostream>
#include<string>

using namespace std;

char * par( char * str );

int main() {

    char * str = new char[50];
    cout << "Enter a combination of '( and ')' : ";
    cin.getline(str, 50);
    char * temp = par( str );    
    if( *temp == '\0' )
	cout << "This combination is balanced." << endl;
    else
	cout << "This combination is not balanced." << endl;

    return 0;
}

char *par( char *str ) {
        if( *str == '\0' || *str == ')' ) { return str; }
        if( *str == '(' )
        {
                char *stop = par(++str);
                if( *stop == ')' )
                {
                        return par(++stop);
                }
                return str - 1;
        }

        return par(++str);
}
