#include <math.h>
#include <stdlib.h>

void seed_gauss(unsigned int seed) {
    srand(seed);
}

double ranf() {
    return (double)rand() / RAND_MAX;
}

float gauss(double mu, double sigma) {
    double x1, x2, w, y1, y2;

    do {
        x1 = 2.0 * ranf() - 1.0;
        x2 = 2.0 * ranf() - 1.0;
        w = x1*x1 + x2*x2;
    } while (w >= 1.0);

    w = sqrt( (-2.0 * log(w)) / w);
    return mu + sigma * x1 * w;
}
