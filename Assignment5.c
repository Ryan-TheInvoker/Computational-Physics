
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define J 1.0  // Exchange energy
#define SWEEPS 1000  // Number of Monte Carlo sweeps
#define EQUILIBRATION_SWEEPS 500 // Additional sweeps for equilibration


// Function to compute the Hamiltonian for a given state and lattice size.
    //  Details: It loops over all spins in the lattice and calculates the energy based on neighboring spins and an external magnetic field h
double compute_H(int **s, int L, double h) {
    double H = 0.0;
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            H -= J * s[i][j] * (s[i][(j+1)%L] + s[(i+1)%L][j]) + h * s[i][j]; //The modulo operation ensures periodic boundary conditions, effectively "wrapping" around the lattice
        }
    }
    return H;
}


void run_simulation(int L, double beta, FILE *file, int equilibration) {

    //accumulators for averages
    double E_acc = 0.0, E2_acc = 0.0;
    int M_acc = 0, M2_acc = 0;

    int **s = malloc(L * sizeof(int *));
    for (int i = 0; i < L; i++) {
        s[i] = malloc(L * sizeof(int));
    }

    // Initialize lattice to all-up state
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            s[i][j] = 1;
        }
    }

    // Initialize a lookup table for exp(-beta * dH) values at the start of the simulation
    double exp_lookup[5];
    int dH_values[5] = {-8*J, -4*J, 0, 4*J, 8*J};
    for (int i = 0; i < 5; i++) {
        exp_lookup[i] = exp(-beta * dH_values[i]);
    }

    if(equilibration){

            // Equilibration
        for (int sweep = 0; sweep < EQUILIBRATION_SWEEPS; sweep++) {
            for (int i = 0; i < L; i++) {
                for (int j = 0; j < L; j++) {

                    double dH = 2.0 * s[i][j] * (J * (s[i][(j+1)%L] + s[(i+1)%L][j] + s[i][(j-1+L)%L] + s[(i-1+L)%L][j]));

                    if (dH < 0.0 || (drand48() < (dH == -8*J ? exp_lookup[0] : dH == -4*J ? exp_lookup[1] : dH == 0 ? exp_lookup[2] : dH == 4*J ? exp_lookup[3] : exp_lookup[4]))) {
                        s[i][j] *= -1;
                    }
                }
            }
        }

    }

    // Monte Carlo sweeps
    for (int sweep = 0; sweep < SWEEPS; sweep++) {
        for (int i = 0; i < L; i++) {
            for (int j = 0; j < L; j++) {

                double dH = 2.0 * s[i][j] * (J * (s[i][(j+1)%L] + s[(i+1)%L][j] + s[i][(j-1+L)%L] + s[(i-1+L)%L][j]));

                //ternary operators; essentially just a compounded elseif
                if (dH < 0.0 || (drand48() < (dH == -8*J ? exp_lookup[0] : 
                    dH == -4*J ? exp_lookup[1] : 
                    dH ==  0 ? exp_lookup[2] :
                    dH ==  4*J ? exp_lookup[3] : exp_lookup[4]))) 
                {
                    s[i][j] *= -1;
                }
            }
        }

        // Compute magnetization and energy after each sweep
        int M = 0;
        double E = compute_H(s, L, 0.0);

        for (int i = 0; i < L; i++) {
            for (int j = 0; j < L; j++) {
                M += s[i][j];
            }
        }
        
        // Add to accumulators
        E_acc += E;
        E2_acc += E * E;
        M_acc += M;
        M2_acc += M * M;

        //writing instanteous values to file
        fprintf(file, "%d,%.2f,%d,%d,%.2f\n", L, beta, M, abs(M), E); //is this E/n??
    }

    double E_avg = E_acc / SWEEPS;
    double E2_avg = E2_acc / SWEEPS;
    double M_avg = (double)M_acc / SWEEPS;
    double M2_avg = (double)M2_acc / SWEEPS;
    FILE *avg_file = fopen("averages.csv", "a");
    fprintf(avg_file, "%d,%.2f,%.2f,%.2f,%.2f,%.2f\n", L, beta, E_avg, E2_avg, M_avg, M2_avg);
    fclose(avg_file);


    for (int i = 0; i < L; i++) {
        free(s[i]);
    }
    free(s);
}

int main() {
    srand48(time(NULL));

    FILE *file = fopen("data.csv", "w");
    fprintf(file, "L,Beta,M,|M|,E\n");

    // Example of running the simulation for given L and beta values
    

    //PART_1
    run_simulation(4, 0.2, file,0);
    run_simulation(4, 0.4, file,0);
    run_simulation(4, 0.6, file,0);
    run_simulation(10, 0.2, file,0);
    run_simulation(10, 0.4, file,0);
    run_simulation(10, 0.6, file,0);
    run_simulation(50, 0.2, file,0);
    run_simulation(50, 0.4, file,0);
    run_simulation(50, 0.6, file,0);

    fclose(file);
    //PART_2
    FILE *file1 = fopen("data_part2.csv", "w");
    fprintf(file1, "L,Beta,M,|M|\n");

    int L = 200;
    for (double beta = 0.1; beta <= 1.0; beta += 0.05) {
        run_simulation(L, beta, file1,1);
    }

    fclose(file);

    FILE *file2 = fopen("data_part3.csv", "w");
    fprintf(file2, "L,Beta,E_avg,E2_avg,M_avg,M2_avg\n");

    // Investigate the average magnetization per spin m and m_abs as functions of beta*J

    L = 200;
    for (double beta = 0.1; beta <= 1.0; beta += 0.05) {
        run_simulation(L,beta, file2,1);
    }

    fclose(file2);


    return 0;
}
