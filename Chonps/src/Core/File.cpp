#include "cepch.h"
#include "File.h"

namespace Chonps
{
	std::string get_file_contents(const char* filename)
	{
		std::ifstream in(filename, std::ios::binary);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		CHONPS_CORE_ERROR("ERROR: FILE: Cannot open file: {0}", filename);
		return std::string();
	}

	bool get_file_contents(const char* filename, std::string* source)
	{
		std::ifstream in(filename, std::ios::binary);
		if (in)
		{
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			*source = contents;
			return true;
		}
		CHONPS_CORE_ERROR("ERROR: FILE: Cannot open file: {0}", filename);
		return false;
	}

	std::vector<char> get_file_bin(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			CHONPS_CORE_ERROR("ERROR: FILE: Cannot open file: {0}", filename);
			return std::vector<char>();
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
}