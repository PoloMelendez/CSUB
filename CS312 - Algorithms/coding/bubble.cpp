#include<iostream>
#include<ctime>
#include<cstdlib>
#include<cmath>
#include<unistd.h>

using namespace std;

int bubble(int*, int);
void printArr(int*, int, int);

int total = 0;

int main() {

    srand(time(NULL));

    int arr[] = {2,4,65,34,76,56,54,34,32,12,65,978,56,4,3,56,4,12,3,42,72,33};
    int n=22;
    //for(int i=0; i<n; i++)
      //  arr[i] = rand() % 100;

    bubble(arr, n);
    cout << total << endl;

    return 0;
}

int bubble(int* arr, int n) {
    for(int i=0; i<n-1; i++) {
        bool swapped = false;
        for(int j=0; j<n - 1; j++) {
            if (arr[j + 1] < arr[j]) {
                printArr(arr, n, j);
                total++;
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j+1] = tmp;
                swapped = true;
                usleep(10);
                printArr(arr, n, j);
            }
        }
        if(swapped == false)
            break;
    }
}

void printArr(int* arr, int n, int place) {
    system("clear");
    for(int i=0; i<n; i++) {
        for(int j=0; j<arr[i]; j++) {
            cout << "*";
        }
        cout << endl;
    }
}
