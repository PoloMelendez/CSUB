/* Hanoi Towers2:
	* Use the stack to simulate the disks.
	* Define 3 stacks as global so that the order of poles printed will not
	  change as the order of stacks in the call to move changes.
*/

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "stack_tpl.h"
using namespace std;

IntStack s1, s2, s3;

void displayDisks(int k)
{
	int sz1 = s1.getSize(), sz2 = s2.getSize(), sz3 = s3.getSize();
	int max ;
	max  = sz1>=sz2 ? (sz1>=sz3 ? sz1:sz3) : (sz2>=sz3 ? sz2 : sz3);
	IntStackNode * p1 = s1.top, *p2 = s2.top, * p3 = s3.top;

	cout << "\n\n\n";
	while ( max ) {
		if ( sz1 == max ) {
			if ( p1 ) {
				cout << setw(10) << p1->info;
				p1 = p1->next;
				sz1 --;
			}
		} else cout << "          ";
		if ( sz2 == max ) {
			if ( p2 ) {
				cout << setw(10) << p2->info;
				p2 = p2->next;
				sz2 --;
			}
		} else cout << "          ";
		if ( sz3 == max ) {
			if ( p3 ) {
				cout << setw(10) << p3->info;
				p3 = p3->next;
				sz3 --;
			}
		} else cout << "          ";
		max --;
		cout << endl;
	}
	cout << "       -----     " << "-----     " << "-----\n" ;
	cout << "Step: " << k << "  Press RETURN to continue ... ";
	char s[5];
	cin.getline(s, 5);

}


void move( int n, IntStack &s1, IntStack &s3, IntStack &s2, int & i)
{
	if ( n ) {
     	// move n -1 from pole 1 to pole 2 using pole 3 as temporary pole.
		 move( n - 1, s1, s2, s3, i) ;
		int t ;
		s1.pop( t );
		s3.push( t );
		displayDisks(++i) ;
		// move n - 1 disk from pole2 to pole 2 using pole 1 as temporary pole.
      	move ( n - 1, s2, s3, s1, i);
    }

}

int main()
{
	int diskCnt;
	char s[5];
	// ask for number of disks to move.
	cout << "Enter the number of towers: ";
	cin.getline(s, 5);
	diskCnt = atoi(s);

	for ( int i = diskCnt; i > 0; i -- )
		s1.push(i);

	// from N disks from pole 1 to pole 3 using pole 2 as temporary pole.
	int step = 0;
	displayDisks(step) ;
	move(diskCnt, s1, s3, s2, step);
}
