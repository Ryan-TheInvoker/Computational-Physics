
//TODO: should I be the PRNG once or for each method in my code


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


typedef struct {
    int x;
    int y;
} Position;


//helper methods

//Here we simply just removing walks that intersect themselves


// void SAWs_Rejection_Sampling(){

    

//     FILE *output_file = fopen("saw_data.txt", "w");
//     if (output_file == NULL) {
//         printf("Error opening file for writing.\n");
//         exit(0);
//     }

//     fprintf(output_file, "N, R_N, R_N^2, Fraction\n");  // header for the CSV file

//     int max_N = 1000;  // Maximum number of steps
//     int step_size = 100;  // Increment step for N
//     int trials = 1000;  // Number of random walk trials

//     for (int N = step_size; N <= max_N; N += step_size) {
//         double totalR = 0;
//         double totalR2 = 0;
//         int valid_walks = 0;
//         double averageRN = 0;
//         double averageRN2 = 0;


//         for (int t = 0; t < trials; t++) {

//             Position visited[N];
//             visited[0] = (Position){0, 0};
//             Position current_pos = {0, 0};

//             int is_valid = 1;//flag

            
//             for (int i = 1; i < N; i++) {
//                 Position last_pos = current_pos;
//                 int direction = random() % 4;
//                 switch (direction) {
                
//                 case 0: current_pos.x++; break;

//                 case 1: current_pos.x--; break;

//                 case 2: current_pos.y++; break;

//                 case 3: current_pos.y--; break;
//             }

//             //remember 0 is false in C 

//             // Check if the new position is already visited
//                 if (isVisited(current_pos, visited, i)) { //
//                     is_valid = 0;//is not valid

//                     break;  // Exit the loop early if the walk is not valid
//                 } else {
//                     visited[i] = current_pos;
//                     valid_walks++;
//                 }
            
//             }

//                 //valid_walks++ moved into the if
//                 double R = sqrt(current_pos.x * current_pos.x + current_pos.y * current_pos.y);
//                 totalR += R;
//                 totalR2 += R * R;
//             }

//         if (valid_walks == 0) {
//             averageRN = 0;
//             averageRN2 = 0;
//         } else {
//             averageRN = totalR / valid_walks;
//             averageRN2 = totalR2 / valid_walks;
//             }

//         fprintf(output_file, "%d,  %.5f,  %.5f\n", N, averageRN2, sqrt(averageRN));

        
//     }

        
        

    

//     fclose(output_file);
//     printf("SAW data saved to saw_data.txt\n");

// }






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
        }

        double averageRN2 = totalRN2 / trials;
        fprintf(output_file, "%d,  %.5f,  %.5f\n", N, averageRN2, sqrt(averageRN2));

    }

    fclose(output_file);
    printf("Data saved to random_walk_data.txt\n");
}


//SAW_Rejection Sampling -- 2d Array implementation
#define GRID_SIZE 100


void SAW_rejection() {
     // Seed random number generator

    FILE *output_file = fopen("saw_data.txt", "w");

    if (output_file == NULL) {
        printf("Error opening file for writing.\n");
        exit(0);
    }

    fprintf(output_file, "N, R_N^2, RN, Fraction\n");  // header for the CSV file

    int max_N = 30;  // Maximum number of steps
    int increment_size = 1;  // Increment step for N
    int trials = 500000;

    int direction=0;
    int temp;
    for (int N = 1; N <= max_N; N+= increment_size) {
        double totalRN2 = 0.0;
        double validWalkCount = 0;

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
                // Check if out of bounds or revisiting a site
                if ( grid[x][y] == 1) {    // optional grid size implementation: x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE ||
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
                validWalkCount++;
            }
            
        }

        double averageRN2 = totalRN2 / validWalkCount; //TODO: replaced trials with validWalkCount
        fprintf(output_file, "%d, %.4f, %.4f, %.10f\n", N, averageRN2, sqrt(averageRN2),validWalkCount/trials);
    }

    fclose(output_file);
    printf("Data saved to saw_data.txt\n");
}


//myopic_sampling: in this first rendition we are chooosing the next move from the unvisited sites with equal probability.

int main(){
    srand48(time(0));  // Seed random number generator
    srandom(time(NULL)); 
    SRW();
    SAW_rejection();

    printf("I Ran through Rand al' Thor");
    return 0;
}