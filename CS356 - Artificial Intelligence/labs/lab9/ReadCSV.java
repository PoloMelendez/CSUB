import java.io.*;
import java.util.*;

public class ReadCSV {

    public static ArrayList<double[]> convert(String file) {
        ArrayList<double[]> data = new ArrayList<double[]>();
        BufferedReader br = null;
        String line = "";
        String[] lineArray;
        String delim = ",";
        double[] vals;

        try {
            br = new BufferedReader(new FileReader(file));
            while((line = br.readLine()) != null) {
                lineArray = line.split(delim);
                vals = new double[lineArray.length];
                for(int i=0; i<lineArray.length; i++) {
                    vals[i] = Double.parseDouble(lineArray[i]);
                }
                data.add(vals);
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
        return data;
    }

}
