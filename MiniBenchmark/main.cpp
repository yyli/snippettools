#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "MiniBenchmark.hpp"

using namespace std;

int main() {
    Timer a;
    Timer b(CLOCK_REALTIME);
    Timer c(CLOCK_MONOTONIC_RAW);
    Timer d(CLOCK_PROCESS_CPUTIME_ID);
    Timer e(CLOCK_THREAD_CPUTIME_ID);
    while(1)
    {
        MiniBenchmark::BENCHMARK_BEGIN("all");
        a.start();
        b.start();
        c.start();
        d.start();
        e.start();

        MiniBenchmark::BENCHMARK_BEGIN("hi");
        usleep(1*1000000);
        MiniBenchmark::BENCHMARK_END("hi");

        a.end();
        b.end();
        c.end();
        d.end();
        e.end();
        printf("Time passed: \n");
        printf("   MONO: %15.10lfms\n", a.elapsed()*1000);
        printf("   REAL: %15.10lfms\n", b.elapsed()*1000);
        printf("   MRAW: %15.10lfms\n", c.elapsed()*1000);
        printf("   CPU:  %15.10lfms\n", d.elapsed()*1000);
        printf("   THRD: %15.10lfms\n", e.elapsed()*1000);
        printf("\n");
       
        MiniBenchmark::BENCHMARK_END("all");
        MiniBenchmark::BENCHMARK_PRINT("all");
        MiniBenchmark::BENCHMARK_PRINT_MS();
    }
    return 0;
}
