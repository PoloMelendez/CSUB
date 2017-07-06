#include<iostream>
#include<iomanip>
using namespace std;

template< class T >
double calcAverage( T arr[], int size ){
    double avg;
    T sum;
    for(int i=0; i < size; i++){
	sum += arr[i];
    }
    avg = (double)sum / size;
    return avg;
}

template< class T >
void printArray( T arr[], int size ){
    for(int i=0; i < size; i++){
	cout << "[ " << arr[i] << " ]";
    }
    cout << endl;
}

void printArray( double arr[], int size ){
    for(int i=0; i < size; i++){
	cout << setprecision(3) << fixed << "[ " << arr[i] << " ]";
    }
    cout << endl;
}


int main(){
    
    int iArr[] = {4, 7, 19, 71, 55};
    char cArr[] = {'a', 'y', '%', '!', 'T'};
    double dArr[] = {8.3, 7.2, 19.65, 2.5, 15.27};

    printArray(iArr, 5);
    cout << "The average is " << calcAverage(iArr, 5) << endl;

    printArray(dArr, 5);
    cout << "The average is " << calcAverage(dArr, 5) << endl;

    printArray(cArr, 5);

    return 0;
}
