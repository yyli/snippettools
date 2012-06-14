#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "Timer.hpp"

using namespace std;

int main() {
	Timer a;
	Timer b(CLOCK_REALTIME);
	Timer c(CLOCK_MONOTONIC_RAW);
	Timer d(CLOCK_PROCESS_CPUTIME_ID);
	Timer e(CLOCK_THREAD_CPUTIME_ID);
	while(1)
	{
		a.start();
		b.start();
		c.start();
		d.start();
		e.start();
		sleep(1);
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
	}
	return 0;
}
