#include <lib_window_creation/helpers.hpp>

#include <unordered_set>
#include <map>
#include <ranges>

using namespace lib::window_creation;

#ifndef DEF_LIB_INPUT_off
lib::input::key_button helpers::glfw_to_input_key(int key)
{
    switch (key)
    {
        case GLFW_KEY_TAB: return lib::input::key_button::tab;
        case GLFW_KEY_LEFT: return lib::input::key_button::leftarrow;
        case GLFW_KEY_RIGHT: return lib::input::key_button::rightalt;
        case GLFW_KEY_UP: return lib::input::key_button::uparrow;
        case GLFW_KEY_DOWN: return lib::input::key_button::downarrow;
        case GLFW_KEY_PAGE_UP: return lib::input::key_button::pageup;
        case GLFW_KEY_PAGE_DOWN: return lib::input::key_button::pagedown;
        case GLFW_KEY_HOME: return lib::input::key_button::home;
        case GLFW_KEY_END: return lib::input::key_button::end;
        case GLFW_KEY_INSERT: return lib::input::key_button::insert;
        case GLFW_KEY_DELETE: return lib::input::key_button::delete_key;
        case GLFW_KEY_BACKSPACE: return lib::input::key_button::backspace;
        case GLFW_KEY_SPACE: return lib::input::key_button::space;
        case GLFW_KEY_ENTER: return lib::input::key_button::enter;
        case GLFW_KEY_ESCAPE: return lib::input::key_button::escape;
        case GLFW_KEY_APOSTROPHE: return lib::input::key_button::apostrophe;
        case GLFW_KEY_COMMA: return lib::input::key_button::comma;
        case GLFW_KEY_MINUS: return lib::input::key_button::minus;
        case GLFW_KEY_PERIOD: return lib::input::key_button::period;
        case GLFW_KEY_SLASH: return lib::input::key_button::slash;
        case GLFW_KEY_SEMICOLON: return lib::input::key_button::semicolon;
        case GLFW_KEY_EQUAL: return lib::input::key_button::equal;
        case GLFW_KEY_LEFT_BRACKET: return lib::input::key_button::leftbracket;
        case GLFW_KEY_BACKSLASH: return lib::input::key_button::backslash;
        case GLFW_KEY_RIGHT_BRACKET: return lib::input::key_button::rightbracket;
        case GLFW_KEY_GRAVE_ACCENT: return lib::input::key_button::graveaccent;
        case GLFW_KEY_CAPS_LOCK: return lib::input::key_button::capslock;
        case GLFW_KEY_SCROLL_LOCK: return lib::input::key_button::scrolllock;
        case GLFW_KEY_NUM_LOCK: return lib::input::key_button::numlock;
        case GLFW_KEY_PRINT_SCREEN: return lib::input::key_button::printscreen;
        case GLFW_KEY_PAUSE: return lib::input::key_button::pause;
        case GLFW_KEY_KP_0: return lib::input::key_button::keypad0;
        case GLFW_KEY_KP_1: return lib::input::key_button::keypad1;
        case GLFW_KEY_KP_2: return lib::input::key_button::keypad2;
        case GLFW_KEY_KP_3: return lib::input::key_button::keypad3;
        case GLFW_KEY_KP_4: return lib::input::key_button::keypad4;
        case GLFW_KEY_KP_5: return lib::input::key_button::keypad5;
        case GLFW_KEY_KP_6: return lib::input::key_button::keypad6;
        case GLFW_KEY_KP_7: return lib::input::key_button::keypad7;
        case GLFW_KEY_KP_8: return lib::input::key_button::keypad8;
        case GLFW_KEY_KP_9: return lib::input::key_button::keypad9;
        case GLFW_KEY_KP_DECIMAL: return lib::input::key_button::keypaddecimal;
        case GLFW_KEY_KP_DIVIDE: return lib::input::key_button::keypaddivide;
        case GLFW_KEY_KP_MULTIPLY: return lib::input::key_button::keypadmultiply;
        case GLFW_KEY_KP_SUBTRACT: return lib::input::key_button::keypadsubtract;
        case GLFW_KEY_KP_ADD: return lib::input::key_button::keypadadd;
        case GLFW_KEY_KP_ENTER: return lib::input::key_button::keypadenter;
        case GLFW_KEY_KP_EQUAL: return lib::input::key_button::keypadequal;
        case GLFW_KEY_LEFT_SHIFT: return lib::input::key_button::leftshift;
        case GLFW_KEY_LEFT_CONTROL: return lib::input::key_button::leftctrl;
        case GLFW_KEY_LEFT_ALT: return lib::input::key_button::leftalt;
        case GLFW_KEY_LEFT_SUPER: return lib::input::key_button::leftsuper;
        case GLFW_KEY_RIGHT_SHIFT: return lib::input::key_button::rightshift;
        case GLFW_KEY_RIGHT_CONTROL: return lib::input::key_button::rightctrl;
        case GLFW_KEY_RIGHT_ALT: return lib::input::key_button::rightalt;
        case GLFW_KEY_RIGHT_SUPER: return lib::input::key_button::rightsuper;
        case GLFW_KEY_MENU: return lib::input::key_button::menu;
        case GLFW_KEY_0: return lib::input::key_button::num_0;
        case GLFW_KEY_1: return lib::input::key_button::num_1;
        case GLFW_KEY_2: return lib::input::key_button::num_2;
        case GLFW_KEY_3: return lib::input::key_button::num_3;
        case GLFW_KEY_4: return lib::input::key_button::num_4;
        case GLFW_KEY_5: return lib::input::key_button::num_5;
        case GLFW_KEY_6: return lib::input::key_button::num_6;
        case GLFW_KEY_7: return lib::input::key_button::num_7;
        case GLFW_KEY_8: return lib::input::key_button::num_8;
        case GLFW_KEY_9: return lib::input::key_button::num_9;
        case GLFW_KEY_A: return lib::input::key_button::a;
        case GLFW_KEY_B: return lib::input::key_button::b;
        case GLFW_KEY_C: return lib::input::key_button::c;
        case GLFW_KEY_D: return lib::input::key_button::d;
        case GLFW_KEY_E: return lib::input::key_button::e;
        case GLFW_KEY_F: return lib::input::key_button::f;
        case GLFW_KEY_G: return lib::input::key_button::g;
        case GLFW_KEY_H: return lib::input::key_button::h;
        case GLFW_KEY_I: return lib::input::key_button::i;
        case GLFW_KEY_J: return lib::input::key_button::j;
        case GLFW_KEY_K: return lib::input::key_button::k;
        case GLFW_KEY_L: return lib::input::key_button::l;
        case GLFW_KEY_M: return lib::input::key_button::m;
        case GLFW_KEY_N: return lib::input::key_button::n;
        case GLFW_KEY_O: return lib::input::key_button::o;
        case GLFW_KEY_P: return lib::input::key_button::p;
        case GLFW_KEY_Q: return lib::input::key_button::q;
        case GLFW_KEY_R: return lib::input::key_button::r;
        case GLFW_KEY_S: return lib::input::key_button::s;
        case GLFW_KEY_T: return lib::input::key_button::t;
        case GLFW_KEY_U: return lib::input::key_button::u;
        case GLFW_KEY_V: return lib::input::key_button::v;
        case GLFW_KEY_W: return lib::input::key_button::w;
        case GLFW_KEY_X: return lib::input::key_button::x;
        case GLFW_KEY_Y: return lib::input::key_button::y;
        case GLFW_KEY_Z: return lib::input::key_button::z;
        case GLFW_KEY_F1: return lib::input::key_button::f1;
        case GLFW_KEY_F2: return lib::input::key_button::f2;
        case GLFW_KEY_F3: return lib::input::key_button::f3;
        case GLFW_KEY_F4: return lib::input::key_button::f4;
        case GLFW_KEY_F5: return lib::input::key_button::f5;
        case GLFW_KEY_F6: return lib::input::key_button::f6;
        case GLFW_KEY_F7: return lib::input::key_button::f7;
        case GLFW_KEY_F8: return lib::input::key_button::f8;
        case GLFW_KEY_F9: return lib::input::key_button::f9;
        case GLFW_KEY_F10: return lib::input::key_button::f10;
        case GLFW_KEY_F11: return lib::input::key_button::f11;
        case GLFW_KEY_F12: return lib::input::key_button::f12;
		default: return lib::input::key_button::none;
    }
}

lib::input::key_button helpers::glfw_to_mouse_key(int button)
{
	switch (button)
    {
        case GLFW_MOUSE_BUTTON_LEFT: return lib::input::key_button::mouseleft;
		case GLFW_MOUSE_BUTTON_RIGHT: return lib::input::key_button::mouseright;
		case GLFW_MOUSE_BUTTON_MIDDLE: return lib::input::key_button::mousemiddle;
		default: return lib::input::key_button::none;
    }
}
#endif

#ifdef DEF_LIB_RENDERING_vulkan
#include <vulkan/vulkan.hpp>

#if WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

#if __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#endif

// assuming 1.f is highest priority, cant find shit anywhere :(
constexpr float queue_priority = 1.f;

constexpr uint32_t queue_index = 0;
constexpr uint32_t vulkan_api_version = VK_MAKE_API_VERSION(0, 1, 2, 0);

const std::unordered_set<std::string> vulkan_instace_extensions =
{
	"VK_KHR_surface",

#ifndef NDEBUG
	"VK_EXT_debug_utils",
#endif

#if WIN32
	"VK_KHR_win32_surface",
#elif __APPLE__
	"VK_MVK_macos_surface",
	"VK_EXT_metal_surface",
	// apply needs this since vulkan 1.3.216
	"VK_KHR_portability_enumeration",
#endif
};

const std::unordered_set<std::string> vulkan_device_extensions =
{
	"VK_KHR_swapchain",
	
#if __APPLE__
	"VK_KHR_portability_subset"
#endif
};

const std::unordered_set<std::string> validation_layers =
{
#ifndef NDEBUG
	"VK_LAYER_KHRONOS_validation"
#endif
};

const auto get_supported_extensions = [](const std::unordered_set<std::string>& extensions_set) {
	// has to std::string otherwise we pass a ptr to a string that is
	// nuke after this function leaves scope, ffs
	std::vector<std::string> extensions = {};

	uint32_t extension_count = 0;
	vk::enumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

	std::vector<vk::ExtensionProperties> extension_properties(extension_count);
	vk::enumerateInstanceExtensionProperties(
		nullptr,
		&extension_count,
		extension_properties.data());

	std::ranges::for_each(
		extension_properties.begin(),
		extension_properties.end(),
		[&](const auto& properties) {
			if (const auto extensionName = std::string(properties.extensionName);
				extensions_set.contains(extensionName))
			{
				lib_log_d("render_api: added vulkan instance extension: {}", extensionName);
				extensions.emplace_back(extensionName);
			}
		});

	return extensions;
};

const auto get_supported_device_extensions = [](
	const vk::PhysicalDevice& device,
	const std::unordered_set<std::string>& extensions_set) {
	// has to std::string otherwise we pass a ptr to a string that is
	// nuke after this function leaves scope, ffs
	std::vector<std::string> extensions = {};

	uint32_t extension_count = 0;
	device.enumerateDeviceExtensionProperties(nullptr, &extension_count, nullptr);

	std::vector<vk::ExtensionProperties> extension_properties(extension_count);
	device.enumerateDeviceExtensionProperties(
		nullptr,
		&extension_count,
		extension_properties.data());

	std::ranges::for_each(
		extension_properties.begin(),
		extension_properties.end(),
		[&](const auto& properties) {
			if (const auto extensionName = std::string(properties.extensionName);
				extensions_set.contains(extensionName))
			{
				lib_log_d("render_api: added vulkan device extension: {}", extensionName);
				extensions.emplace_back(extensionName);
			}
		});

	return extensions;
};

const auto get_supported_layers = [](const std::unordered_set<std::string>& layers_set) {
	// has to std::string otherwise we pass a ptr to a string that is
	// nuke after this function leaves scope, ffs
	std::vector<std::string> layers = {};

	uint32_t layer_count = 0;
	vk::enumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<vk::LayerProperties> layer_properties(layer_count);
	vk::enumerateInstanceLayerProperties(&layer_count, layer_properties.data());

	std::ranges::for_each(
		layer_properties.begin(),
		layer_properties.end(),
		[&](const auto& properties) {
			if (const auto layerName = std::string(properties.layerName);
				layers_set.contains(layerName))
			{
				lib_log_d("render_api: added vulkan layer: {}", layerName);
				layers.emplace_back(layerName);
			}
		});

	return layers;
};

bool helpers::create_vulkan_instance(
	GLFWwindow* window,
	lib::rendering::render_api_data_t& api_data)
{
	api_data.vulkan_api_version = vulkan_api_version;

	// api_context must be valid for vulkan
	const auto supported_instance_extensions = get_supported_extensions(vulkan_instace_extensions);
	const auto supported_layers = get_supported_layers(validation_layers);

	// convert back into const char*, should be scoped for this function
	std::vector<const char*> instance_extensions_list = {};
	std::vector<const char*> layers_list = {};

	std::ranges::for_each(supported_instance_extensions.begin(), supported_instance_extensions.end(),
		[&](const auto& entry) {
		instance_extensions_list.push_back(entry.c_str());
	});

	std::ranges::for_each(supported_layers.begin(), supported_layers.end(),
		[&](const auto& entry) {
		layers_list.push_back(entry.c_str());
	});

	vk::ApplicationInfo app_info = {};
	{
		app_info.sType = vk::StructureType::eApplicationInfo;
		app_info.pApplicationName = nullptr;
		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.pEngineName = nullptr;
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.apiVersion = vulkan_api_version;
	}

	vk::InstanceCreateInfo instance_create_info = {};
	{
		instance_create_info.sType = vk::StructureType::eInstanceCreateInfo;
		instance_create_info.pApplicationInfo = &app_info;

		instance_create_info.enabledExtensionCount = instance_extensions_list.size();
		instance_create_info.ppEnabledExtensionNames = instance_extensions_list.data();

		instance_create_info.enabledLayerCount = layers_list.size();
		instance_create_info.ppEnabledLayerNames = layers_list.data();

#if __APPLE__
		// apply needs this since vulkan 1.3.216
		instance_create_info.flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif
	}

	if (const auto result = vk::createInstance(
		&instance_create_info, nullptr, &api_data.instance); result != vk::Result::eSuccess)
	{
		lib_log_e("window_creation: failed to create vulkan instance {}", static_cast<int>(result));
		return false;
	}

	/**										Create surface											 */

	VkSurfaceKHR tmp_surface = nullptr;
	if (const auto result = glfwCreateWindowSurface(api_data.instance, window, nullptr, &tmp_surface);
		result != VK_SUCCESS)
	{
		lib_log_e("render_api: failed to create surface {}", static_cast<int>(result));
		return false;
	}

	// make from VkSurfaceKHR
	api_data.surface = vk::SurfaceKHR(tmp_surface);

	/**										Create device											 */

	uint32_t device_count = 0;
	api_data.instance.enumeratePhysicalDevices(&device_count, nullptr);

	std::vector<vk::PhysicalDevice> device_list(device_count);
	api_data.instance.enumeratePhysicalDevices(&device_count, device_list.data());

	if (device_list.empty())
	{
		lib_log_e("window_creation: no supported devices found");
		return false;
	}

	for (const auto& device: device_list)
	{
		vk::PhysicalDeviceProperties properties = {};
		device.getProperties(&properties);

		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		{
			api_data.physical_device = device;
			break;
		}
	}

	if (!api_data.physical_device)
	{
		lib_log_w("window_creation: no descrete GPU found, using first returned GPU");
		api_data.physical_device = device_list.at(0);
	}

	// find graphics queue family
	uint32_t queue_count = 0;
	api_data.physical_device.getQueueFamilyProperties(&queue_count, nullptr);

	std::vector<vk::QueueFamilyProperties> queue_families(queue_count);
	api_data.physical_device.getQueueFamilyProperties(&queue_count, queue_families.data());

	for (uint32_t i = 0; i < queue_count; i++)
	{
		vk::Bool32 present_support = false;
		api_data.physical_device.getSurfaceSupportKHR(i, api_data.surface, &present_support);

		if ((queue_families.at(i).queueFlags & vk::QueueFlagBits::eGraphics) && present_support)
		{
			api_data.present_family_index = i;
			break;
		}
	}

	// find device extenions
	const auto supported_device_extensions = get_supported_device_extensions(
		api_data.physical_device,
		vulkan_device_extensions);

	std::vector<const char*> devide_extensions_list = {};
	std::ranges::for_each(supported_device_extensions.begin(), supported_device_extensions.end(),
		[&](const auto& entry) {
		devide_extensions_list.push_back(entry.c_str());
	});

	// setup queues we need
	std::array<vk::DeviceQueueCreateInfo, 1> device_queue_create_infos = {};
	{
		{
			auto& graphic_present_queue_create_info = device_queue_create_infos[0];

			graphic_present_queue_create_info.sType = vk::StructureType::eDeviceQueueCreateInfo;
			graphic_present_queue_create_info.queueFamilyIndex = api_data.present_family_index;
			graphic_present_queue_create_info.queueCount = 1;
			graphic_present_queue_create_info.pQueuePriorities = &queue_priority;
		}
	}

	vk::PhysicalDeviceFeatures device_features = {};
	{
		// we dont use any device features right now so leave empty
	}

	vk::DeviceCreateInfo device_create_info = {};
	{
		device_create_info.sType = vk::StructureType::eDeviceCreateInfo;

		device_create_info.pQueueCreateInfos = device_queue_create_infos.data();
		device_create_info.queueCreateInfoCount = device_queue_create_infos.size();
		device_create_info.pEnabledFeatures = &device_features;

		device_create_info.enabledExtensionCount = devide_extensions_list.size();
		device_create_info.ppEnabledExtensionNames = devide_extensions_list.data();

		device_create_info.enabledLayerCount = layers_list.size();
		device_create_info.ppEnabledLayerNames = layers_list.data();
	}

	if (const auto result = api_data.physical_device.createDevice(
		&device_create_info, nullptr, &api_data.device);
		result != vk::Result::eSuccess)
	{
		lib_log_e("render_api: failed to create logical device {}", static_cast<int>(result));
		return false;
	}

	api_data.device.getQueue(api_data.present_family_index, queue_index, &api_data.queue);
	return true;
}

bool helpers::destroy_vulkan_instance(lib::rendering::render_api_data_t& api_data)
{
	if (api_data.device && api_data.surface && api_data.queue)
	{
		api_data.device.destroy();

		api_data.instance.destroySurfaceKHR(api_data.surface);
		api_data.instance.destroy();
	}

	return true;
}
#endif