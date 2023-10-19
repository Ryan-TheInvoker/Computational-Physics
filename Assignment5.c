
//TODO: note for question 2 we have a serious need for higher numbers of points


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void compute_Gr(int** s, int L, double* Gr_values);
double distance(int x1, int y1, int x2, int y2, int L);

#define J 1.0  // Exchange energy
// #define SWEEPS 10000  // Number of Monte Carlo sweeps

#define EQUILIBRATION_SWEEPS 1000 
#define EQUILIBRATION_SWEEPS_KLOMP 8000

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


void  run_simulation(int L, double beta, FILE *file,  int equilibration,int sweeps,FILE *avg_file,FILE *partIV) {

    //accumulators for averages
    double E_acc = 0.0, E2_acc = 0.0;
    int M_acc = 0, M2_acc = 0, M_abs_acc=0;

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

    switch (equilibration)
    {
    case 1:

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
        break;
    case 2:

        for (int sweep = 0; sweep < EQUILIBRATION_SWEEPS_KLOMP; sweep++) {
                for (int i = 0; i < L; i++) {
                    for (int j = 0; j < L; j++) {

                        double dH = 2.0 * s[i][j] * (J * (s[i][(j+1)%L] + s[(i+1)%L][j] + s[i][(j-1+L)%L] + s[(i-1+L)%L][j]));

                        if (dH < 0.0 || (drand48() < (dH == -8*J ? exp_lookup[0] : dH == -4*J ? exp_lookup[1] : dH == 0 ? exp_lookup[2] : dH == 4*J ? exp_lookup[3] : exp_lookup[4]))) {
                            s[i][j] *= -1;
                        }
                    }
               }
        }
    break;
    default:
        break;
    }
    

    // Monte Carlo sweeps
    for (int sweep = 0; sweep < sweeps; sweep++) {
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
        M_abs_acc += abs(M);
        M2_acc += M * M;
    
        //write to file or not
        if(file!=NULL){

        fprintf(file, "%d,%.2f,%d,%d,%.2f\n", L, beta, M, abs(M), E); //is this E/n??

        }

    //     if(partIV) {
    //         // double Gr_values[L/2 + 1] = {0};
    //         // compute_Gr(s, L, Gr_values);
    //         for (int r = 0; r <= L/2; r++) {
    //        // fprintf(file4, "%d,%.2f,%d,%.2f\n", L, beta, r, Gr_values[r]);
    // }
//}  
    }
    
    

    if(avg_file != NULL){

    double E_avg = E_acc / sweeps;
    double E2_avg = E2_acc / sweeps;
    double M_avg = (double)M_acc / sweeps;
    double M_abs_avg = (double)M_abs_acc /sweeps;
    double M2_avg = (double)M2_acc / sweeps;

    
    
    fprintf(avg_file, "%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", L, beta, E_avg, E2_avg, M_avg, M_abs_avg, M2_avg);

    
    
    }
    if(partIV!=NULL){
    // Compute G(r) and m
        double Gr_values[L/2 + 1];
        for (int r = 0; r <= L/2; r++) {
            Gr_values[r] = 0.0;
        }

        compute_Gr(s, L, Gr_values);

        double m = 0.0;
        for (int i = 0; i < L; i++) {
            for (int j = 0; j < L; j++) {
                m += s[i][j];
            }
        }
        m /= (L * L);

    // Compute g(r) = G(r) - m^2 and save to file
        for (int r = 0; r <= L/2; r++) {
            double g = Gr_values[r] - m * m;
            fprintf(partIV, "%d,%.2f,%d,%.2f,%.2f\n", L, beta, r, Gr_values[r], g);
        }

    }


    
    for (int i = 0; i < L; i++) {
        free(s[i]);
    }

    free(s);
}






// ... [Your previous code and functions here] ...

double distance(int x1, int y1, int x2, int y2, int L){
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    
    // Accounting for periodic boundary conditions
    if (dx > L/2) dx = L - dx;
    if (dy > L/2) dy = L - dy;

    return sqrt(dx*dx + dy*dy);
}

// void compute_two_point_function(int **s, int L, double *G, double *m) {
//     // Initialize accumulators
//     double m_acc = 0.0;
//     int pairs = 0;  // Number of pairs for each r

//     // Loop over all pairs of spins in the lattice
//     for (int x1 = 0; x1 < L; x1++) {
//         for (int y1 = 0; y1 < L; y1++) {
//             for (int x2 = 0; x2 < L; x2++) {
//                 for (int y2 = 0; y2 < L; y2++) {
//                     double r = distance(x1, y1, x2, y2, L);
//                     int r_int = (int) round(r);
//                     G[r_int] += s[x1][y1] * s[x2][y2];
//                     pairs++;
//                 }
//             }
//             m_acc += s[x1][y1];
//         }
//     }
    
//     // Normalize the results
//     for (int i = 0; i < L; i++) {
//         G[i] /= pairs;
//     }   
//     *m = m_acc / (L * L);
// }

// void compute_and_save_G(int **s, int L, double beta, FILE *file) {
//     // Compute G(r) and m
//     double *G = malloc(L * sizeof(double));
//     double m;
//     compute_two_point_function(s, L, G, &m);

//     // Compute g(r) = G(r) - m^2 and save to file
//     for (int r = 0; r < L; r++) {
//         double g = G[r] - m * m;
//         fprintf(file, "%d,%.2f,%d,%.2f,%.2f\n", L, beta, r, G[r], g);
//     }

//     free(G);
// }


void compute_Gr(int** s, int L, double* Gr_values) {
    
    int num_pairs[L/2 + 1];
    for (int r = 0; r <= L/2; r++) {
        num_pairs[r] = 0;
    }  // Store number of pairs at each distance r

    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            for (int x = 0; x < L; x++) {
                for (int y = 0; y < L; y++) {
                    int r = (int)round(distance(i, j, x, y, L));
                    if (r <= L/2) {
                        Gr_values[r] += s[i][j] * s[x][y];
                        num_pairs[r]++;
                    }
                }
            }
        }
    }

    // Normalize G(r) values
    for (int r = 0; r <= L/2; r++) {
        if (num_pairs[r] != 0) {
            Gr_values[r] /= num_pairs[r];
        }
    }
}





int main() {
    srand48(time(NULL));

    FILE *file = fopen("data.csv", "w");
    fprintf(file, "L,Beta,M,|M|,E\n");
    //FILE *file_avg = fopen("averages_part1.csv","w");
    // Example of running the simulation for given L and beta values
    
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    //PART_1
    // run_simulation(4, 0.2, file,0,10000,NULL,NULL);
    // run_simulation(4, 0.4, file,0,10000,NULL,NULL);
    // run_simulation(4, 0.6, file,0,10000,NULL,NULL);
    // run_simulation(10, 0.2, file,0,10000,NULL,NULL);
    // run_simulation(10, 0.4, file,0,10000,NULL,NULL);
    // run_simulation(10, 0.6, file,0,10000,NULL,NULL);
    // run_simulation(50, 0.2, file,0,10000,NULL,NULL);
    // run_simulation(50, 0.4, file,0,10000,NULL,NULL);
    // run_simulation(50, 0.6, file,0,10000,NULL,NULL);
    run_simulation(200, 0.2, file,0,10000,NULL,NULL);
    run_simulation(200, 0.4, file,0,10000,NULL,NULL);
    run_simulation(200, 0.6, file,0,10000,NULL,NULL);

    fclose(file);

    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken Part 1: %f seconds\n", cpu_time_used);


    //PART_2 

    // start = clock();

    // FILE *file2 = fopen("data_part2.csv", "w");
    // fprintf(file2, "L,Beta,M,|M|\n");
    
    // FILE *file2_avg = fopen("averages_part2.csv","w");
    // fprintf(file2_avg, "L,Beta,<E>,<E2>,<M>,<|M|>,<M2>\n");
    
    // int L = 200;
    
    // for (double beta = 0.1; beta <= 0.4; beta += 0.05) {
    //     run_simulation(L,beta, NULL,1,10000,file2_avg,NULL);
    // }

    // for(double beta = 0.4; beta <=0.6; beta+= 0.01){

    //     run_simulation(L,beta,NULL,2,50000,file2_avg,NULL);

    // }

    // for (double beta = 0.6; beta <= 1; beta += 0.05) {
    //     run_simulation(L,beta, NULL,1,10000,file2_avg,NULL);
    // }

    
    // end = clock();
    // cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    // printf("Time taken Part 2: %f seconds\n", cpu_time_used);

    // //fclose(file1);
    // fclose(file2_avg);

    //PART 3

    // start = clock();

    // FILE *file3 = fopen("data_part3.csv", "w");
    // fprintf(file3, "L,Beta,E_avg,E2_avg,M_avg,M2_avg\n");
    // FILE *file3_avg = fopen("averages_part3.csv","w");
    // fprintf(file3_avg, "L,Beta,<E>,<E2>,<M>,<|M|>,<M2>\n");
    // L = 200;
    // for (double beta = 0.1; beta <= 0.4; beta += 0.05) {
    //     run_simulation(L, beta, NULL,2,10000,file3_avg,NULL);
    // }
    // for (double beta = 0.4; beta <= 0.45; beta += 0.01) {
    //     run_simulation(L, beta, NULL,2,10000,file3_avg,NULL);
    // }
    // for (double beta = 0.45; beta <= 1; beta += 0.05) {
    //     run_simulation(L, beta, NULL,2,10000,file3_avg,NULL);
    // }   
    
    // L = 300;
    // for (double beta = 0.1; beta <= 0.4; beta += 0.05) {
    //     run_simulation(L, beta, NULL,2,10000,file3_avg,NULL);
    // }
    // for (double beta = 0.4; beta <= 0.45; beta += 0.01) {
    //     run_simulation(L, beta, NULL,2,10000,file3_avg,NULL);
    // }
    // for (double beta = 0.45; beta <= 1; beta += 0.05) {
    //     run_simulation(L, beta, NULL,2,10000,file3_avg,NULL);
    // }   

    // end = clock();
    // cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    // printf("Time taken Part 3: %f seconds\n", cpu_time_used);

    // fclose(file2);
    // fclose(file3_avg);



    //PART 4

    start = clock();

    FILE *file4 = fopen("data_part4.csv", "w");
    fprintf(file4, "L,Beta,r,G(r),g(r)\n");

    int L = 200;  // Sample L value

    double Gr_values[L/2 + 1];
    for (int r = 0; r <= L/2; r++) {
        Gr_values[r] = 0.0;
    } 

    for (double beta = 0.1; beta <= 1.0; beta += 0.1) {

        run_simulation(L, beta, NULL,2,5000,NULL,file4);
        
    }

    fclose(file4);

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken Part 4: %f seconds\n", cpu_time_used);

    return 0;
}
