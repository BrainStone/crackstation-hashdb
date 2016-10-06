#ifndef CRACKSTATION_PROGRESSBAR_H
#define CRACKSTATION_PROGRESSBAR_H

#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>

#include <sys/ioctl.h>

#include "util.h"

class ProgressBar {
public:
	typedef std::chrono::high_resolution_clock HRC;
	typedef std::chrono::duration<double> duration;

	ProgressBar();
	ProgressBar( const std::vector<std::pair<std::string, size_t>> & segments, std::function<std::string( double )> extraDataGenerator = std::function<std::string( double )>() );
	~ProgressBar();

	static winsize getConsoleSize();
	static unsigned short getConsoleHeight();
	static unsigned short getConsoleWidth();

	void init( const std::vector<std::pair<std::string, size_t>> & segments, std::function<std::string( double )> extraDataGenerator = std::function<std::string( double )>() );
	void start();
	void finish( bool blocking = false );

	void updateProgress( size_t numSegment, double progress );
	void updateProgress( size_t numSegment, size_t workDone, size_t workToDo );

	int getNumSegments();

private:
	static constexpr long long defaultTimeout = 100;

	template<typename T>
	static double div( const T& lhs, const T& rhs );
	static std::string getPercentString( double progress, size_t width );

	bool initialized;
	std::atomic<bool> isRunning;
	size_t totalWeight;
	HRC::time_point startTime;
	std::unique_ptr<std::thread> thread;
	std::atomic<size_t> activeSegment;
	std::vector<std::string> segmentNames;
	std::vector<size_t> segmentWeights;
	std::vector<double> segmentProgresses;
	std::mutex segmentsMutex;
	std::function<std::string( double )> extraDataGenerator;

	void renderThread();
	void renderBar( double progress );
	double getTotalProgress();
	const std::string & getActiveSegment();
};

std::ostream & operator<<( std::ostream & os, ProgressBar::duration dSeconds );

#endif
