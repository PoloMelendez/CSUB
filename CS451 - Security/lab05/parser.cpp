#include<iostream>
#include "string.h"

using namespace std;

char escape(char);
char* newString(char*);
char* decode(char*);
char encode(char*);

int main() {

    char buff[1000];
    cout << "Input: ";
    cin.getline(buff, 1000);

    char* modified = newString(buff);
    cout << modified << endl;

    return 0;
}

char escape(char c) {
    switch(c) {
        case '\n': return 'n';
        case '\r': return 'r';
        case '\t': return 't';
        case '&': return '&';
        case ';': return ';';
        case '`': return '`';
        case '\'': return '\'';
        case '\\': return '\\';
        case '"': return '"';
        case '|': return '|';
        case '*': return '*';
        case '?': return '?';
        case '~': return '~';
        case '<': return '<';
        case '>': return '>';
        case '^': return '^';
        case '(': return '(';
        case ')': return ')';
        case '[': return '[';
        case ']': return ']';
        case '{': return '{';
        case '\0': return '0';
        case '}': return '}';
        case '$': return '$';
        case '#': return '#';
        case '-': return '-';
        case '\v': return 'v';
        case '\f': return 'f';
        case '\0': return '0';
        case '!': return '!';
    }
    return 0;
}

char* newString(char *str) {
    int newLen = 0, length = strlen(str), iterator = 0;
    for(int i=0; i < length; i++) {
        if(escape(str[i]))
            newLen++;
    }
    char* newString = new char[length + newLen];
    for(int i=0; i<length; i++) {
        char escaped = escape(str[i]);
        if(escaped) {
            newString[iterator++] = '\\';
            newString[iterator++] = escaped;
        }
        else
            newString[iterator++] = str[i];
    }
    return newString;
}

/*
char encode(char* str) {
}

char* decode(char *str) {
    int i, j, k;
    bool found;
    int length = strlen(str), subLength = 0;
    char tmp[4], tmp2;
    for(i=0; i<length; i++) {
        if(str[i] == '%') {
            found = false;
            k = 0;
            for(j=i+1; j<i+3; j++) {
                if(isxdigit(str[j])) {
                    found = true;
                    tmp[k] = str[j];
                    k++;
                }
                else {
                    found = false;
                    k = 0;
                    break;
                }
                if(found) {
                    tmp2 = encode(tmp);
                    str[i] = tmp2;
                    str[i+1] = ' ';
                    str[i+2] = ' ';
                }
            }
        }
    }
    return str;
}
*/
