#pragma once

#include <string>
#include <fstream>
#include <filesystem>

class DataFile: public std::ifstream
{
public:
	using std::ifstream::ifstream;

	// Fuck C++: it forces me to implement the move constructor.
	DataFile(DataFile&& other):
		std::ifstream(std::move(other))
	{}

	template<typename T>
	bool read_binary(T* var, std::streamsize nmemb = 1)
	{
		const std::streamsize count = nmemb * sizeof *var;
		read(reinterpret_cast<char*>(var), count);
		return gcount() == count;
	}
};

/** Given relative path from DATA_DIR, checks and returns the full path.*/
std::filesystem::path get_data_path(const std::string& rel_path);

/** Given relative path from DATA_DIR, load data file. */
DataFile load_data_file(const std::string& rel_path);
