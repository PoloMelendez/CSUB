import java.util.*;

public class Individual {
    double[] chromosome;
    double fitness;
    ArrayList<double[]> data;

    public Individual(ArrayList<double[]> d) {
        data = d;
        init_chromosome();
    }

    public void init_chromosome() {
        chromosome = new double[4];
        Random r = new Random();
        for(int i=0; i<chromosome.length; i++) {
            //initialize to random num
            chromosome[i] = -20 + (20 - (-20)) * r.nextDouble();
        }
        fitness = fitness(chromosome);
    }

    public double fitness(double[] chrom) {
        //check fitness
        double x, yd, y, sse = 0;
        for(double[] d : data) {
            x = d[0];
            yd = d[1];
            y = chrom[0] * Math.pow(x, 3) + chrom[1] * Math.pow(x, 2) + chrom[2] * x + chrom[3];
            sse += Math.pow(yd - y, 2);
        }
        return sse;
    }

    public void mutate() {
        //creates the child
        double std = (20 - (-20))/4;
        double[] childChromosome = new double[chromosome.length];
        Random r = new Random();
        for(int i=0; i<childChromosome.length; i++) {
            childChromosome[i] = chromosome[i] + (r.nextGaussian()*std);
        }
        double childFitness = fitness(childChromosome);
        if(childFitness < fitness) {
            chromosome = childChromosome;
            fitness = childFitness;
        }
    }
}
