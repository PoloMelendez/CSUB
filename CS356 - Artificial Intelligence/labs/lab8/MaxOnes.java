import java.util.*;

public class MaxOnes {

    ArrayList<Individual> population;
    private int individualSize = 10;
    private int populationSize = 10;
    int generationCount = 1000;

    public static void main(String[] args) {
        MaxOnes obj;
        if(args.length == 1) {
            int gens = Integer.parseInt(args[0]);
            obj = new MaxOnes(gens);
        } else {
            obj = new MaxOnes(1000);
        }
        obj.run();
    }

    public MaxOnes(int generationCount) {
        this.generationCount = generationCount;
    }

    public void run() {
        population = new ArrayList<Individual>();
        int maxNum = (int)Math.pow(2, individualSize);
        for(int i=0; i<populationSize; i++) {
            population.add(new Individual((int)(Math.random() * maxNum), individualSize));
        }
        evolve();
    }

    public void crossOver(Individual a, Individual b) {
        int point = (int)(Math.random() * (individualSize+1));
        //Split parent A's chromosome
        int a_left = a.chromosome >> point;
        a_left <<= point;
        int a_right = a.chromosome & (int)Math.pow(2, point) - 1;
        //Split parent B's chromosome
        int b_left = b.chromosome >> point;
        b_left <<= point;
        int b_right = b.chromosome & (int)Math.pow(2, point) - 1;
        //Create two offspring by combining the halves of the parent's
        int offspring1 = a_left + b_right, offspring2 = a_right + b_left;
        population.add(new Individual(offspring1, individualSize));
        population.add(new Individual(offspring2, individualSize));
    }

    //Cross over the parent population to create the offspring generation
    //Combine them together and prune the least fit until population size is back to N
    //Repeat for # of generations, or until a complete chromosome is found
    public void evolve() {
        int bestFitness = 0;
        for(int generation = 1; generation <= generationCount; generation++) {
            for (int i = 0; i < populationSize; i += 2) {
                crossOver(population.get(i), population.get(i + 1));
            }
            prunePopulation();
            bestFitness = population.get(populationSize - 1).fitness;
            if((generationCount - generation) <= 100 || bestFitness >= individualSize) {
                display();
                System.out.println("-----------------------------------------------");
                System.out.println("Generation " + generation + ": Best individual's fitness is " + bestFitness);
                System.out.println("-----------------------------------------------");
                if(bestFitness >= individualSize)
                    break;
            }
        }
    }

    //Trim least fit from population
    public void prunePopulation() {
        Collections.sort(population);
        int pruneLength = population.size() - populationSize;
        for(int k = pruneLength-1; k >=0; k--) {
            population.remove(k);
        }
    }

    public void display() {
        for(int i=0; i < populationSize; i++) {
            System.out.println("Individual " + i + ":  [" + population.get(i).binary + "] and fitness is " + population.get(i).fitness);
        }
    }
}
