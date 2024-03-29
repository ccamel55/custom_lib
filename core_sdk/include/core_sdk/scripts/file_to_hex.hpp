#pragma once

#include <fstream>
#include <vector>
#include <string>

#include <core_sdk/logger/logger.hpp>

namespace lib::scripts
{
inline void file_to_hex(std::string array_name,  const std::string& file_path, const std::string& destination_path)
{
	try
	{
		// open the file as bytes
		auto in_file = std::ifstream(file_path, std::ios::binary);

		if (!in_file.is_open())
		{
			lib_log_e("file_to_hex: could not open input file");
			return;
		}

		// read bytes into vector
		std::vector<uint8_t> file_as_bytes((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
		in_file.close();

		// open/create the output file
		auto out_file = std::ofstream(destination_path, std::ofstream::out | std::ofstream::trunc);

		if (!out_file.is_open())
		{
			lib_log_e("file_to_hex: could not open output file");
			return;
		}

		// write as an array
		out_file << "#include <array>" << "\n";
		out_file << "\n";
		out_file << "constexpr std::array<uint8_t, " << file_as_bytes.size() << "> " << array_name << " = " << "\n";
		out_file << "{";

		out_file << "\t";
		for (size_t i = 0; i < file_as_bytes.size(); i++)
		{
			// every 20 bytes we make a new line
			if (i % 20 == 0)
			{
				out_file << "\n";
				out_file << "\t";
			}

			out_file << std::showbase << std::hex << static_cast<int16_t>(file_as_bytes.at(i)) << ", ";
		}
		out_file << "\n";

		out_file << "};";
		out_file.close();
	}
	catch (const std::exception& e)
	{
		lib_log_e("file_to_hex: could not open file happened: {}", e.what());
		return;
	}
}
}  // namespace lib::scripts