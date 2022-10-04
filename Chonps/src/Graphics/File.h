#ifndef CHONPS_FILE_H
#define CHONPS_FILE_H

#include <string>
#include <fstream>
#include <sstream>

namespace Chonps
{
	std::string get_file_contents(const char* filename);

	bool get_file_contents(const char* filename, std::string& source);
}

#endif