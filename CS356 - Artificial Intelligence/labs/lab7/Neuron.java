import java.util.*;

public class Neuron {
    static int numNeurons = 0;
    int myNum;
    double alpha = 0.05, error, errorGradient, Yd;
    double theta;
    double deltaTheta;
    double[] weights;
    double[] deltaWeights;
    double output;
    ArrayList<Neuron> prevLayer;
    ArrayList<Neuron> nextLayer;

    Neuron() {
        myNum = numNeurons++;
    }

    Neuron(ArrayList<Neuron> prev) {
        myNum = numNeurons++;                                                   //Which neuron is this in net
        prevLayer = prev;
        error = output = 0;
        theta = randomize();
        initializeWeights();
    }

    public void feed_forward() {
        output = activation();
    }

    public void calcErrorGradient() {   //Account for output layer, and hidden layer
        errorGradient = output * (1 - output) * error;
    }

    public void calc_deltaWeights() {
        calcErrorGradient();
        for(int i=0; i<weights.length; i++) {
            deltaWeights[i] = alpha * prevLayer.get(i).output * errorGradient;
        }
        deltaTheta = alpha * (-1) * errorGradient;
    }

    public void update_weights() {
        for(int i=0; i<weights.length; i++) {
            weights[i] += deltaWeights[i];
        }
        theta += deltaTheta;
    }

    public void initializeWeights() {
        weights = new double[prevLayer.size()];
        deltaWeights = new double[prevLayer.size()];
        for(int i=0; i<weights.length; i++) {
            weights[i] = randomize();
        }
    }

    public double randomize() { return Math.random() * 2 - 1; }

    double dot() {
        double scalar = 0;
        for(int i=0; i<prevLayer.size(); i++) {
            scalar += prevLayer.get(i).output * weights[i];
        }
        return scalar;
    }

    public double activation() {
        return 1/(1 + Math.pow(Math.E, -(dot() - theta)));
    }
}
