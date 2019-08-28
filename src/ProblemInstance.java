
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change thi
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author iot
 */
public class ProblemInstance {

    public String name;
    public int dimension;
    public int vehicles;
    public int capacity;
    public int[][] adjacency;

    public ProblemInstance(String name, int dimension, int vehicles, int capacity, int[][] adjacency) {
        this.name = name;
        this.dimension = dimension;
        this.vehicles = vehicles;
        this.capacity = capacity;
        this.adjacency = adjacency;
    }

    /**
     * Finds the central client
     * @return
     */
    public List<Integer> routeBuilder() {
        int sum = 0, smallest = Integer.MAX_VALUE, centralElement, smallestDistance, biggestDistance;

        for(int i = 0, smallestOnLine = 0, biggestOnLine = 0; i < dimension; i++, sum = 0, smallestOnLine = 0, biggestOnLine = 0) {
            for(int j = 0; j < dimension; j++) {
                sum += adjacency[i][j];

                if(adjacency[i][j] > adjacency[i][biggestOnLine]) {
                    biggestOnLine = j;
                }
                if(adjacency[i][j] < adjacency[i][smallestOnLine]) {
                    smallestOnLine = j;
                }
            }
            if(sum < smallest) {
                smallest = sum;
                centralElement = i;
                biggestDistance = biggestOnLine;
                smallestDistance = smallestOnLine;
            }
        }
        //TODO
        return null;
    }

    public static ProblemInstance parseFile(String path) {
        String name = null;
        int dimension = 0, vehicles = 0, capacity = 0, count = 0;
        int[][] adjacency = null;

        try {
            File inputFile = new File(path);
            BufferedReader in = new BufferedReader(new FileReader(inputFile));
            String line;
            String[] words;

            while ((line = in.readLine()) != null) {
                words = line.trim().split(" +");
                switch (words[0]) {
                    case "NAME:":
                        name = words[1];
                        break;
                    case "DIMENSION:":
                        dimension = Integer.parseInt(words[1]);
                        adjacency = new int[dimension][dimension];
                        break;
                    case "VEHICLES:":
                        vehicles = Integer.parseInt(words[1]);
                        break;
                    case "CAPACITY:":
                        capacity = Integer.parseInt(words[1]);
                        break;
                    case "EDGE_WEIGHT_SECTION":
                        continue;
                    default:
                        if (count < dimension) {
                            for (int i = 0; i < words.length; i++) {
                                adjacency[count][i] = Integer.parseInt(words[i]);
                            }
                        } else {
                            break;
                        }
                        count++;
                }
            }
        } catch(IOException ex){
            System.out.println("Error while parsing file: ");
            ex.printStackTrace();
        }
        return new ProblemInstance(name, dimension, vehicles, capacity, adjacency);
    }
}