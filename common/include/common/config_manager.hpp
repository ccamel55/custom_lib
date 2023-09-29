#pragma once

#include <filesystem>
#include <string>

#include <common/hash/fnv1a.hpp>
#include <nlohmann/json.hpp>

namespace lib::common
{
//! Config manager using json.
class config_manager
{
public:
	config_manager(const std::filesystem::path& config_directory, const std::string& file_extension);

	//! Create a new config in the config directory with \a config_name.
	void create_config(const std::string& config_name);

	//! Attempt to remove the config named \a config_name.
	void remove_config(const std::string& config_name);

	//! Attempt to save the config named \a config_name.
	void save_config(const std::string& config_name);

	//! Attempt to load the config named \a config_name.
	void load_config(const std::string& config_name);

	//! Attempt get all the configs in the current config directory.
	void refresh_config_list();

private:
};
}  // namespace lib::common