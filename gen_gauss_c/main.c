#include <stdio.h>
#include <time.h>
#include "gauss.h"

int main() {
    seed_gauss(time(NULL));
    int i = 0;
    while(i++ < 1000)
        printf("%f\n", gauss(100, 4.3));
}
