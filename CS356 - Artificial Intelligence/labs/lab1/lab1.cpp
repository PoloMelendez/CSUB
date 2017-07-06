#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include<cstdlib>

#define MIN 1
#define MAX 3

using namespace std;

void displayResults(vector <vector <string> > dataSet);
void displayPredicted(vector<int> pred);
void exportResults(vector <vector <string> > dataSet, vector<int> prediction, char * outName);
double calcPercentCorrect(vector <vector <string> > dataSet, vector<int> prediction);
double calcPredicted(vector <vector <string> > dataSet, vector<int>& predicted, double threshold);

int main(int argc, char *argv[]) {

    if( argc < 2 ) {
        cout << "Usage: ./lab1 <inFile> <outFile>" << endl;
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

    vector< int >  predicted;
    double threshold = MIN;
    double bestThreshold = MIN;
    double bestPercent = 0;
    while(threshold < MAX) {
        predicted.clear();
        double currentPercent = calcPredicted(dataSet, predicted, threshold);
        if(currentPercent > bestPercent) {
            bestPercent = currentPercent;
            bestThreshold = threshold;
            cout << "Percent: " << bestPercent << " Threshold: " << bestThreshold << endl;
        }
        threshold += 0.1;
    }
    
    exportResults(dataSet, predicted, argv[2]);
}

double calcPredicted(vector <vector <string> > dataSet, vector<int> &predicted, double threshold) {
    for(int i=0; i<dataSet.size(); i++) {
        if(atof((dataSet[i][0]).c_str()) > threshold) {
            predicted.push_back(2);
        }
        else {
            predicted.push_back(1);
        }
    }
    return calcPercentCorrect(dataSet, predicted);
}

void exportResults(vector <vector <string> > dataSet, vector<int> prediction, char * outName) {
    ofstream out;
    out.open(outName);
    for(int i=0; i<dataSet.size(); i++) {
        string tmp = to_string(prediction[i]) + " " + dataSet[i][1];
        out << tmp << endl;
    }
    out.close();
}

double calcPercentCorrect(vector <vector <string> > dataSet, vector<int> prediction) {
    double correct = 0, total = dataSet.size();
    for(int i=0; i<total; i++) {
        if(atof(dataSet[i][1].c_str()) == prediction[i]) {
            correct++;
        }
    }
    return correct / total;
}

void displayResults(vector <vector <string> > dataSet) {
    for(int i=0; i<dataSet.size(); i++) {
        for(int j=0; j<dataSet[i].size(); j++) {
            cout << dataSet[i][j] << " ";
        }
        cout << endl;
    }
}

void displayPredicted(vector<int> pred) {
    for(int i=0; i<pred.size(); i++) {
        cout << pred[i] << endl;
    }
}
