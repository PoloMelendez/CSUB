#include<iostream>
#include<cstring>

using namespace std;

int main() {

    char str[256];
    cout << "Enter a string: ";
    cin >> str;

    int length = strlen(str);
    int arr[256];
    memset(arr, -1, 256);

    for(int i=0; i<length; i++) {
        if( arr[str[i]] >= 0 ) {
            //duplicate
            int index = arr[ str[i] ];
            char *p = str + index + 1;
            while( *p == '\0')
                p++;
            if (*p > str[i]) {
                str[i] = '\0';
                arr[str[i]] = index;
            }
            else {
                str[index] = '\0';
                arr[str[i]] = i;
            }
        }
        else {
            arr[str[i]] = i;
        }
    }

    for(int i=0; i<length; i++) {
        if(str[i] != '\0')
            cout << str[i];
    }
    cout << endl;

    return 0;
}
