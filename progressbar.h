#ifndef CRACKSTATION_PROGRESSBAR_H
#define CRACKSTATION_PROGRESSBAR_H

#include <algorithm>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
#include <unistd.h>

#include <sys/ioctl.h>

class ProgressBar {
public:
	typedef std::lock_guard<std::mutex> scoped_lock;
	typedef std::chrono::high_resolution_clock HRC;
	typedef std::chrono::duration<double> duration;
	typedef std::tuple<std::string, size_t, std::function<std::string( double )>> SegmentBase;
	typedef std::function<std::string( double )> extraDataFunc;

	class InvalidStateException;
	class Segment;

	ProgressBar();
	ProgressBar( const std::vector<Segment> & segments, bool displaySubProgress );
	ProgressBar( const std::vector<Segment> & segments, extraDataFunc extraDataGenerator = extraDataFunc(), bool displaySubProgress = false );
	~ProgressBar();

	static winsize getConsoleSize();
	static unsigned short getConsoleHeight();
	static unsigned short getConsoleWidth();

	void init( const std::vector<Segment> & segments, bool displaySubProgress );
	void init( const std::vector<Segment> & segments, extraDataFunc extraDataGenerator = extraDataFunc(), bool displaySubProgress = false );
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
	static std::string centerString( size_t width, const std::string& str );

	bool initialized;
	std::atomic<bool> isRunning;
	size_t totalWeight;
	HRC::time_point startTime;
	std::unique_ptr<std::thread> thread;
	std::atomic<size_t> activeSegment;
	std::vector<Segment> segments;
	std::vector<double> segmentProgresses;
	std::vector<HRC::time_point> segmentStartTimes;
	std::mutex segmentsMutex;
	extraDataFunc extraDataGenerator;
	bool displaySubProgress;

	void renderThread();
	void renderBar( double progress );
	double getTotalProgress();
	const Segment & getActiveSegment();
};

std::ostream & operator<<( std::ostream & os, ProgressBar::duration dSeconds );

class ProgressBar::InvalidStateException : public std::runtime_error {
	// Explicitly inheriting all constructors
	using std::runtime_error::runtime_error;
};

class ProgressBar::Segment : public ProgressBar::SegmentBase {
public:
	Segment() = default;
	Segment( std::string title, size_t weight, extraDataFunc extraDataGenerator = extraDataFunc() );

	const std::string & getTitle() const;
	const size_t & getWeight() const;
	const extraDataFunc & getExtraDataGenerator() const;

	bool hasExtraDataGenerator() const;
};

#endif
