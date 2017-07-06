public class Individual implements Comparable<Individual> {
    public int chromosome;
    public int chromosomeSize;
    public int fitness;
    public String binary;
    public double mutationChance = 0.05;

    Individual(int chromosome, int chromosomeSize) {
        this.chromosome = chromosome;
        this.chromosomeSize = chromosomeSize;
        mutate();
        fitness = binary_fitness();
    }

    //Calculate the individual's fitness
    //Also, create binary string for easy output
    public int binary_fitness() {
        int onesCount = 0;
        int tempChromosome = chromosome, lsb = 0;
        StringBuilder builder = new StringBuilder();
        for(int i=0; i < chromosomeSize; i++) {
            lsb = tempChromosome & 1;
            if(lsb == 1) { onesCount++; }
            builder.append(lsb);
            tempChromosome >>= 1;
        }
        binary = builder.toString();
        return onesCount;
    }
    //Mutation_probability to randomly flip a bit in chromosome
    public void mutate() {
        double threshold = Math.random() * 100;
        if(threshold <= mutationChance) {
            int randIndex = (int)(Math.random() * (chromosomeSize) + 1);
            chromosome ^= (1 << (randIndex - 1));
        }
    }
    //For easy ArrayList sorting(Ascending order)
    public int compareTo(Individual comparesTo) {
        return this.fitness - comparesTo.fitness;
    }
}
