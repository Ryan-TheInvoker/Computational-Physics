
//TODO: calculate RMS Error for all cases except the last

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>






//2d Array Implementation
void SRW() {
    

    FILE *output_file = fopen("random_walk_data.txt", "w");

    if (output_file == NULL) {
        printf("Error opening file for writing.\n");
        exit(0);
    }

    fprintf(output_file, "N, R_N^2, RN\n");  // header for the CSV file

    int max_N = 10000;  // Maximum number of steps
    int increment_size = 100;  // Increment step for N
    int trials = 100;  

    for (int N = increment_size; N <= max_N; N += increment_size) {
        double totalRN2 = 0;
        double totalRN = 0;

        for (int t = 0; t < trials; t++) {
            int pos_x = 0, pos_y = 0;  // Start at the origin (x=0, y=0)

            for (int i = 0; i < N; i++) {
                int direction = (int)(drand48() * 4);  // Random number between 0 and 3
                switch (direction) {
                    case 0: pos_x++; break;  // Right
                    case 1: pos_x--; break;  // Left
                    case 2: pos_y++; break;  // Up
                    case 3: pos_y--; break;  // Down
                }
            }

            double RN2 = pos_x * pos_x + pos_y * pos_y; // leave out the square root
            totalRN2 += RN2;
            totalRN += sqrt(RN2);
        }

        double averageRN2 = totalRN2 / trials;
        double averageRN = totalRN / trials;

        fprintf(output_file, "%d,  %.8f,  %.8f\n", N, averageRN2, averageRN);

    }

    fclose(output_file);
    printf("Data saved to random_walk_data.txt\n");
}


//SAW_Rejection Sampling -- 2d Array implementation
#define GRID_SIZE 100

 // 2d case for non-reversing walks
void SAW_rejection() {
     // Seed random number generator

    FILE *output_file = fopen("saw_data.txt", "w");

    if (output_file == NULL) {
        printf("Error opening file for writing.\n");
        exit(0);
    }

    fprintf(output_file, "N, R_N^2, RN, CN_estimate\n");  // header for the CSV file

    int max_N = 26;  // Maximum number of steps
    int increment_size = 1;  // Increment step for N
    int trials = 50000;

    int direction=0;
    int temp;
    for (int N = 1; N <= max_N; N+= increment_size) {
        double totalRN2 = 0;
        double totalRN = 0;
        double validWalkCount = 0.0;

        for (int t = 0; t < trials; t++) {
            int grid[GRID_SIZE][GRID_SIZE] = {0};  // Initialize grid with zeros
            int x = GRID_SIZE / 2, y = GRID_SIZE / 2;  // Start at the center
            grid[x][y] = 1;  // Mark the starting point as visited
            
            int validWalk = 1;  // Flag to check if the walk is valid
            
            //starting positions of x and y
            int xStart = x;
            int yStart = y;

            for (int i = 0; i < N; i++) {
                
                //This part of the function ensures that the walk is non-reversing.
                // Random number between 0 and 3
                int generate = (int)(drand48() * 4);

                if(i>0){

                    while(generate == temp){
                        generate = (int)(drand48() * 4);
                    }
                }

                direction = generate;
                  
                switch (direction) {
                    case 0: x++; break;  // Right
                    case 1: x--; break;  // Left
                    case 2: y++; break;  // Up
                    case 3: y--; break;  // Down
                }
                
                temp = direction;
                // Check if  revisiting a site
                if ( grid[x][y] == 1) {   
                    validWalk = 0;
                    break;
                }

                grid[x][y] = 1;  // Mark the new site as visited
            }

            if (validWalk) {
                
                int xDistance = x-xStart;
                int yDistance = y-yStart;
                double RN2 = xDistance * xDistance + yDistance * yDistance;  // No need for square root
                totalRN2 += RN2;
                totalRN += sqrt(RN2);
                validWalkCount++;
            }
            
        }
        
       
        double averageRN2 = totalRN2 /validWalkCount; 
        double averageRN = totalRN/validWalkCount;
        long double dimensions = 2.0L;
        
        long double total_NRW = (2.0L * dimensions) * powl((2.0L * dimensions - 1.0L), (long double)(N - 1));
        double fraction = validWalkCount/trials;
        long double result = fraction * total_NRW;

        fprintf(output_file, "%d, %.10f, %.10f, %.10Lf\n", N, averageRN2, averageRN, result);
    }

    fclose(output_file);
    printf("Data saved to saw_data.txt\n");
}


//myopic_sampling: in this first rendition we are choosing the next move from the unvisited sites with equal probability.

// Myopic SAW function
void SAW_myopic() {
   // Initialization (Replace these with your actual initializations)
    int max_N = 100;
    int trials = 500000;
    
    FILE *output_file = fopen("SAW_myopic_data.txt", "w");

    if (output_file == NULL) {
        printf("Error opening file for writing.\n");
        exit(1);
    }

    fprintf(output_file, "N, Average_RN2,Average_RN\n");

    for (int N = 1; N <= max_N; N++) {
        double totalRN2 = 0.0;
        int validWalks = 0;
        double totalRN = 0;
        for (int t = 0; t < trials; t++) {
            int grid[GRID_SIZE][GRID_SIZE] = {0};
            int x = GRID_SIZE / 2, y = GRID_SIZE / 2;
            int xStart = x, yStart = y;
            
            grid[x][y] = 1;
            int validWalk = 1;

            for (int i = 0; i < N; i++) {

                int neighbors[4][2] = {0};
                int count = 0;

                // Check unvisited neighbors
                if (grid[x-1][y] == 0) {
                    neighbors[count][0] = x-1; neighbors[count][1] = y; count++;
                }
                if (grid[x+1][y] == 0) {
                    neighbors[count][0] = x+1; neighbors[count][1] = y; count++;
                }
                if (grid[x][y-1] == 0) {
                    neighbors[count][0] = x; neighbors[count][1] = y-1; count++;
                }
                if ( grid[x][y+1] == 0) {
                    neighbors[count][0] = x; neighbors[count][1] = y+1; count++;
                }

                //if count remains zero it means that the walker is stuck
                if (count == 0) {
                    validWalk = 0;
                    break;
                }

                // Choose a random unvisited neighbor
                int choice = (int)(drand48() * count);
                x = neighbors[choice][0];
                y = neighbors[choice][1];
                grid[x][y] = 1;
            }

            if (validWalk) {
                validWalks++;
                int xDistance = x-xStart;
                int yDistance = y-yStart;
                double RN2 = xDistance * xDistance + yDistance * yDistance;
                
                totalRN2 += RN2;
                totalRN += sqrt(RN2);
            }
        }

        double averageRN2 = (validWalks == 0) ? 0 : totalRN2 / validWalks;
        double averageRN = (validWalks == 0) ? 0 : totalRN / validWalks;
        fprintf(output_file, "%d, %.8f, %.8f\n", N, averageRN2,averageRN);   
    }

    fclose(output_file);
    printf("Data saved to saw_myopic_data.txt\n");
}



//In this rendition we are using self-importance sampling to try and berid ourselves of any pesky bias
void SAW_myopic_Rosenbluth(int max_N, int trials) {

    FILE *output_file = fopen("saw_rosenbluth_data.txt", "w");
    if (output_file == NULL) {
        printf("Error opening file for writing.\n");
        exit(1);
    }
    fprintf(output_file, "N, R_N^2, RN\n");

    for (int N = 1; N <= max_N; N++) {
        double weightedSumRN2 = 0.0;
        double weightedSumRN = 0.0;
        double totalWeight = 0.0;

        for (int t = 0; t < trials; t++) {
            int grid[GRID_SIZE][GRID_SIZE] = {0};
            int x = GRID_SIZE / 2, y = GRID_SIZE / 2;
            int xStart = x, yStart = y;
            double W = 1.0;  // Initialize weight
            grid[x][y] = 1;

            for (int i = 0; i < N; i++) {
               
                int neighbors[4][2] = {0};
                int count = 0;

                if (grid[x-1][y] == 0) {
                    neighbors[count][0] = x-1; neighbors[count][1] = y; count++;
                }
                if (grid[x+1][y] == 0) {
                    neighbors[count][0] = x+1; neighbors[count][1] = y; count++;
                }
                if (grid[x][y-1] == 0) {
                    neighbors[count][0] = x; neighbors[count][1] = y-1; count++;
                }
                if ( grid[x][y+1] == 0) {
                    neighbors[count][0] = x; neighbors[count][1] = y+1; count++;
                }

                if (count == 0) {
                    W = 0;
                    break;
                }

                W *= count;  // Update weight

     
                

                // Choose a random unvisited neighbor
                int choice = (int)(drand48() * count);
                x = neighbors[choice][0];
                y = neighbors[choice][1];


                grid[x][y] = 1;
            }
            int xDistance = x-xStart, yDistance = y-yStart;
            double RN2 = xDistance * xDistance + yDistance * yDistance;
            double RN = sqrt(RN2);
            weightedSumRN2 += W * RN2;
            weightedSumRN += W * RN;


            totalWeight += W;
        }

        double averageRN2 = (totalWeight == 0) ? 0 : weightedSumRN2 / totalWeight;
        double averageRN = (totalWeight == 0) ? 0 : weightedSumRN / totalWeight;

        fprintf(output_file, "%d, %.10f, %.10f\n", N, averageRN2, averageRN);
    }

    fclose(output_file);
    printf("Data saved to saw_rosenbluth_data.txt\n");

}


//3D case
void SAW_rejection_3D() {
     

    FILE *output_file = fopen("saw_3D_data.txt", "w");

    if (output_file == NULL) {
        printf("Error opening file for writing.\n");
        exit(0);
    }

    fprintf(output_file, "N, R_N^2, RN, Fraction\n");  // header for the CSV file

    int max_N = 40;  // Maximum number of steps
    int increment_size = 1;  // Increment step for N
    int trials = 50000;

    int direction=0;
    int temp;
    for (int N = 1; N <= max_N; N+= increment_size) {
        double totalRN2 = 0;
        double totalRN = 0;
        double validWalkCount = 0;

        for (int t = 0; t < trials; t++) {
            int grid[GRID_SIZE][GRID_SIZE][GRID_SIZE] = {0};  // Initialize grid with zeros
            int x = GRID_SIZE / 2, y = GRID_SIZE / 2, z = GRID_SIZE / 2;  // Start at the center
            grid[x][y][z] = 1;  // Mark the starting point as visited
            
            int validWalk = 1;  // Flag to check if the walk is valid
            
            //starting positions of x and y
            int xStart = x;
            int yStart = y;
            int zStart = z;

            for (int i = 0; i < N; i++) {
                
                //This part of the function ensures that the walk is non-reversing.
                // Random number between 0 and 5
                int generate = (int)(drand48() * 6);

                //if just to skip for first case
                if(i>0){

                    while(generate == temp){
                        generate = (int)(drand48() * 6);
                    }
                }

                direction = generate;
                  
                switch (direction) {
                    case 0: x++; break;  
                    case 1: x--; break;  
                    case 2: y++; break;  
                    case 3: y--; break;  
                    case 4: z++; break;
                    case 5: z--; break;
                }
                
                temp = direction;
                // Check if out of bounds or revisiting a site
                if ( grid[x][y][z] == 1) {    
                    validWalk = 0;
                    break;
                }

                grid[x][y][z] = 1;  // Mark the new site as visited
            }

            if (validWalk) {
                
                int xDistance = x - xStart;
                int yDistance = y - yStart;
                int zDistance = z - zStart;
                double RN2 = xDistance * xDistance + yDistance * yDistance + zDistance * zDistance; 
                totalRN2 += RN2;
                totalRN += sqrt(RN2);

                validWalkCount++;
            }
            
        }

        double averageRN2 = totalRN2 / validWalkCount; 
        double averageRN = totalRN / validWalkCount;
        int dimensions = 3;
        int total_NRW = (2*dimensions)*pow((2*dimensions-1),(N-1));

        fprintf(output_file, "%d, %.10f, %.10f, %.10f\n", N, averageRN2, averageRN,(validWalkCount/(double)trials)*total_NRW);
    }

    fclose(output_file);
    printf("Data saved to saw_3D_data.txt\n");
}




int main(){
    srand48(time(0));  // Seed random number generator
     
    SRW();
    SAW_rejection();
    SAW_myopic();
    SAW_myopic_Rosenbluth(100,500000);
    SAW_rejection_3D();

    printf("I Ran through Rand al' Thor");
    return 0;
}