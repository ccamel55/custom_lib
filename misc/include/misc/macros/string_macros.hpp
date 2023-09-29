#pragma once

namespace lib
{
//! Truncates a string in place to \a max_length.
inline void TRUNCATE_STRING(std::string& string, size_t max_length, bool show_ellipsis = false)
{
	if (string.length() > max_length)
	{
		string = string.substr(0, max_length);

		if (show_ellipsis)
		{
			string += "...";
		}
	}
}

//! Trims white space left of \a string in place.
inline void L_TRIM_STRING(std::string& string)
{
	string.erase(
		string.begin(), std::find_if(string.begin(), string.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

//! Trims white space right of \a string in place.
inline void R_TRIM_STRING(std::string& string)
{
	string.erase(
		std::find_if(string.rbegin(), string.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
		string.end());
}

//! Trims white space of \a string in place.
inline void TRIM_STRING(std::string& string)
{
	L_TRIM_STRING(string);
	R_TRIM_STRING(string);
}

//! Splits \a string using the character \a split. Returns a vector of split strings.
inline std::vector<std::string> SPLIT_STRING(const std::string& string, const char split = ' ')
{
	std::vector<std::string> split_strings = {};

	size_t first_idx = 0;
	size_t second_idx = 0;

	// while we can keep finding split character....
	while ((second_idx = string.substr(first_idx, std::string::npos).find(split)) != std::string::npos)
	{
		const auto& sub_string = string.substr(first_idx, second_idx);

		// second_idx should be at the index of the split character, therefore substring should not include split
		// character if split character is included then it will be size 0 or empty, thus we skip.
		if (!sub_string.empty())
		{
			split_strings.push_back(sub_string);
		}

		// move onto next part of the string
		first_idx = second_idx + 1;
	}

	// add last string
	split_strings.push_back(string.substr(first_idx, std::string::npos));

	return split_strings;
}
}  // namespace lib