#include <unordered_map>
#include <string>
#include <ctime>

class Timer {
    public:
        Timer() : clockType(CLOCK_MONOTONIC) {};
        Timer(clockid_t clock) : clockType(clock) {};
        
        void start() {
            struct timespec time;
            clock_gettime(clockType, &time);
            start_f = time.tv_sec + 1e-9*time.tv_nsec;  
        };

        void end() {
            struct timespec time;
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

class MiniBenchmark
{
    public:
        static MiniBenchmark& getInstance() {
            static MiniBenchmark instance;
            return instance;
        }

        static void BEGIN_BENCHMARK(std::string id) {
            getInstance().start(id);
        }

        static void END_BENCHMARK(std::string id) {
            getInstance().end(id);
        }

        static double BENCHMARK_RESULT(std::string id) {
            getInstance().elapsed(id);
        }

        static void BENCHMARK_PRINT_MS(std::string id) {
            getInstance().printElapsed(id, 1000);
        }

        static void BENCHMARK_PRINT_US(std::string id) {
            getInstance().printElapsed(id, 1000000);
        }

        static void BENCHMARK_PRINT(std::string id, int multiplier=1) {
            getInstance().printElapsed(id, multiplier);
        }

        static void BENCHMARK_PRINT(int multiplier=1) {
            getInstance().printElapsed(multiplier);
        }

        void start(std::string id) {
            map[id] = Timer();
            map[id].start();
        }

        void end(std::string id) {
            map[id].end();
        }
    
        double elapsed(std::string id) {
            map[id].elapsed();
        };

        void printElapsed(std::string id, int multiplier=1) {
            char unit[3] = {0, 0, 0};

            if (multiplier == 1) {
                unit[0] = 's';
            } else if (multiplier == 1000) {
                unit[0] = 'm';
                unit[1] = 's';
            } else if (multiplier == 1000000) {
                unit[0] = 'u';
                unit[1] = 's';
            }

            printf("Timer %s: %10.5f%s\n", id.c_str(), elapsed(id)*multiplier, unit);
        };

        void printElapsed(int multiplier=1) {
            for (auto m : map) {
                printElapsed(m.first, multiplier);
            }
        };

    private:
        MiniBenchmark() {};
        MiniBenchmark(MiniBenchmark const&);
        void operator=(MiniBenchmark const&);
        std::unordered_map<std::string, Timer> map;
};