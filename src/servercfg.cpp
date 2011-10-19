#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

#include "servercfg.h"

ServerCfg::ServerCfg(const std::string &filename) {
	std::ifstream cfg(filename.c_str());

	if (cfg.is_open()) {
		std::string line;
		while (std::getline(cfg, line, '\n')) {
			std::stringstream linestream(line);

			// Get first word in the line
			std::string name;
			std::getline(linestream, name, ' ');

			// Get the rest
			std::string value;
			std::getline(linestream, value, '\n');

			options_[name] = value;
		}
	} else {
		throw std::runtime_error("Couldn't open file " + filename);
	}
}

std::string ServerCfg::GetOption(const std::string &name) {
	return options_[name];
}

long ServerCfg::GetOptionAsInt(const std::string &name, int defaultValue) {
	std::string value = GetOption(name);
	if (value.empty()) {
		return defaultValue;
	}
	return strtol(value.c_str(), 0, 10);
}

double ServerCfg::GetOptionAsDouble(const std::string &name, double defaultValue) {
	std::string value = GetOption(name);
	if (value.empty()) {
		return defaultValue;
	}
	return strtod(value.c_str(), 0);
}