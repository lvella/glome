#include "data_file.hpp"

#include "config.hpp"

#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <tuple>

#include <sstream>
#include <SDL.h>

static std::tuple<std::filesystem::path, DataFile>
assemble_and_check_path(const std::string& path)
{
	std::filesystem::path full_path(DATA_DIR);
	full_path /= path;

	errno = 0;
	DataFile file(full_path);
	if(!file) {
		std::stringstream error_reason;
		error_reason << "FATAL ERROR: Failed to open game file: "
			<< full_path << '\n';
		if(errno) {
			error_reason << "  Reason: " << strerror(errno) << '\n';
		}
		error_reason << "  Please be sure game data is in the " <<
			(full_path.is_absolute() ? "absolute": "relative") << " path "
			"\"" << DATA_DIR << "\" and ";
		if(full_path.is_absolute()) {
			error_reason << "you have permission to read it.";
		} else {
			error_reason << "it is accessible from game's working directory.";
		}
		std::cerr << error_reason.str() << std::endl;
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"FATAL_ERROR",
			error_reason.str().c_str(),
			NULL);
		exit(1);
	}

	return {full_path, std::move(file)};
}


std::filesystem::path get_data_path(const std::string& rel_path)
{
	return std::get<0>(assemble_and_check_path(rel_path));
}

DataFile load_data_file(const std::string& rel_path)
{
	return std::get<1>(assemble_and_check_path(rel_path));
}
