#ifndef __TESTBENCH_H_
#define __TESTBENCH_H_

#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <string>
#include <set>
#include <sstream>

#ifdef USE_CHRONO
#include <chrono>
namespace ch = std::chrono;
#else 
// Chrono is not available on g++ 4.4.7
// Use Unix Time, sometimes fails but its the only timer that provides proper microseconds
#include <sys/time.h>
#endif


// All the data our Benchmark should store for 1 test.
struct TestData {
	long long Time;
	int Steps;

	TestData() 
		: Time(0)
		, Steps(0) {}

	TestData(long long time, int steps)
		: Time(time)
		, Steps(steps) {}
};

void operator+=(TestData& rhs, const TestData& lhs) {
	rhs.Time += lhs.Time;
	rhs.Steps += lhs.Steps;
}

bool operator<(TestData& rhs, const TestData& lhs) {
	if (rhs.Time == lhs.Time) {
		return rhs.Steps < lhs.Steps;
	}
	return rhs.Time < lhs.Time;
}

// Struct to hold the benchmark results.
// Implementation can switch between chrono / unix time through defining USE_CHRONO.
// Chrono is a bit more accurate but not available on older c++ versions.
// All time values (stored as long long) are in microseconds.

struct Benchmark {

private:
	std::vector<TestData> DijkTime;
	std::vector<TestData> AstarTime;

#ifdef USE_CHRONO
	ch::time_point<ch::system_clock> StartTime;
	void RestartTimer() {
		StartTime = ch::system_clock::now();
	}

	long long GetCurrent() const {
		return ch::duration_cast<ch::microseconds>(ch::system_clock::now() - StartTime).count();
	}
#else
	long StartTime;

	long GetUnixMicros() const {
		struct timeval TimeVal;
		struct timezone TimeZone;
		if (gettimeofday(&TimeVal, &TimeZone)) {
            return 0;
        }
		return TimeVal.tv_usec;
	}

	void RestartTimer() {
		StartTime = GetUnixMicros();
	}

	long long GetCurrent() const {
        long UnixMicros = GetUnixMicros();
        if (UnixMicros <= 0 || StartTime <= 0) {
            return 0;
        }
        return UnixMicros - StartTime;
	}
#endif

public:

	// Internal,  formats and prints a line with 2 times and their difference.
	void PrintBenchLine(TestData DijkT, TestData AstarT) {
		static std::string TimestepStr = " micros";
		std::cout << std::right;
		std::cout << "\n\t| Dijk: " << std::setw(7) << DijkT.Time << TimestepStr << " | " << std::setw(3) << DijkT.Steps << " steps"
				 << "\n\t| A*  : " << std::setw(7) << AstarT.Time << TimestepStr << " | " << std::setw(3) << AstarT.Steps << " steps";

        if (DijkT.Time <= 0 || AstarT.Time <= 0) {
		    std::cout << "\t- Invalid timer result detected. -\n";
            return;
        }

		long long Hi = std::max(DijkT.Time, AstarT.Time);
		long long Lo = std::max(std::min(DijkT.Time, AstarT.Time), 1LL);
		
		int Percent = (int)std::floor(((float)Hi / Lo) * 100.f + 0.5f) - 100;
		long long AbsDiff = Hi - Lo;

		std::string Who = DijkT < AstarT ? "Dijk" : "A* ";
		int StepDiff = DijkT < AstarT ? DijkT.Steps - AstarT.Steps : AstarT.Steps - DijkT.Steps;
		std::cout << "\n\t| " << Who << " is faster by: " << std::setw(3) << Percent << "% ( " << AbsDiff << TimestepStr << " ) Step Difference: " << StepDiff << " \n" ;
	}

public:

	void Reset() {
		DijkTime.clear();
		AstarTime.clear();
	}

	void StartTest() {
		RestartTimer();
	}

	// The Dijkstra test has finished
	// does not start the timer instantly.
	void SwitchTest(int Steps) {
		long long Duration = GetCurrent();
		DijkTime.push_back(TestData(Duration, Steps));
	}

    void StopTest(int Steps) {
		long long Duration = GetCurrent();
		AstarTime.push_back(TestData(Duration, Steps));
	}

	// Print the last added test.
	void PrintLast() {
		size_t Index = DijkTime.size() - 1;
		PrintBenchLine(DijkTime[Index], AstarTime[Index]);
	}

	// Calculate and print total stats.
	void Print() {
        bool ContainsInvalidResult = false;
		TestData DijkTotal;
		TestData AstarTotal;
		for (int i = 0; i < DijkTime.size(); ++i) {
            if (DijkTime[i].Time > 0 && AstarTime[i].Time > 0) {
                DijkTotal += DijkTime[i];
                AstarTotal += AstarTime[i];
            }
            else {
                ContainsInvalidResult = true;
            }
		}

		std::cout << "Totals:";
		PrintBenchLine(DijkTotal, AstarTotal);
        std::cout << "\n";

        if (ContainsInvalidResult) {
            std::cout << "Unix timer does not always guarantee returning a valid time.\n"
                    << "It is HIGHLY recommended to use chrono if possible for better results.\n"
                    << "Results detected with invalid times where not included in the total.\n\n"
                    << "Rerunning the program usually fixes this.\n";
        }
	}
};

#endif //__TESTBENCH_H_