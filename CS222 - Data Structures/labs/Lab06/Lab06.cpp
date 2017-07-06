#include<iostream>
#include<cstdlib>

using namespace std;

char* getAndCopy(char *str, int max);

int main()
{
    char line[121];
    char *copy = NULL;

    copy = getAndCopy(line, 120);

    cout << "Pointer information.\n";
    cout << "Address of line: " << &line << endl;
    cout << "Address of copy: " << &copy << endl;
    cout << "Base address of line pointed to by copy: " << &(copy[0]) << endl;

    cout << "\nContents of the strings:\n";
    cout << "Line: " << line << endl;

    if(copy == NULL)
    {
	cout << "Copy failed!\n";
    }
    else
    {
	cout << "Copy: " << copy << endl;
	delete [] copy;  // Deallocate the string
    }

    return 0;
}

char* getAndCopy(char *str, int max){
    char c;
    char * tmp;
    int count = 0;

    cin.get(c);
    while( c != '\n' && count < max-1 ){
	str[count] = c;
	count++;
	cin.get(c);
    }
    str[count] = '\0';
    try{
	tmp = new char[count + 1];
    } catch(bad_alloc ex) {
	cout << "Exception: " << ex.what() << endl;
	return NULL;
    }
    for( int i = 0; i < count;i++)
	tmp[i] = str[i];
    tmp[count + 1] = '\0';
    return tmp;
}
