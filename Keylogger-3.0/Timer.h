#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>
#include <functional>

class Timer {
private:
	std::thread worker;
	bool alive{ false };
	long int call_number{ -1L };
	long int repeat_count{ -1L };
	std::chrono::milliseconds interval{ std::chrono::milliseconds(0) };
	std::function<void(void)> workerHandler{};

	void sleepAndRun();
	void workerFunction();
public:
	static const long int infinite{ -1L };

	Timer() {}
	Timer(const std::function<void(void)>& f);
	Timer(const std::function<void(void)>& f, const unsigned long& i, const long repeat = Timer::infinite);

	void start(bool async = true);
	void stop();
	void setWorker(const std::function<void(void)>& f);
	bool isAlive() const;
	void setRepeatCount(const long int r);
	long int getRepeatCount() const;
	long int getCurrentCount() const;
	void setInterval(const unsigned long& i);
	unsigned long int getInterval() const;
	const std::function<void(void)>& callWorker() const;
};

#endif