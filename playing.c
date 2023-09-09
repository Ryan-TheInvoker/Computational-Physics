#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    // Seed the random number generator
    srand48(time(0));

    // Generate 10 random numbers using drand48
    for(int i = 0; i < 10; ++i) {
        double random_value = drand48();
        printf("Random Value %d: %f\n", i + 1, random_value);
    }

    return 0;
}