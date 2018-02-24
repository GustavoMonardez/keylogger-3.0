#include "DateTime.h"
#include <ctime>
#include <string>

namespace Utilities {
	DateTime::DateTime() {
		getCurrentDateTime();
	}
	DateTime::DateTime(int seconds, int minutes, int hour, int day, int month, int year):
		seconds{ seconds }, minutes{ minutes }, hour{ hour }, day{ day }, month{ month }, year{ year } {}

	DateTime::DateTime(int day, int month, int year): 
		seconds{ 0 }, minutes{ 0 }, hour{ 0 }, day{ day }, month{ month }, year{ year } {}
	/*public*/
	DateTime DateTime::now() const {
		return DateTime();
	}
	std::string DateTime::getDateTimeString(const std::string& separator) const {
		return getDateString() + " " + getTimeString(separator);
	}
	/*private*/
	void DateTime::getCurrentDateTime() {
		time_t secs;
		time(&secs);
		tm info;
		localtime_s(&info, &secs);

		seconds = info.tm_sec;
		minutes = info.tm_min;
		hour = info.tm_hour;
		day = info.tm_mday;
		month = info.tm_mon + 1;
		year = 1900 + info.tm_year;
	}
	std::string DateTime::getDateString() const {
		/*format DD:MM:YYYY*/
		return (day < 10 ? "0" : "") + std::to_string(day) +
			(month < 10 ? ".0" : ".") + std::to_string(month) +
			"." + std::to_string(year);
	}
	std::string DateTime::getTimeString(const std::string& separator) const {
		/*format HH:MM:SS*/
		return (hour < 10 ? "0" : "") + std::to_string(hour) + separator +
			(minutes < 10 ? "0" : "") + std::to_string(minutes) + separator +
			(seconds < 10 ? separator : "") + std::to_string(seconds);
	}
}