#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

int main(const int argc, const char* argv[]) {
    try {
        // 1st arg == current file path (default)
        // 2nd arg == source file (user)
        if (argc < 2) {
            throw std::runtime_error("Source file was not provided");
        }

        const std::filesystem::path file(argv[1]);

        std::vector<uint8_t> file_bytes;

        {
            std::ifstream file_handle(file, std::ios::binary);

            if (!file_handle.is_open()) {
                throw std::runtime_error("Could not open source file: " + file.string());
            }

            std::vector<char> tmp_vec{
                std::istreambuf_iterator<char>{file_handle},
                std::istreambuf_iterator<char>{}
            };

            file_bytes.resize(tmp_vec.size());

            std::memcpy(file_bytes.data(), tmp_vec.data(), tmp_vec.size());
        }

        std::filesystem::path output_file_name = file;
        output_file_name.replace_extension(".hpp");

        {
            std::ofstream output_handle(output_file_name, std::ofstream::trunc);

            if (!output_handle.is_open()) {
                throw std::runtime_error("Could not open output file: " + output_file_name.string());
            }

            output_handle << "static std::array<uint8_t, " << file_bytes.size() << "> arr = " << std::endl;
            output_handle << "{" << "\t";

            for (size_t i = 0; i < file_bytes.size(); i++) {

                if ( i % 16 == 0 ) {
                    output_handle << std::endl;
                    output_handle << "\t";
                }

                output_handle << std::setw(2) << std::setfill('0') << std::hex << static_cast<int16_t>(file_bytes.at(i)) << ", ";
            }

            output_handle << std::endl << "};";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Could not open file - " << e.what() << "\n";
        return -1;
    }

    return 0;
}