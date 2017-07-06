#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>

using namespace std;

void display(vector <vector <string> > dataSet);

int main(int argc, char *argv[]) {

    if( argc < 2 ) {
        //usage
        cout << "error" << endl;
        return 0;
    }
    
    vector <vector <string> > dataSet;
    
    ifstream inFile;
    inFile.open(argv[1]);
    while(inFile) {
        string temp;
        if(!getline( inFile, temp)) break; //error
        istringstream ss( temp );
        vector<string> tmp;
        while( ss ) {
            string s;
            if(!getline( ss, s, ',' )) break; //error
            tmp.push_back(s);
        }
        dataSet.push_back(tmp);
    }
    inFile.close();

    //display(dataSet);

    ofstream outFile;
    //outFile.open(argv[2]);
}

void display(vector <vector <string> > dataSet) {
    for(int i=0; i<dataSet.size(); i++) {
        for(int j=0; j<dataSet[i].size(); j++) {
            cout << dataSet[i][j] << " ";
        }
        cout << endl;
    }
}
