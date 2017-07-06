#include<iostream>
#include<cstdlib>
#include<ctime>

using namespace std;

int main() {

    srand(time(NULL));
    int r1, r2, r3;
    double s = 0;
    double E = 0;
    int i = 0;
    int limit;

    cout << "Gimme the limit: " << endl;
    cin >> limit;

    do {
        s++;
        r1 = rand() % 26;
        r2 = rand() % 26;
        r3 = rand() % 26;

        do {
            r2 = rand() % 26;
            r3 = rand() % 26;
        } while( r1 == r2 || r2 == r3 || r3 == r1);

        int diff1 = abs(r1 - r2);
        int diff2 = abs(r1 - r3);
        int diff3 = abs(r2 - r3);
        if((diff1 == 1 && (diff2 == 1 || diff3 == 1)) ||
               (diff2 == 1 && (diff1 == 1 || diff3 == 1)) ||
               (diff3 == 1 && (diff1 == 1 || diff2 == 1))
          ) {
            int row1 = r1 / 7;
            int row2 = r2 / 7;
            int row3 = r3 / 7;
            if(row1 == row2 && row2 == row3) {
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
