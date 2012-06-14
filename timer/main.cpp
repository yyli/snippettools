#include <stdio.h>
#include <iostream>
#include "Timer.hpp"

using namespace std;

int main() {
	Timer a;
	a.start();
	for (int i = 0; i < 10; i++)
	{
		sleep(1);
		printf("Time passed: %15.10lf\n", a.timePassed());	
	}
	a.end();
	printf("Time elapsed: %15.10lf\n", a.elapsed());	
	return 0;
}
