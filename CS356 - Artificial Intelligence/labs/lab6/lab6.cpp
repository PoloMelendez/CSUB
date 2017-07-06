#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include<cstdlib>
#include"Neuron.h"

using namespace std;

void test(Neuron* neuron, vector< vector<double> > input, vector<double> Yd);
void train(Neuron* neuron, vector< vector<double> > input, vector<double> Yd);
void vecDisplay(vector<double> vec);

int main(int argc, char *argv[]) {

    if(argc < 1) {
        cout << "Usage: ./lab6 <epoch cnt> <inFile>" << endl;
        return 0;
    }
    //Read iris data into 2D vector
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
    //Convert 2D string vector into doubles, split off the Yd
    vector< vector<double> > input(dataSet.size());
    vector<double> Yd;
    for( int i = 0; i < dataSet.size(); i++) {
        Yd.push_back(stod(dataSet[i][dataSet[i].size()-1]));
        for( int j = 0; j < dataSet[i].size()-1; j++) {
            input[i].push_back(stod(dataSet[i][j]));
        }
    }

    //Create neuron, test initial weights, train, test again
    Neuron* myNeuron = new Neuron();
    myNeuron->initializeWeights(4);
    test(myNeuron, input, Yd);
    train(myNeuron, input, Yd);
    test(myNeuron, input, Yd);

    return 0;
}
//  Number of iterations with 0 ERROR  /  Total number of iteratoins
void test(Neuron* neuron, vector< vector<double> > input, vector<double> Yd) {
    double correct = 0, total = input.size();
    for(int i=0; i<input.size(); i++) {
        neuron->feed_forward(input[i], Yd[i]);
        if(neuron->error == 0)
            correct++;
    }
    cout << "The percent correct is: " << correct/total * 100 << endl;
}

//Update weights for each iteration, for specified number of epochs
void train(Neuron* neuron, vector< vector<double> > input, vector<double> Yd) {
    int epoch = 3;
    for(int j = 0; j < epoch; j++) {
        for(int i = 0; i < input.size(); i++) {
            neuron->feed_forward(input[i], Yd[i]);
            neuron->updateWeights();
            cout << "Iteration " << j*input.size() + i << ": Weights are [";
            for(double d : neuron->weights)
                cout << d << ", ";
            cout << "], classification was " << 
                ((neuron->error) ? "an ERROR!" : "CORRECT!") << endl;
        }
    }
}

void vecDisplay(vector<double> vec) {
    for(double d : vec) 
        cout << d << " ";
    cout << endl;
}
