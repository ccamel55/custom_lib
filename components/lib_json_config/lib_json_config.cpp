#include "fmt/ostream.h"

#include <core_sdk/logger.hpp>
#include <core_sdk/macros/string_macros.hpp>

#include <lib_json_config/lib_json_config.hpp>

#include <nlohmann/json.hpp>

using namespace lib::json_config;

namespace
{
bool is_config_name_valid(const std::string& name)
{
	if (name.find('.') != std::string::npos)
	{
		return false;
	}

	return true;
}
}

config_manager::config_manager(const std::filesystem::path& config_directory, const std::string& file_extension)
	: _file_extension(file_extension), _config_directory(config_directory)
{
	// remove . if it starts with a dot
	lib::trim_string(_file_extension);

	if (_file_extension[0] == '.')
	{
		// remove first character
		_file_extension = _file_extension.substr(1, _file_extension.size() - 1);
	}

	if (!std::filesystem::exists(config_directory))
	{
		lib_log_d("config_manager: directory does not exist, creating it ({})", config_directory.string());
		std::filesystem::create_directory(config_directory);
	}
	else
	{
		refresh_configs();
	}
}

void config_manager::remove_config(const std::string& config_name)
{
	assert(is_config_name_valid(config_name) == true);

	for (const auto& sub_config : _configs)
	{
		const auto& full_config_name = config_name + "." + sub_config.first;
		const auto& full_config_path = std::filesystem::path(_config_directory / (full_config_name + "." + _file_extension));

		if (!std::filesystem::exists(full_config_path))
		{
			continue;
		}

		std::filesystem::remove(full_config_path);
		lib_log_d("config_manager: removed config {}", full_config_name);
	};

	_current_configs.erase(config_name);
}

void config_manager::save_config(const std::string& config_name)
{
	assert(is_config_name_valid(config_name) == true);

	for (const auto& sub_config : _configs)
	{
		const auto json_sub_config = sub_config.second.lock();
		assert(json_sub_config != nullptr);

		const auto& full_config_name = config_name + "." + sub_config.first;
		const auto& full_config_path = std::filesystem::path(_config_directory / (full_config_name + "." + _file_extension));

		nlohmann::json json_file = {};

		// serialize contents to json :D
		for (const auto& var : json_sub_config->_values)
		{
			const auto& [name, value] = var;

			nlohmann::json json_entry = {};

			json_entry["name"] = name;
			json_entry["type"] = value.type;

			switch (value.type)
			{
			case value_type::boolean: {
				json_entry["value"] = *static_cast<bool*>(value.value_ptr);
				break;
			}
			case value_type::integer: {
				json_entry["value"] = *static_cast<int*>(value.value_ptr);
				break;
			}
			case value_type::floating_point: {
				json_entry["value"] = *static_cast<float*>(value.value_ptr);
				break;
			}
			case value_type::string: {
				json_entry["value"] = *static_cast<std::string*>(value.value_ptr);
				break;
			}
			case value_type::boolean_array: {
				json_entry["value"] = *static_cast<std::vector<bool>*>(value.value_ptr);
				break;
			}
			case value_type::integer_array: {
				json_entry["value"] = *static_cast<std::vector<int>*>(value.value_ptr);
				break;
			}
			case value_type::floating_point_array: {
				json_entry["value"] = *static_cast<std::vector<float>*>(value.value_ptr);
				break;
			}
			case value_type::string_array: {
				json_entry["value"] = *static_cast<std::vector<std::string>*>(value.value_ptr);
				break;
			}
			}

			// write into main json file
			json_file.push_back(json_entry);
		}

		std::ofstream config_ostream(full_config_path, std::ios::trunc);
		assert(config_ostream.is_open() == true);

		// 4 is indent
		config_ostream << json_file.dump(4);
		config_ostream.close();

		lib_log_d("config_manager: saving config {}", full_config_name);
	};

	_current_configs.insert(config_name);
}

void config_manager::load_config(const std::string& config_name) const
{
	assert(is_config_name_valid(config_name) == true);

	for (const auto& sub_config : _configs)
	{
		const auto& full_config_name = config_name + "." + sub_config.first;
		const auto& full_config_path = std::filesystem::path(_config_directory / (full_config_name + "." + _file_extension));

		if (!std::filesystem::exists(full_config_path))
		{
			lib_log_w("config_manager: could not find config {}, skipping", full_config_name);
			continue;
		}

		const auto json_sub_config = sub_config.second.lock();
		assert(json_sub_config != nullptr);

		std::ifstream config_ostream(full_config_path);
		assert(config_ostream.is_open() == true);

		const auto& json_file = nlohmann::json::parse(config_ostream, nullptr, false);
		config_ostream.close();

		if (json_file.is_discarded())
		{
			lib_log_e("config_manager: could open as json, womp womp", full_config_name);
			assert(false);
		}

		// deserialize from json
		for (const auto& var : json_file)
		{
			if (!json_sub_config->_values.contains(var["name"].get<std::string>()))
			{
				continue;
			}

			auto& config_var = json_sub_config->_values.at(var["name"].get<std::string>());

			// make sure types are the same
			assert(var["type"].get<value_type>() == config_var.type);

			switch (config_var.type)
			{
			case value_type::boolean: {
				*static_cast<bool*>(config_var.value_ptr) = var["value"].get<bool>();
				break;
			}
			case value_type::integer: {
				*static_cast<int*>(config_var.value_ptr) = var["value"].get<int>();
				break;
			}
			case value_type::floating_point: {
				*static_cast<float*>(config_var.value_ptr) = var["value"].get<float>();
				break;
			}
			case value_type::string: {
				*static_cast<std::string*>(config_var.value_ptr) = var["value"].get<std::string>();
				break;
			}
			case value_type::boolean_array: {
				*static_cast<std::vector<bool>*>(config_var.value_ptr) = var["value"].get<std::vector<bool>>();
				break;
			}
			case value_type::integer_array: {
				*static_cast<std::vector<int>*>(config_var.value_ptr) = var["value"].get<std::vector<int>>();
				break;
			}
			case value_type::floating_point_array: {
				*static_cast<std::vector<float>*>(config_var.value_ptr) = var["value"].get<std::vector<float>>();
				break;
			}
			case value_type::string_array: {
				*static_cast<std::vector<std::string>*>(config_var.value_ptr) = var["value"].get<std::vector<std::string>>();
				break;
			}
			}
		}

		lib_log_d("config_manager: loaded config {}", full_config_name);
	}
}

void config_manager::register_config(const std::string& sub_config_name, std::weak_ptr<json_config>&& sub_config)
{
	lib_log_d("config_manager: registered new sub config {}", sub_config_name);
	_configs[sub_config_name] = std::move(sub_config);
}

void config_manager::refresh_configs()
{
	_current_configs.clear();

	const auto files = std::filesystem::directory_iterator(_config_directory);
	std::ranges::for_each(files, [&](const auto& file)
	{
		// check for two periods as we always have a subextension in our configs
		const auto file_name = std::filesystem::path(file).lexically_relative(_config_directory);
		const auto period_count = std::ranges::count(file_name.string(), '.');

		if (period_count != 2 || file_name.extension().string() != ("." + _file_extension))
		{
			return;
		}

		// name of config is string right before first period
		const auto config_name = file_name.string().substr(0, file_name.string().find_first_of('.'));

		if (_current_configs.contains(config_name))
		{
			return;
		}

		lib_log_d("config_manager: found existing config {}", config_name);
		_current_configs.insert(config_name);
	});

	lib_log_d("config_manager: found {} existing configs", _current_configs.size());
}

const std::unordered_set<std::string>& config_manager::get_current_configs() const
{
	return _current_configs;
}