#include "Timer.h"

/*public*/
Timer::Timer(const std::function<void(void)>& f) :workerHandler{ f } {}
Timer::Timer(const std::function<void(void)>& f, const unsigned long& i, const long repeat):
	workerHandler{ f }, interval{ std::chrono::milliseconds{ i } }, call_number{ repeat } {}

void Timer::start(bool async) {
	if (isAlive())
		return;
	alive = true;
	repeat_count = call_number;
	if (async)
		worker = std::thread(&Timer::workerFunction, this);
	else
		workerFunction();
}
void Timer::stop() {
	alive = false;
	worker.join();
}
void Timer::setWorker(const std::function<void(void)>& f) {
	workerHandler = f;
}
bool Timer::isAlive() const {
	return alive;
}
void Timer::setRepeatCount(const long int r) {
	if (isAlive())
		return;
	call_number = r;
}
long int Timer::getRepeatCount() const {
	return call_number;
}
long int Timer::getCurrentCount() const {
	return repeat_count;
}
void Timer::setInterval(const unsigned long& i) {
	if (isAlive())
		return;
	interval = std::chrono::milliseconds(i);
}
unsigned long int Timer::getInterval() const {
	return interval.count();
}
const std::function<void(void)>& Timer::callWorker() const {
	return workerHandler;
}
/*private*/
void Timer::sleepAndRun() {
	std::this_thread::sleep_for(interval);
	if (isAlive())
		callWorker()();
}
void Timer::workerFunction() {
	if (call_number == infinite) {
		while (isAlive()) {
			sleepAndRun();
		}
	}
	else {
		while (repeat_count) {
			sleepAndRun();
			--repeat_count;
		}
	}
}