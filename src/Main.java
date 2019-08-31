import java.io.*;
import java.util.LinkedList;
import java.util.List;

public class Main {

    public static void main(String[] args) {
        long start = System.currentTimeMillis();
        ProblemInstance instances = ProblemInstance.parseFile("./instancias_teste/P-n55-k7.txt");
        long end = System.currentTimeMillis();
        System.out.println("" + (end - start) + " ms");
    }
}
