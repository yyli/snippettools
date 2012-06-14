#include <ctime>

class Timer {
	public:
		Timer() : clockType(CLOCK_MONOTONIC) {};
		Timer(clockid_t clock) : clockType(clock) {};
		
		void start() {
			struct timespec	time;
			clock_gettime(clockType, &time);
			start_f = time.tv_sec + 1e-9*time.tv_nsec;	
		};

		void end() {
			struct timespec	time;
			clock_gettime(clockType, &time);
			end_f = time.tv_sec + 1e-9*time.tv_nsec;	
		};
		
		double timePassed() {
			struct timespec time;
			clock_gettime(clockType, &time);
			double mid_f = time.tv_sec + 1e-9*time.tv_nsec;	
			return mid_f - start_f;
		};

		double elapsed() {
			return end_f - start_f;
		};

		double getStartTime() {
			return start_f;
		};

		double getEndTime() {
			return end_f;
		};

	private:
		double start_f, end_f;
		clockid_t clockType;
};
