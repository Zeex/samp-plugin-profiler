#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

#include "configreader.h"

ConfigReader::ConfigReader(const std::string &filename) 
	: loaded_(false)
{
	LoadFile(filename);
}

bool ConfigReader::LoadFile(const std::string &filename) {
	std::ifstream cfg(filename.c_str());

	if (cfg.is_open()) {
		loaded_ = true;

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
	}

	return loaded_;
}