#include<iostream>
#include<cstdlib>
#include<ctime>

using namespace std;

int main() {

    srand(time(NULL));
    int r1, r2;
    double s = 0;
    double E = 0;
    int i = 0;
    int limit;

    cout << "Gimme the limit: " << endl;
    cin >> limit;

    do {
        s++;
        r1 = rand() % 35;
        r2 = rand() % 35;

        do {
            r2 = rand() % 35;
        } while( r1 == r2 );

        int diff = abs(r1 - r2);
        if(diff == 1) {
            int row1 = r1 / 15;
            int row2 = r2 / 15;
            if(row1 == row2) {
                E++;
            }        
        }
        i++;
    } while ( i < limit );

    cout << "Count " << s << endl;
    cout << "Success " << E << endl;
    cout << "Probability is " << E / s << endl;

    return 0;
}
