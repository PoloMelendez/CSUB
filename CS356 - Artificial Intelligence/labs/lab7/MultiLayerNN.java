import java.util.ArrayList;

public class MultiLayerNN {

    ArrayList<double[]> Yd;
    ArrayList<double[]> inputs;
    ArrayList<ArrayList<Neuron> > layers;
    ArrayList<Neuron> outputLayer;
    ArrayList<Neuron> inputLayer;
    Neuron neuron;

    public static void main(String[] args) {
        MultiLayerNN obj = new MultiLayerNN();
        obj.run();
    }

    public void run() {
        convertInput(4);
        int inputNum = 4;
        int[] hiddenNums = {3};
        int outputNum = 3;
        createLayers(inputNum, hiddenNums, outputNum);
        train(100);
    }

    public void test() {
        double sampleError = 0;
        double totalSampleError = 0;
        for(int i=0; i < inputs.size(); i++) {
            for(int j=0; j < outputLayer.size(); j++) {
                sampleError += Math.abs(Yd.get(i)[j] - outputLayer.get(j).output);
            }
            totalSampleError += sampleError;
            sampleError = 0;
        }
        System.out.println("Mean Absolute Deviation: " + totalSampleError / inputs.size());
    }

    public void train(int epochs) {
        double sampleError = 0, totalEpochError = 0;
        int iteration = 0;
        for (int m = 0; m < epochs; m++) {
            totalEpochError = 0;
            for (int k = 0; k < inputs.size(); k++) {                             //for every input
                setInputLayer(inputs.get(k));
                for(int i = 0; i < layers.size(); i++) {                       //for every layer
                    feed_forward_layer(layers.get(i));
                }
                feed_forward_layer(outputLayer);
                calc_outputLayerError(Yd.get(k));
                update_weights();
                sampleError = 0;
                for(int i=0; i < outputLayer.size(); i++) {
                    sampleError += Math.abs(Yd.get(k)[i] - outputLayer.get(i).output);
                }
                totalEpochError += sampleError;
                System.out.print("Iteration " + (iteration+1) + ": ");
                display(k);
                iteration++;
            }
            System.out.println("Epoch" + (m+1) + " Complete: MAD is: " + totalEpochError / inputs.size());
        }
    }

    public double sumSquaredErrors(double[] expected) {
        double squaredError = 0;
        for(int i=0; i < outputLayer.size(); i++) {
            squaredError += Math.pow(expected[i] - outputLayer.get(i).output, 2);
        }
        return squaredError;
    }

    public void update_weights() {
        for(int i=0; i<outputLayer.size(); i++) {
            outputLayer.get(i).calc_deltaWeights();
        }
        for(int i=layers.size()-1; i>=0; i--) {
            calc_hiddenLayerError(layers.get(i));
            for(int j=0; j<layers.get(i).size(); j++) {
                layers.get(i).get(j).calc_deltaWeights();
            }
        }
        for(int i=0; i < layers.size(); i++) {
            for(int j=0; j < layers.get(i).size(); j++) {
                layers.get(i).get(j).update_weights();
            }
        }
        for(int i=0; i < outputLayer.size(); i++) {
            outputLayer.get(i).update_weights();
        }
    }

    public void calc_outputLayerError(double[] expected) {
        for(int i=0; i < outputLayer.size(); i++) {
            outputLayer.get(i).error = expected[i] - outputLayer.get(i).output;
        }
    }

    public void calc_hiddenLayerError(ArrayList<Neuron> layer) {
        double delta = 0;
        ArrayList<Neuron> next;
        for(int i=0; i < layer.size(); i++) {
            next = layer.get(i).nextLayer;
            for(int j=0; j < next.size(); j++) {
                delta += next.get(j).weights[i] * next.get(j).errorGradient;
            }
            layer.get(i).error = delta;
        }
    }

    public void feed_forward_layer(ArrayList<Neuron> currentLayer) {
        for(int i=0; i<currentLayer.size(); i++) {
            currentLayer.get(i).feed_forward();
        }
    }

    public void createLayers(int input, int[] hidden, int output) {
        inputLayer = new ArrayList<Neuron>();
        layers = new ArrayList<ArrayList<Neuron>>();
        outputLayer = new ArrayList<Neuron>();
        for(int i=0; i < input; i++) {
            inputLayer.add(new Neuron());
        }
        for(int i=0; i < hidden.length; i++) {
            layers.add(new ArrayList<Neuron>());
            for(int j=0; j<hidden[i]; j++) {
                if(i == 0) {
                    layers.get(i).add(new Neuron(inputLayer));
                } else {
                    layers.get(i).add(new Neuron(layers.get(i-1)));
                }
            }
        }
        for(int i=0; i<output; i++) {
            outputLayer.add(new Neuron(layers.get(layers.size() - 1)));
        }
        setNextLayer(inputLayer, layers.get(0));
        for(int i=0; i < layers.size()-1; i++) {
            setNextLayer(layers.get(i), layers.get(i+1));
        }
        setNextLayer(layers.get(layers.size()-1), outputLayer);
    }

    public void setNextLayer(ArrayList<Neuron> prev, ArrayList<Neuron> next) {
        for(int i=0; i < prev.size(); i++) {
            prev.get(i).nextLayer = next;
        }
    }

    public void setInputLayer(double[] currentInputs) {
        for(int i=0; i < inputLayer.size(); i++) {
            inputLayer.get(i).output = currentInputs[i];
        }
    }

    public void displayNN() {
        System.out.print("InputLayer: ");
        for(int i=0; i<inputLayer.size(); i++) {
            System.out.print(inputLayer.get(i).myNum + " ");
        }
        System.out.println();
        for(int i=0; i<layers.size(); i++) {
            System.out.print("Layer" + i + ": ");
            for(int j=0; j<layers.get(i).size(); j++) {
                System.out.print(layers.get(i).get(j).myNum + " ");
            } System.out.println();
        }
        System.out.print("OutputLayer: ");
        for(int i=0; i<outputLayer.size(); i++) {
            System.out.print(outputLayer.get(i).myNum + " ");
        }
        System.out.println("\n");
    }

    public void display(int k) {
        System.out.print("Outputs are: ");
        for(Neuron n : outputLayer) {
            System.out.print(n.output + " ");
        }
        System.out.print("expected: ");
        for(double d : Yd.get(k)) {
            System.out.print(d + " ");
        }
        System.out.print("SSE: " + sumSquaredErrors(Yd.get(k)));
        System.out.println();
    }

    public void displayWeights() {
        for(Neuron n : inputLayer) {
            System.out.println("InputNeuron: " + n.output);
        }
        for(int i=0; i<layers.size();i++) {
            for(Neuron n : layers.get(i)) {
                System.out.print("Neuron" + n.myNum + ": ");
                for(double d : n.weights) {
                    System.out.print(d + " ");
                }
                System.out.println("Theta: " + n.theta + " Output: " + n.output + " Error: " + n.error);
            }
        }
        for(int i=0; i < outputLayer.size(); i++) {
            System.out.print("OutputNeuron" + i + ": ");
            for(double w : outputLayer.get(i).weights) {
                System.out.print(w + " ");
            }
            System.out.print(" output: " + outputLayer.get(i).output + " error: " + outputLayer.get(i).error);
            System.out.println();
        }
        System.out.println();
    }

    public void displayInput() {
        for(int i=0; i<inputs.size(); i++) {
            System.out.print("Input: ");
            for (int k = 0; k < inputs.get(i).length; k++) {
                System.out.print(inputs.get(i)[k] + " ");
            }
            System.out.print(" Yd: ");
            for (int j = 0; j < Yd.get(i).length; j++) {
                System.out.print(Yd.get(i)[j] + "  ");
            }
            System.out.println();
        }
        System.out.println();
    }

    public void convertInput(int xNum) {
        ReadCSV csv = new ReadCSV();
        csv.run();
        inputs = new ArrayList<double[]>();
        Yd = new ArrayList<double[]>();
        double[] currentX;
        double[] currentYd;
        for(String[] s : csv.output) {
            currentX = new double[xNum];
            currentYd = new double[s.length - xNum];
            for(int i=0; i<xNum; i++) {
                currentX[i] = Double.parseDouble(s[i]);
            }
            for(int j=0; j<s.length-xNum; j++) {
                currentYd[j] = Double.parseDouble(s[j + xNum]);
            }
            inputs.add(currentX);
            Yd.add(currentYd);
        }
    }
}
