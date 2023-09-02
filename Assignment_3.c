
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
    int x;
    int y;
} Position;


//helper methods


//returns 1 if visited and 0 if not visited
int isVisited(Position pos, Position *visited, int length) {
    for (int i = 0; i < length; i++) {
        if (visited[i].x == pos.x && visited[i].y == pos.y) {
            return 1;
        }
    }
    return 0;
}




void SRW() {
    srandom(time(NULL));  // Seed random number generator

    FILE *output_file = fopen("random_walk_data.txt", "w");

    if (output_file == NULL) {
        printf("Error opening file for writing.\n");
        exit(0);
    }

    fprintf(output_file, "N, R_N^2, RN\n");  // header for the CSV file

    int max_N = 10000;  // Maximum number of steps
    int step_size = 100;  // Increment step for N
    int trials = 100;  

    for (int N = step_size; N <= max_N; N += step_size) {
        double totalRN2 = 0;

        for (int t = 0; t < trials; t++) {
            Position pos = {0, 0};  // Start at the origin

            for (int i = 0; i < N; i++) {
                int direction = random() % 4;  // Random number between 0 and 3
                switch (direction) {
                    case 0: pos.x++; break;  // Right
                    case 1: pos.x--; break;  // Left
                    case 2: pos.y++; break;  // Up
                    case 3: pos.y--; break;  // Down
                }
            }

            double RN2 = pos.x * pos.x + pos.y * pos.y; //4 leave out the square root
            totalRN2 += RN2;
        }

        double averageRN2 = totalRN2 / trials;
        fprintf(output_file, "%d,  %.5f,  %.5f\n", N, averageRN2, sqrt(averageRN2));

    }

    fclose(output_file);
    printf("Data saved to random_walk_data.txt\n");

   
}





void SAWs_Rejection_Sampling(){

    srandom(time(NULL));  // Seed random number generator

    FILE *output_file = fopen("saw_data.txt", "w");
    if (output_file == NULL) {
        printf("Error opening file for writing.\n");
        exit(0);
    }

    fprintf(output_file, "N, R_N, R_N^2, Fraction\n");  // header for the CSV file

    int max_N = 1000;  // Maximum number of steps
    int step_size = 100;  // Increment step for N
    int trials = 1000;  // Number of random walk trials

    for (int N = step_size; N <= max_N; N += step_size) {
        double totalR = 0;
        double totalR2 = 0;
        int valid_walks = 0;
        double averageRN = 0;
        double averageRN2 = 0;


        for (int t = 0; t < trials; t++) {

            Position visited[N];
            visited[0] = (Position){0, 0};
            Position current_pos = {0, 0};

            int is_valid = 1;//flag

            
            for (int i = 1; i < N; i++) {
                Position last_pos = current_pos;
                int direction = random() % 4;
                switch (direction) {
                
                case 0: current_pos.x++; break;

                case 1: current_pos.x--; break;

                case 2: current_pos.y++; break;

                case 3: current_pos.y--; break;
            }

            //remember 0 is false in C 

            // Check if the new position is already visited
                if (isVisited(current_pos, visited, i)) { //
                    is_valid = 0;//is not valid

                    break;  // Exit the loop early if the walk is not valid
                } else {
                    visited[i] = current_pos;
                    valid_walks++;
                }
            
            }

                //valid_walks++ moved into the if
                double R = sqrt(current_pos.x * current_pos.x + current_pos.y * current_pos.y);
                totalR += R;
                totalR2 += R * R;
            }

        if (valid_walks == 0) {
            averageRN = 0;
            averageRN2 = 0;
        } else {
            averageRN = totalR / valid_walks;
            averageRN2 = totalR2 / valid_walks;
            }

        fprintf(output_file, "%d,  %.5f,  %.5f\n", N, averageRN2, sqrt(averageRN));

        
    }

        
        

    

    fclose(output_file);
    printf("SAW data saved to saw_data.txt\n");

}

int main(){

    SRW();
    SAWs_Rejection_Sampling();
}