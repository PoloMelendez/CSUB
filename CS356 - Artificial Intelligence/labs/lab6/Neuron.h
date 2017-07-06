#ifndef NEURON_H
#define NEURON_H
#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<vector>

using namespace std;

class Neuron {
    public:
        double alpha = 0.05;
        double theta;
        double error;
        vector<double> input;
        vector<double> weights;
        
        Neuron() {
            srand(time(NULL));
            theta = 1 - ((double)(rand()%200000) / 100000);
            error = 0;
        }

        void initializeWeights(int i) {
            weights.reserve(i);
            for(int j=0; j<i; j++) {
                weights.push_back(0);
            }
            randomizeWeights();
        }

        void feed_forward(vector<double> input2, double Yd) {
            input = input2;
            double X = dot(input, weights);
            double Y = activate(X);
            error = Yd - Y;
        }

        double dot(vector<double> X, vector<double> W) {
            double scalar = 0;
            for(int i = 0; i < X.size(); i++) {
                scalar += X[i] * W[i];
            }
            return scalar;
        }

        double activate(double X) {
            if( X > theta)
                return 1;
            else
                return -1;
        }

        void updateWeights() {
            if(error) {
                for(int i=0; i<weights.size(); i++) {
                    weights[i] += alpha * input[i] * error;
                }
                theta += alpha * error;
            }
        }
        
        void randomizeWeights() {
            for(int i=0; i<weights.size(); i++) {
                weights[i] = (1 - ((double)(rand() % 200000) / 100000));
            }
        }

        void display() {
            int index = 0;
            for(double d : input) {
                cout << "input[" << index << "] : " << d << endl;
                cout << "weights[" << index << "] : " << weights[index] << endl;
                index++;
            }
        }
};
#endif


