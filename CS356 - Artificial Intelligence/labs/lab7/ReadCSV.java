import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;

import java.io.*;
import java.util.*;

public class ReadCSV {
    ArrayList<String[]> output;
    String file = "./lab7_iris.csv";
    //String file = "E:/Code/src/messidor_features.csv";
    //String file = "E:/Code/src/sonar.csv";

    public static void main(String[] args) {
        ReadCSV obj = new ReadCSV();
        obj.run();
    }

    public void run() {
        output = new ArrayList<String[]>();
        BufferedReader br = null;
        String line = "";
        String delim = ",";
        System.out.println(System.getProperty("user.dir"));

        try {
            br = new BufferedReader(new FileReader(file));
            while((line = br.readLine()) != null) {
                output.add(line.split(delim));
            }
        } catch(FileNotFoundException e) {
            e.printStackTrace();
        } catch(IOException e) {
            e.printStackTrace();
        } finally {
            if(br != null) {
                try {
                    br.close();
                } catch(IOException e) {
                    e.printStackTrace();
                }
            }
        }
        //display();
    }

    public void display() {
        for(String[] s : output) {
            for(String c : s) {
                System.out.print(c + " ");
            }
            System.out.println("");
        }
    }
}
