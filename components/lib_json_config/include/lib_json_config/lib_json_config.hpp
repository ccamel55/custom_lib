#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <nlohmann/json.hpp>
#include <lib_json_config/json_config.hpp>

namespace lib::json_config
{
//! Config manager using json. Each config can have multiple json files associated with it.
class config_manager
{
public:
	config_manager(const std::filesystem::path& config_directory, const std::string& file_extension);
	~config_manager() = default;

	//! Attempt to remove the config named \a config_name.
	void remove_config(const std::string& config_name);

	//! Attempt to save the config named \a config_name.
	void save_config(const std::string& config_name);

	//! Attempt to load the config named \a config_name.
	void load_config(const std::string& config_name) const;

	//! Register a new sub config to the config named \a sub_config_name.
	void register_config(const std::string& sub_config_name, std::weak_ptr<json_config>&& sub_config);

	//! Return current configs.
	[[nodiscard]] const std::unordered_set<std::string>& get_current_configs() const;

private:
	void refresh_configs();

private:
	std::string _file_extension = {};
    std::filesystem::path _config_directory = {};

	std::unordered_set<std::string> _current_configs = {};
	std::unordered_map<std::string, std::weak_ptr<json_config>> _configs = {};

};
}  // namespace lib::json_config