#include<iostream>

using namespace std;

int main() {

    int total = 0;

    int arr[] = {0,2,4,65,34,76,56,54,34,32,12,65,978,56,4,3,56,4,12,3,42,72,33};
    int n = 22;
    bool swapped;

    for(int i=1; i < n; i++) {
        swapped = false;
        for(int j= n-1; j > i+1; j++) {
            if(arr[j] < arr[j-1]) {
                int temp = arr[j];
                arr[j] = arr[j-1];
                arr[j-1] = temp;
                swapped = true;
                total++;
            }
        }
        if(swapped == false)
            break;
    }

    cout << total << endl;

    return 0;
}
