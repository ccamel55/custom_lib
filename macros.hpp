#pragma once

#include <string>
#include <vector>

namespace lib {

    namespace array {

        //! creates a filled (std::vector) array.
        //! \tparam T type to initialize the vector with
        //! \tparam S how many elements to fill
        //! \param fill state of element to fill with
        //! \return std::vector with \a S elements
        template <typename T, size_t S>
        inline std::vector<T> create_filled_vector(const T& fill) {

            std::vector<T> ret(S);
            std::fill(ret.begin(), ret.begin() + S, fill);

            return ret;
        }
    }

    namespace string {

        //! truncates the end of a string
        //! \param str input string
        //! \param maxLength size of the truncated string
        //! \param show_ellipsis whether to append ... to the end of the truncated string
        //! \return string that is truncated to \a maxLength
        inline std::string truncate(const std::string& str, size_t maxLength, bool show_ellipsis = true) {

            if (str.length() > maxLength) {

                // append ellipsis if string is too long
                if (show_ellipsis) {
                    return str.substr(0, maxLength) + "...";
                }
                else {
                    return str.substr(0, maxLength);
                }
            }

            return str;
        }

        //! trim white space on the left
        //! \param s string that will be trimmed
        inline void trim_left(std::string& s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
        }

        //! trim white space on the right
        //! \param s string that will be trimmed
        inline void trim_right(std::string& s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), s.end());
        }

        //! trim white space from a string
        //! \param s string that will be trimmed
        inline void trim(std::string& s) {
            trim_right(s);
            trim_left(s);
        }

        //! split a string into an array
        //! \param s string that will be split
        //! \param split_char character that splits the array
        //! \return std::vector with each element being a substring of \a s split by \a split_char
        inline std::vector<std::string> split_string(const std::string& s, const char split_char = ' ') {

            std::string s_copy = s;
            std::vector<std::string> ret{};

            size_t idx = 0;

            // look for first instance of split character
            while ((idx = s_copy.find(split_char)) != std::string::npos) {

                const auto& curStr = s_copy.substr(0, idx);

                // add as substring if its not split char
                if (!curStr.empty()) {
                    ret.push_back(curStr);
                }

                s_copy.erase(0, idx + 1);
            }

            // add last string
            ret.push_back(s_copy);
            return ret;
        }
    }
}