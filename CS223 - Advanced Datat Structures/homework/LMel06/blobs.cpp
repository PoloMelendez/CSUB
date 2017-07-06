#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <string>
#include "Menu.h"
#include <ctime>
#include <time.h>
using namespace std;

const int NUMOFROWS = 5;
const int NUMOFCOLUMNS = 13;
const int MAXBLOBSIZE = 33;

//for random do rows * cols * percent / 100.00

void clearBlob(char ba[][NUMOFCOLUMNS]);
void checkBlob(char ba[][NUMOFCOLUMNS]);
void Path(char ba[][NUMOFCOLUMNS], int R, int C);
void enterAsterisks(char ba[][NUMOFCOLUMNS]);
void display(char ba[][NUMOFCOLUMNS]);
void randomAsterisks( char ba[][NUMOFCOLUMNS]);

int main()
{
    srand( (unsigned)(time(NULL) ) );
    const char * testMenu[] = {
	"+=========================================+",
	"|             Blob Counter                |",
	"+=========================================+",
	"| F/f: Fill the grid with asterisks       |",
	"|      manually.                          |",
	"+-----------------------------------------+",
	"| S/s: Show the grid                      |",
	"+-----------------------------------------+",
	"| R/r: Fill a percentage of the grid      |",
	"|      randomly.                          |",
	"+-----------------------------------------+",
	"| C/c: Calculate the number of blobs      |",
	"|      and clear the grid.                |",
	"+-----------------------------------------+",
	"| D/d: Change the dimensions of the grid  |",
	"+-----------------------------------------+",
	"| H/h/?: Show this menu.                  |",
	"+-----------------------------------------+",
	"| Q/q: Exit program.                      |",
	"+-----------------------------------------+",
	NULL
    };

    char choice = ' ';
    Menu m( testMenu );

    char blob[NUMOFROWS][NUMOFCOLUMNS];
    clearBlob(blob);

    while( true ) {
	choice = m.prompt("Enter a command, Q to quit, H for help: ");
	cout << endl;

	switch( toupper(choice) ){
	    case ' ' : break;
	    case 'H' : m.show(); break;
	    case 'Q' : exit(0);
	    case 'F' :
		       clearBlob(blob);
		       enterAsterisks(blob);
		       cin.ignore();
		       break;
	    case 'S' :
		       display(blob);
		       break;
	    case 'R' :
		       clearBlob(blob);
		       randomAsterisks(blob);
		       break;
	    case 'C' :
		       checkBlob(blob);
		       break;
	    case 'D' :
		       break;
	    default:
		       cout << "\t\tIllegal selection.\n";
	}
    } 
    return 0;
}

void randomAsterisks( char ba[][NUMOFCOLUMNS]) {
    int col = 1, row = 1;
    double percent = 0.0;
    cout << "\tWhat percent of the grid do you wish to fill? : ";
    do {
	cin >> percent;
    } while( percent <= 0.0 || percent > 100.0 );
    int numAsterisk = 3 * 11 * percent / 100.0;
    for( int i = 0; i < numAsterisk; i++) {
	col = rand() % 11 + 1;
	row = rand() % 3 + 1;
	ba[row][col] = '*';

    }
    cin.ignore();
}
void enterAsterisks(char ba[][NUMOFCOLUMNS]) {
    int col = 1;
    int row = 1;
    int cnt = 0;
    cout << "\tHow many asterisks do you want to input? : ";
    cin >> cnt;
    for(int i = 0; i < cnt; i++) {
	do {
	    cout << "\tEnter a row number : ";
	    cin >> row;
	} while( row <= 0 || row > 3 );
	do {
	    cout << "\tEnter a column number : ";
	    cin >> col;
	} while( col <= 0 || col > 11 );
	cout << endl;
	ba[row][col] = '*';
    }
}

void display(char ba[][NUMOFCOLUMNS] ) {
    cout << "\t   1    2    3    4    5    6    7    8    9    10   11 " << endl;
    cout << "\t =======================================================" << endl;
    for(int i = 1; i < 4; i++) {
	cout << "\t" << i;
	for(int j = 1; j < 12; j++) {
	    cout << "[ " <<ba[i][j] << " ]";
	}
	cout << endl << endl;
    }
}

void clearBlob(char ba[][NUMOFCOLUMNS])
{
    for(int row = 0; row < NUMOFROWS; row++)
    {
	for (int column = 0; column < NUMOFCOLUMNS; column++)
	{
	    ba[row][column] = ' ';
	}
    }
}

void checkBlob(char ba[][NUMOFCOLUMNS])
{
    int blobcount = 0;

    for (int row = 1; row < NUMOFROWS; row++)
    {
	for (int column = 1; column < NUMOFCOLUMNS; column++)
	{
	    if (ba[row][column] == '*')
	    {
		Path(ba, row, column);

		cout << "\tBlob at row: " << row;
		cout << " \tcolumn: " << column << endl;
		blobcount++;
	    }
	}
    }

    cout << "\tNumber of blobs: " << blobcount << endl;
}

void Path(char ba[][NUMOFCOLUMNS], int R, int C)
{
    ba[R][C] = 'X';
    if (ba[R][C+1] == '*')
    {
	Path(ba, R, C+1);
    }
    if (ba[R+1][C] == '*')
    {
	Path(ba, R+1, C);
    }
    if (ba[R][C-1] == '*')
    {
	Path(ba, R, C-1);
    }
    if (ba[R-1][C] == '*')
    {
	Path(ba, R-1, C);
    }
    else
	return;
}
