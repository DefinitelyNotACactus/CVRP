
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

public class ProblemInstance {

    public String name;
    public int dimension;
    public int vehicles;
    public int capacity;
    public int[][] adjacency;
    public int[] demand;

    public ProblemInstance(String name, int dimension, int vehicles, int capacity, int[][] adjacency, int[] demand) {
        this.name = name;
        this.dimension = dimension;
        this.vehicles = vehicles;
        this.capacity = capacity;
        this.adjacency = adjacency;
        this.demand = demand;
    }

    /**
     * Finds routes for a problem instance
     * @return
     */
    public List<Integer> routeBuilder() {
        int sum = 0, smallest = Integer.MAX_VALUE, centralElement=0, smallestDistance=0, biggestDistance=0;
        Edge[] edgesA = new Edge[dimension - 1];
        Route[] routes = new Route[vehicles];

        int max = 0, min = Integer.MAX_VALUE;
        for(int i = 1; i < dimension; i++) {
            edgesA[i] = new Edge(0, i, adjacency[0][i], demand[i]);
            if(edgesA[i].distance < min) {
                min = edgesA[i].distance;
            } else if(edgesA[i].distance > max) {
                max = edgesA[i].distance;
            }
        }
        int[] count = new int[max - min + 1];
        for(int i = 0; i < edgesA.length; i++) {
            count[edgesA[i].distance - min]++;
        }
        for(int i = 1; i < count.length; i++) {
            count[i] += count[i - 1];
        }
        Edge[] out = new Edge[dimension - 1];
        for(int i = out.length - 1; i >= 0; i--) {
            out[count[edgesA[i].distance - min] - 1] = edgesA[i];
            count[edgesA[i].distance - min]--;
        }
        edgesA = out;

        for(int i = 0; i < vehicles; i++) {
            routes[i] = new Route(capacity);
            routes[i].path.add(edgesA[i]);
        }

        for(int i = 0; i < vehicles; i++) {
        }
        /*
        //Calculates the radius of the groups
        int radius = (adjacency[centralElement][biggestDistance] - adjacency[centralElement][smallestDistance])/vehicles;
        //create the groups
        ArrayList<ArrayList<Integer>> groups = new ArrayList<ArrayList<Integer>>();
        for(int i = 0; i < (adjacency[centralElement][biggestDistance] - adjacency[centralElement][smallestDistance])/radius + 1; i++)
            groups.add(new ArrayList<Integer>());

        //allocates every client in a group
        for(int i = 1; i <= groups.size(); i++) {
            for(int j = 0; j < dimension; j++) {
                if( (adjacency[centralElement][smallestDistance] + (i-1)*radius) <= adjacency[centralElement][j] && adjacency[centralElement][j] < (adjacency[centralElement][smallestDistance] + i*radius)){
                    groups.get(i-1).add(j);
                }
            }
        }
        */






        //TODO
        return null;
    }

    public static ProblemInstance parseFile(String path) {
        String name = null;
        int dimension = 0, vehicles = 0, capacity = 0, count = 0;
        int[][] adjacency = null;
        int[] demand = null;
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
                        demand = new int[dimension];
                        break;
                    case "VEHICLES:":
                        vehicles = Integer.parseInt(words[1]);
                        break;
                    case "CAPACITY:":
                        capacity = Integer.parseInt(words[1]);
                        break;
                    case "DEMAND_SECTION:":
                        for(int i = 0; i < dimension; i++) {
                            line = in.readLine();
                            words = line.trim().split(" +");
                            demand[i] = Integer.parseInt(words[1]);
                        }
                    case "EDGE_WEIGHT_SECTION":
                    case "":
                        continue;
                    default:
                        for (int i = 0; i < dimension; i++) {
                            adjacency[count][i] = Integer.parseInt(words[i]);
                        }
                        count++;
                }
            }
        } catch(IOException ex){
            System.out.println("Error while parsing file: ");
            ex.printStackTrace();
        }
        return new ProblemInstance(name, dimension, vehicles, capacity, adjacency, demand);
    }

    static class Route {
        List<Edge> path;
        int capacity;
        int cost;

        Route(int capacity) {
            path = new ArrayList<>();
            this.capacity = capacity;
            cost = 0;
        }

        public boolean addEdge(Edge edge) {
            if(edge.demand > capacity) {
                return false;
            }
            capacity -= edge.demand;
            cost += edge.distance;
            path.add(edge);
            return true;
        }
    }

    static class Edge {
        int a;
        int b;
        int distance;
        int demand;

        Edge(int a, int b, int distance, int demand) {
            this.a = a;
            this.b = b;
            this.distance = distance;
            this.demand = demand;
        }
    }
}