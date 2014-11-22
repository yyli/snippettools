#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
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
        typedef std::pair<std::string, Timer> TimerIdPair;
        static MiniBenchmark& getInstance() {
            static MiniBenchmark instance;
            return instance;
        }

        static void BENCHMARK_BEGIN(std::string id) {
            getInstance().start(id);
        }

        static void BENCHMARK_END(std::string id) {
            getInstance().end(id);
        }

        static double BENCHMARK_RESULT(std::string id) {
            return getInstance().elapsed(id);
        }

        static void BENCHMARK_PRINT_MS(std::string id) {
            getInstance().printElapsed(id, 1000);
        }

        static void BENCHMARK_PRINT_MS() {
            getInstance().printElapsed(1000);
        }

        static void BENCHMARK_PRINT_US(std::string id) {
            getInstance().printElapsed(id, 1000000);
        }

        static void BENCHMARK_PRINT_US() {
            getInstance().printElapsed(1000000);
        }

        static void BENCHMARK_PRINT(std::string id, int multiplier=1) {
            getInstance().printElapsed(id, multiplier);
        }

        static void BENCHMARK_PRINT(int multiplier=1) {
            getInstance().printElapsed(multiplier);
        }

        static void BENCHMARK_CLEAR() {
            getInstance().clear();
        }

        void start(std::string id) {
            if (map.find(id) == map.end()) {
                arr.push_back(TimerIdPair(id, Timer()));
                map[id] = arr.size() - 1;
            }

            arr[map[id]].second.start();
        }

        void end(std::string id) {
            if (map.find(id) == map.end())
                throw std::runtime_error(id + " Timer hasn't been started");

            arr[map[id]].second.end();
        }
    
        double elapsed(std::string id) {
            if (map.find(id) == map.end())
                throw std::runtime_error(id + " Timer hasn't been started");

            return arr[map[id]].second.elapsed();
        };

        void printElapsed(std::string id, int multiplier=1) {
            printf("%s: %11.5f%s\n", id.c_str(), elapsed(id)*multiplier, getUnit(multiplier).c_str());
        };

        void printElapsed(int multiplier=1) {
            printf("===== MiniBenchmark Timers =====\n");
            size_t max_length = 0;
            for (auto m : map) {
                if (m.first.size() > max_length)
                    max_length = m.first.size();
            }

            for (size_t i = 0; i < arr.size(); i++) {
                printf("    ");
                std::string out_id = arr[i].first;
                out_id.insert(out_id.end(), max_length - out_id.size(), ' ');
                printf("%s: %11.5f%s\n", out_id.c_str(), arr[i].second.elapsed()*multiplier, getUnit(multiplier).c_str());
            }
        };

        void clear() {
            map.clear();
            arr.clear();
        }

    private:
        MiniBenchmark() {};
        MiniBenchmark(MiniBenchmark const&);
        void operator=(MiniBenchmark const&);

        std::string getUnit(int multiplier) {
            std::string unit = "";
            if (multiplier == 1) {
                unit = "s";
            } else if (multiplier == 1000) {
                unit = "ms";
            } else if (multiplier == 1000000) {
                unit = "us";
            }

            return unit;
        }
        std::unordered_map<std::string, int> map;
        std::vector<TimerIdPair> arr;
};