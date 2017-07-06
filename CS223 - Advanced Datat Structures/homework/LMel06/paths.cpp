#include<iostream>
#include<cstring>
#include<string.h>
using namespace std;

//const int NUMOFROWS = 4;
//const int NUMOFCOLUMNS = 7;

//void clearGrid( char grd[][NUMOFCOLUMNS]); 
int paths(int x, int y, int column, int row);

int main() {

  //  char grid[NUMOFROWS][NUMOFCOLUMNS];
  //  clearGrid(grid);
    int colA, rowA, colB, rowB;
    cout << "Enter the column for point A : ";
    cin >> colA;
    cout << "Enter the row for point A : ";
    cin >> rowA;
    cout << "Enter the column for point B : ";
    cin >> colB;
    cout << "Enter the row for point B : ";
    cin >> rowB;
    int numPaths = paths(colA, rowA, colB, rowB);
    cout << "The total number of north east paths is : " << numPaths << endl;

    return 0;
}
/*
void clearGrid( char grd[][NUMOFCOLUMNS]) {
    for(int row = 0; row < NUMOFROWS; row++) {
	for(int column = 0; column < NUMOFCOLUMNS; column++) {
	    grd[row][column] = ' ';
	}
    }
}
*/
int paths(int x, int y, int column, int row) {
    int count = 0;
    
    if(x == column && y == row)
	return 1;
    if(x != column)
	count += paths( x + 1, y, column, row );
    if(y != row)
	count += paths( x, y + 1, column, row );
    return count;
}
