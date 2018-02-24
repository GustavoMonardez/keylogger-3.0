#pragma once
#ifndef DATETIME_H
#define DATETIME_H

#include <string>

namespace Utilities {
	class DateTime {
	private:
		int seconds;
		int minutes;
		int hour;
		int day;
		int month;
		int year;

		void getCurrentDateTime();
		std::string getDateString() const;
		std::string getTimeString(const std::string& separator = ":") const;
	public:
		DateTime();
		DateTime(int seconds, int minutes, int hour, int day, int month, int year);
		DateTime(int day, int month, int year);

		DateTime now() const;
		std::string getDateTimeString(const std::string& separator = ":") const;
	};
}

#endif

