#include<iostream>
#include<cstring>
#include<cstdio>
#include<iomanip>

using namespace std;

int main() {
    char password[] = "UHYHUVLQJ IRU QRRE].\0";
    char input[29];

    printf("PASSWORD: ");
    scanf("%[^\n]19c", input);
    //scanf("%19c", input);

    for(int i = 0; password[i] != '\0'; i++) {
        if(password[i] != ' ' && password[i] != '.')
            password[i] -= 3;
    }

    if( strncmp( password, input, 19) == 0 )
        cout << "Good" << endl;
    else
        cout << "Bad" << endl;

    return 0;
}
