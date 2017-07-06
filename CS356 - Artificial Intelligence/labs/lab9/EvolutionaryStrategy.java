import java.util.*;

public class EvolutionaryStrategy {

    ArrayList<double[]> data;
    String file = "./data.csv";

    public static void main(String[] args) {
        EvolutionaryStrategy obj = new EvolutionaryStrategy();
        obj.run();
    }

    public void run() {
        data = ReadCSV.convert(file);
        Individual temp = new Individual(data);
        for(int i=0; i<10000; i++) {
            temp.mutate();
        }
        System.out.println("Final Equation: ");
        System.out.println("Fitness: " + temp.fitness);
        System.out.println(temp.chromosome[0] + "x^3 + " + temp.chromosome[1] + "x^2 + " + temp.chromosome[2] + "x + " + temp.chromosome[3]);
    }

    public void displayData() {
        if (data != null) {
            for (int i = 0; i < data.size(); i++) {
                System.out.println(data.get(i)[0] + "  " + data.get(i)[1]);
            }
        } else { System.out.println("Must initialize data!"); }
    }
}
