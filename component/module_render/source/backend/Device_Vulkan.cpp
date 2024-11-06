#include <module_render/backend/Device_Vulkan.hpp>

#ifdef CAMEL_NVRHI_VULKAN

#include <module_logger/ScopedLog.hpp>

#include <ranges>
#include <sstream>

using namespace lib::render;

namespace {
[[nodiscard]] vulkan_extension_set_t get_default_enabled_extensions() {
    return {
        {
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        },
        { },
        {
            VK_KHR_MAINTENANCE1_EXTENSION_NAME,
        }
    };
}

[[nodiscard]] vulkan_extension_set_t get_default_optional_extensions() {
    return {
        {
            VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME,
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        },
        { },
        {
            VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
            VK_NV_MESH_SHADER_EXTENSION_NAME,
            VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
            VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
            VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME
        }
    };
}

template<typename T>
    requires std::is_same_v<T, vk::ExtensionProperties>
        || std::is_same_v<T, vk::PhysicalDeviceProperties>
        || std::is_same_v<T, vk::LayerProperties>

[[nodiscard]] std::expected<std::string, std::string> check_extensions(
    const std::vector<T>& properties,
    const std::unordered_set<std::string>& optional,
    std::unordered_set<std::string>& enabled
){
    // If we have anything left in this set by end of extension parsing then we have an error!
    std::unordered_set<std::string> missing_extensions = enabled;

    for (const auto& property: properties) {

        std::string name;

        if constexpr (std::is_same_v<T, vk::LayerProperties>) {
            name = std::string(property.layerName);
        }
        else {
            name = std::string(property.extensionName);
        }

        // Add optional extension to enabled list if we have it in properties
        if (optional.contains(name)) {
            enabled.insert(name);
        }

        // Remove from missing set
        missing_extensions.erase(name);
    }

    std::stringstream ss;

    if (!missing_extensions.empty()) {

        ss << "The following required extension(s) are not supported:";

        for (const auto& extension: missing_extensions) {
            ss << "\n - " << extension;
        }

        return std::unexpected(ss.str());
    }

    ss << "Enabled extensions:";

    for (const auto& extension: enabled) {
        ss << "\n - " << extension;
    }

    return ss.str();
}

vk::DynamicLoader DYNAMIC_LOADER = {};
}

Device_Vulkan::Device_Vulkan(const std::shared_ptr<logger::Logger>& logger, const device_settings_t& settings)
    : _settings(settings)
    , _logger(logger)
    , _extension_enabled(get_default_enabled_extensions())
    , _extension_optional(get_default_optional_extensions())
    , _nv_callback(std::make_unique<NvrhiMessageCallback>(logger)) {

    logger::ScopeLog log(_logger, "Device_Vulkan::Device_Vulkan");

    // Update back buffer to starting size
    _back_buffer_size = settings.starting_size;

    const auto vkGetInstanceProcAddr = DYNAMIC_LOADER
        .getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    // Add additional extensions depending on settings
    if (_settings.debug || _settings.validation) {
        _extension_enabled.instance.insert(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        _extension_enabled.layer.insert("VK_LAYER_KHRONOS_validation");
    }

    // Add more extensions based of settings
    _extension_enabled.merge(settings.vulkan.extra_extension_enabled);
    _extension_optional.merge(settings.vulkan.extra_extension_optional);

    //
    // Add optional extensions + validate all required extensions can be used
    // NOTE: we throw here because we are in constructor!!!
    //

    const auto ext_instance_res = check_extensions(
        vk::enumerateInstanceExtensionProperties(),
        _extension_optional.instance,
        _extension_enabled.instance
    );

    if (!ext_instance_res.has_value()) [[unlikely]] {
        log.e("{}", ext_instance_res.error());
        throw std::runtime_error(ext_instance_res.error());
    }

    log.d("{}", ext_instance_res.value());

    const auto ext_layer_res = check_extensions(
        vk::enumerateInstanceLayerProperties(),
        _extension_optional.layer,
        _extension_enabled.layer
    );

    if (!ext_layer_res.has_value()) [[unlikely]] {
        log.e("{}", ext_layer_res.error());
        throw std::runtime_error(ext_layer_res.error());
    }

    log.d("{}", ext_layer_res.value());

    //
    // Create vulkan instance
    //

    if (const auto res = create_vk_instance(); !res.has_value()) [[unlikely]] {
        throw std::runtime_error(res.error());
    }

    if (_settings.debug) {
        const auto info = vk::DebugReportCallbackCreateInfoEXT()
                .setFlags(vk::DebugReportFlagBitsEXT::eError |
                          vk::DebugReportFlagBitsEXT::eWarning |
                          vk::DebugReportFlagBitsEXT::ePerformanceWarning)
                .setPfnCallback(vk_debug_callback)
                .setPUserData(this);

        // How ironic? Debug callback causes an error!
        if (const auto res = _vk_instance.createDebugReportCallbackEXT(&info, nullptr, &_debug_report_callback);
            res != vk::Result::eSuccess) [[unlikely]] {
            log.e("Could not install debug callback, error code = {}", nvrhi::vulkan::resultToString(static_cast<VkResult>(res)));
            throw std::runtime_error("Could not install debug callback");
        }
    }

    //
    // Create vulkan device
    //

    if (const auto res = create_vk_device(); !res.has_value()) [[unlikely]] {
        throw std::runtime_error(res.error());
    }

    //
    // Create vulkan swap chain
    //

    if (const auto res = create_vk_swapchain(); !res.has_value()) [[unlikely]] {
        throw std::runtime_error(res.error());
    }

    // Create memory synchronisation objects
    _nv_command_list_barrier = _nv_device->createCommandList();
    _present_semaphores.reserve(MAX_FRAMES_IN_FLIGHT + 1);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT + 1; ++i) {
        vk::SemaphoreCreateInfo semaphore_create_info = {};
        _present_semaphores.emplace_back(_vk_device.createSemaphore(semaphore_create_info));
    }
}

Device_Vulkan::~Device_Vulkan() {

    // This function will wait until vk device is idle
    destroy_swapchain();

    // Nuke semaphores
    for (auto& semaphore: _present_semaphores) {
        if (!semaphore) {
            continue;
        }

        _vk_device.destroySemaphore(semaphore);
        semaphore = vk::Semaphore();
    }

    // Nuke nvrhi objects
    _nv_command_list_barrier        = nullptr;
    _nv_device_validation   = nullptr;
    _nv_device              = nullptr;

    // Destroy Vulkan objects
    if (_vk_device) {
        _vk_device.destroy();
        _vk_device = nullptr;
    }

    if (_vk_surface) {
        _vk_instance.destroySurfaceKHR(_vk_surface);
        _vk_surface = nullptr;
    }

    if (_debug_report_callback) {
        _vk_instance.destroyDebugReportCallbackEXT(_debug_report_callback);
    }

    if (_vk_instance) {
        _vk_instance.destroy();
        _vk_instance = nullptr;
    }
}

std::expected<void, std::string> Device_Vulkan::create_vk_instance() {

    logger::ScopeLog log(_logger, "Device_Vulkan::create_vk_instance");

    vk::ApplicationInfo application_info = {};

    // Query the Vulkan API version supported on the system.
    vk::Result res = vk::enumerateInstanceVersion(&application_info.apiVersion);

    if (res != vk::Result::eSuccess) [[unlikely]] {
        log.e(
            "Call to vkEnumerateInstanceVersion failed, error code = {}",
            nvrhi::vulkan::resultToString(static_cast<VkResult>(res))
        );
        return std::unexpected("failed to enumerate Vulkan version");
    }

    // Check if the Vulkan API version is sufficient.
    constexpr uint32_t MINIMUM_VULKAN_VERSION = VK_MAKE_API_VERSION(0, 1, 3, 0);

    if (application_info.apiVersion < MINIMUM_VULKAN_VERSION) [[unlikely]] {
        log.e(
            "The Vulkan API version supported on the system ({}.{}.{}) is too low, at least {}.{}.{} is required.",
            VK_API_VERSION_MAJOR(application_info.apiVersion),  VK_API_VERSION_MINOR(application_info.apiVersion),  VK_API_VERSION_PATCH(application_info.apiVersion),
            VK_API_VERSION_MAJOR(MINIMUM_VULKAN_VERSION),       VK_API_VERSION_MINOR(MINIMUM_VULKAN_VERSION),       VK_API_VERSION_PATCH(MINIMUM_VULKAN_VERSION)
        );
        return std::unexpected("The Vulkan API version supported on the system is too low");
    }

    // Spec says: A non-zero variant indicates the API is a variant of the Vulkan API and applications will
    // typically need to be modified to run against it.
    if (VK_API_VERSION_VARIANT(application_info.apiVersion) != 0) [[unlikely]] {
        log.e(
            "The Vulkan API supported on the system uses an unexpected variant {}",
            VK_API_VERSION_VARIANT(application_info.apiVersion)
        );
        return std::unexpected("The Vulkan API supported on the system uses an unexpected variant");
    }

    const auto enabled_extension_layer_vector = _extension_enabled.layer
        | std::views::transform([](const std::string& x) { return x.c_str(); })
        | std::ranges::to<std::vector>();

    const auto enabled_extension_instance_vector = _extension_enabled.instance
        | std::views::transform([](const std::string& x) { return x.c_str(); })
        | std::ranges::to<std::vector>();

    // Create the vulkan instance
    const auto info = vk::InstanceCreateInfo()
        .setEnabledLayerCount(static_cast<uint32_t>(enabled_extension_layer_vector.size()))
        .setPpEnabledLayerNames(enabled_extension_layer_vector.data())
        .setEnabledExtensionCount(static_cast<uint32_t>(enabled_extension_instance_vector.size()))
        .setPpEnabledExtensionNames(enabled_extension_instance_vector.data())
        .setPApplicationInfo(&application_info);

    res = vk::createInstance(&info, nullptr, &_vk_instance);

    if (res != vk::Result::eSuccess) [[unlikely]] {
        log.e(
            "Failed to create a Vulkan instance, error code = {}",
            nvrhi::vulkan::resultToString(static_cast<VkResult>(res))
        );
        return std::unexpected("Could not create vulkan instance");
    }

    VULKAN_HPP_DEFAULT_DISPATCHER.init(_vk_instance);
    log.i("Successfully created Vulkan instance");

    return {};
}

std::expected<void, std::string> Device_Vulkan::create_vk_device() {

    logger::ScopeLog log(_logger, "Device_Vulkan::create_vk_device");

    {
        vk::Result vk_res = vk::Result::eSuccess;

        if (!_settings.vulkan.create_window_surface) [[unlikely]] {
            return std::unexpected("create_window_surface is nullptr");
        }

        // Use callback from settings to create window surface
        vk_create_surface_callback_t cb_params = {
            &_vk_instance,
            &_vk_surface,
        };

        vk_res = static_cast<vk::Result>(_settings.vulkan.create_window_surface(&cb_params));

        if (vk_res != vk::Result::eSuccess) [[unlikely]] {
            log.e("Could not create window surface, error code = {}", nvrhi::vulkan::resultToString(static_cast<VkResult>(vk_res)));
            return std::unexpected("Could not create window surface");
        }
    }

    {
        const auto physical_device = pick_physical_device();

        if (!physical_device.has_value()) [[unlikely]] {
            return std::unexpected(physical_device.error());
        }

        _vk_physical_device = physical_device.value();
    }

    {
        const auto queue_family = pick_queue_families(_vk_physical_device);

        if (!queue_family.has_value()) [[unlikely]] {
            return std::unexpected(queue_family.error());
        }

        _queue_family = queue_family.value();
    }

    if (const auto res = create_logical_device(); !res.has_value()) [[unlikely]] {
        return res;
    }

    //
    // Create queues
    //

    _vk_device.getQueue(_queue_family.graphics_queue, 0, &_graphics_queue);
    _vk_device.getQueue(_queue_family.present_queue, 0, &_present_queue);

    if (_settings.compute_queue) {
        _vk_device.getQueue(_queue_family.compute_queue, 0, &_compute_queue);
    }

    if (_settings.copy_queue) {
        _vk_device.getQueue(_queue_family.transfer_queue, 0, &_transfer_queue);
    }

    log.d("Created queues");

    auto enabled_extension_instance_vector = _extension_enabled.instance
        | std::views::transform([](const std::string& x) { return x.c_str(); })
        | std::ranges::to<std::vector>();

    auto enabled_extension_device_vector = _extension_enabled.device
        | std::views::transform([](const std::string& x) { return x.c_str(); })
        | std::ranges::to<std::vector>();

    nvrhi::vulkan::DeviceDesc deviceDesc = {};
    {
        deviceDesc.errorCB          = _nv_callback.get();
        deviceDesc.instance         = _vk_instance;
        deviceDesc.physicalDevice   = _vk_physical_device;
        deviceDesc.device           = _vk_device;

        deviceDesc.graphicsQueue        = _graphics_queue;
        deviceDesc.graphicsQueueIndex   = _queue_family.graphics_queue;

        if (_settings.compute_queue) {
            deviceDesc.computeQueue         = _compute_queue;
            deviceDesc.computeQueueIndex    = _queue_family.compute_queue;
        }

        if (_settings.copy_queue) {
            deviceDesc.transferQueue        = _transfer_queue;
            deviceDesc.transferQueueIndex   = _queue_family.transfer_queue;
        }

        deviceDesc.instanceExtensions       = enabled_extension_instance_vector.data();
        deviceDesc.numInstanceExtensions    = enabled_extension_instance_vector.size();

        deviceDesc.deviceExtensions         = enabled_extension_device_vector.data();
        deviceDesc.numDeviceExtensions      = enabled_extension_device_vector.size();

        deviceDesc.bufferDeviceAddressSupported = _physical_device_features.contains(PhysicalDeviceFeatures::BufferDeviceAddress);
    }

    _nv_device = nvrhi::vulkan::createDevice(deviceDesc);

    // Wrap device in validation layer if using validation layer
    // Note: device() will return validation layer if it exists

    if (_settings.validation) {
        _nv_device_validation = nvrhi::validation::createValidationLayer(_nv_device);
    }

    return {};
}

std::expected<void, std::string> Device_Vulkan::create_vk_swapchain() {

    logger::ScopeLog log(_logger, "Device_Vulkan::create_vk_swapchain");

    // Destroy current swap chain if it exists.
    destroy_swapchain();

    _vk_swapchain_formats = {
        static_cast<vk::Format>(nvrhi::vulkan::convertFormat(SWAP_CHAIN_FORMAT)),
        vk::ColorSpaceKHR::eSrgbNonlinear
    };

    // Adjust swap chain to match current device capabilities
    const vk::SurfaceCapabilitiesKHR surface_capabilities   = _vk_physical_device.getSurfaceCapabilitiesKHR(_vk_surface);
    auto extent                                             = vk::Extent2D(_back_buffer_size.x, _back_buffer_size.y);

    if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        extent = surface_capabilities.currentExtent;
    }
    else {
        extent.width = std::clamp(
            extent.width,
            surface_capabilities.minImageExtent.width,
            surface_capabilities.maxImageExtent.width
        );

        extent.height = std::clamp(
            extent.height,
            surface_capabilities.minImageExtent.height,
            surface_capabilities.maxImageExtent.height
        );
    }

    std::vector image_formats = {
        _vk_swapchain_formats.format
    };

    switch(_vk_swapchain_formats.format) {
        case vk::Format::eR8G8B8A8Unorm:
            image_formats.push_back(vk::Format::eR8G8B8A8Srgb);
        break;
        case vk::Format::eR8G8B8A8Srgb:
            image_formats.push_back(vk::Format::eR8G8B8A8Unorm);
        break;
        case vk::Format::eB8G8R8A8Unorm:
            image_formats.push_back(vk::Format::eB8G8R8A8Srgb);
        break;
        case vk::Format::eB8G8R8A8Srgb:
            image_formats.push_back(vk::Format::eB8G8R8A8Unorm);
        break;
        default:
            break;
    }

    const std::unordered_set unique_queues = {
        static_cast<uint32_t>(_queue_family.graphics_queue),
        static_cast<uint32_t>(_queue_family.present_queue)
    };

    const auto queues = unique_queues
        | std::ranges::to<std::vector>();

    const bool enable_swap_chain_sharing = queues.size() > 1;

    auto desc = vk::SwapchainCreateInfoKHR()
        .setSurface(_vk_surface)
        .setMinImageCount(SWAP_CHAIN_BUFFER_COUNT)
        .setImageFormat(_vk_swapchain_formats.format)
        .setImageColorSpace(_vk_swapchain_formats.colorSpace)
        .setImageExtent(extent)
        .setImageArrayLayers(1)
        .setImageUsage(
            vk::ImageUsageFlagBits::eColorAttachment
                | vk::ImageUsageFlagBits::eTransferDst
                | vk::ImageUsageFlagBits::eSampled
        )
        .setImageSharingMode(enable_swap_chain_sharing ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive)
        .setFlags(
            _physical_device_features.contains(PhysicalDeviceFeatures::SwapChainMutableFormat)
                ? vk::SwapchainCreateFlagBitsKHR::eMutableFormat
                : static_cast<vk::SwapchainCreateFlagBitsKHR>(0)
        )
        .setQueueFamilyIndexCount(enable_swap_chain_sharing ? static_cast<uint32_t>(queues.size()) : 0)
        .setPQueueFamilyIndices(enable_swap_chain_sharing ? queues.data() : nullptr)
        .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(_settings.vsync ? vk::PresentModeKHR::eFifo : vk::PresentModeKHR::eImmediate)
        .setClipped(true)
        .setOldSwapchain(nullptr);

    const auto image_format_list_create_info = vk::ImageFormatListCreateInfo()
        .setViewFormats(image_formats);

    if (_physical_device_features.contains(PhysicalDeviceFeatures::SwapChainMutableFormat)) {
        desc.pNext = &image_format_list_create_info;
    }

    if (const auto res = _vk_device.createSwapchainKHR(&desc, nullptr, &_vk_swapchain); res != vk::Result::eSuccess) [[unlikely]] {
        log.e("Could not create swap chain, error code = {}", nvrhi::vulkan::resultToString(static_cast<VkResult>(res)));
        return std::unexpected("Could not create window surface");
    }

    // retrieve swap chain images
    for(auto image : _vk_device.getSwapchainImagesKHR(_vk_swapchain)) {

        nvrhi::TextureDesc textureDesc = {};
        {
            textureDesc.width = _back_buffer_size.x ;
            textureDesc.height = _back_buffer_size.y;
            textureDesc.format = SWAP_CHAIN_FORMAT;
            textureDesc.debugName = "Swap chain image";
            textureDesc.initialState = nvrhi::ResourceStates::Present;
            textureDesc.keepInitialState = true;
            textureDesc.isRenderTarget = true;
        }

        _vk_swapchain_images.emplace_back(
            image,
            _nv_device->createHandleForNativeTexture(nvrhi::ObjectTypes::VK_Image, nvrhi::Object(image), textureDesc)
        );
    }

    // Reset index, we will update as frames are in flight!
    _vk_swapchain_index = 0;

    return {};
}

std::expected<vk::PhysicalDevice, std::string> Device_Vulkan::pick_physical_device() const {

    logger::ScopeLog log(_logger, "Device_Vulkan::pick_physical_device");

    // Enumerate physical devices and pick the best match for us
    // Todo: support picking devices explicitly via settings
    // Note: we pick a random initial extent size!

    const auto expected_swap_chain_format   = static_cast<vk::Format>(nvrhi::vulkan::convertFormat(SWAP_CHAIN_FORMAT));
    const auto expected_extent              = vk::Extent2D(_back_buffer_size.x, _back_buffer_size.y);

    const auto devices = _vk_instance.enumeratePhysicalDevices();

    std::vector<vk::PhysicalDevice> device_discrete;
    std::vector<vk::PhysicalDevice> device_other;

    std::unordered_set<std::string> device_extensions;

    for (const auto& device : devices) {

        // Fresh copy !
        device_extensions = _extension_enabled.device;

        const vk::PhysicalDeviceProperties properties   = device.getProperties();
        const vk::PhysicalDeviceFeatures features       = device.getFeatures();

        if (!check_extensions(device.enumerateDeviceExtensionProperties(), {}, device_extensions)) {
            continue;
        }

        if (!features.samplerAnisotropy || !features.textureCompressionBC) {
            continue;
        }

        if (const auto queue_family = pick_queue_families(device);
            !queue_family.has_value() ||
            !device.getSurfaceSupportKHR(queue_family->present_queue, _vk_surface) ||
            !device.getSurfaceSupportKHR(queue_family->graphics_queue, _vk_surface))
        {
            continue;
        }

        // Ensure device supports expected swap chain creation parameters
        const vk::SurfaceCapabilitiesKHR surface_capabilities   = device.getSurfaceCapabilitiesKHR(_vk_surface);
        const std::vector<vk::SurfaceFormatKHR> surface_formats = device.getSurfaceFormatsKHR(_vk_surface);

        if (surface_capabilities.minImageCount > SWAP_CHAIN_BUFFER_COUNT ||
           (surface_capabilities.maxImageCount < SWAP_CHAIN_BUFFER_COUNT && surface_capabilities.maxImageCount > 0)
        ) {
           continue;
        }

        if (surface_capabilities.minImageExtent.width > expected_extent.width ||
            surface_capabilities.minImageExtent.height > expected_extent.height ||
            surface_capabilities.maxImageExtent.width < expected_extent.width ||
            surface_capabilities.maxImageExtent.height < expected_extent.height
        ) {
            continue;
        }

        bool expected_format_found = false;

        for (const vk::SurfaceFormatKHR& surfaceFmt : surface_formats) {
            if (surfaceFmt.format == expected_swap_chain_format) {
                expected_format_found = true;
                break;
            }
        }

        if (!expected_format_found) {
            continue;
        }

        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            device_discrete.push_back(device);
        }
        else {
            device_other.push_back(device);
        }
    }

    if (device_discrete.empty() && device_other.empty()) [[unlikely]]  {
        log.e("Could not find any valid devices");
        return std::unexpected("Could not find any valid devices");
    }

    vk::PhysicalDevice device;

    if (device_discrete.empty()) {
        // No discrete - use integrated
        device = device_other.front();
    }
    else {
        // Use discrete
        device = device_discrete.front();
    }

    log.d("Using device - {}", std::string(device.getProperties().deviceName));

    return device;
}

std::expected<Device_Vulkan::queue_family_properties_t, std::string> Device_Vulkan::pick_queue_families(
    const vk::PhysicalDevice& device
) const {

    queue_family_properties_t queue_family;
    const std::vector<vk::QueueFamilyProperties> properties = device.getQueueFamilyProperties();

    for (size_t i = 0; i < properties.size(); ++i) {

        const vk::QueueFamilyProperties& property = properties.at(i);

        if (queue_family.graphics_queue == -1) {
            if (property.queueCount > 0 &&
               (property.queueFlags & vk::QueueFlagBits::eGraphics)
            ) {
                queue_family.graphics_queue = static_cast<int>(i);
            }
        }

        if (queue_family.compute_queue == -1) {
            if (property.queueCount > 0 &&
               (property.queueFlags & vk::QueueFlagBits::eCompute) &&
              !(property.queueFlags & vk::QueueFlagBits::eGraphics)
            ) {
                queue_family.compute_queue = static_cast<int>(i);
            }
        }

        if (queue_family.transfer_queue == -1) {
            if (property.queueCount > 0 &&
               (property.queueFlags & vk::QueueFlagBits::eTransfer) &&
              !(property.queueFlags & vk::QueueFlagBits::eCompute) &&
              !(property.queueFlags & vk::QueueFlagBits::eGraphics)
            ) {
                queue_family.transfer_queue = static_cast<int>(i);
            }
        }

        if (queue_family.present_queue == -1) {

            // Const cast bad!! but this whole callback system is bad!! so fuck it
            vk_get_physical_device_support_t cb_params = {
                const_cast<vk::Instance*>(&_vk_instance),
                const_cast<vk::PhysicalDevice*>(&device),
                static_cast<uint32_t>(i)
            };

            if (property.queueCount > 0 && _settings.vulkan.get_physical_device_presentation_support(&cb_params)) {
                queue_family.present_queue = static_cast<int>(i);
            }
        }
    }

    if (queue_family.graphics_queue ==  -1 ||
        queue_family.present_queue ==   -1 ||
       (queue_family.compute_queue ==   -1 && _settings.compute_queue) ||
       (queue_family.transfer_queue ==  -1 && _settings.copy_queue)
    ) [[unlikely]] {
        return std::unexpected("Could not find any valid queue families");
    }

    return queue_family;
}

std::expected<void, std::string> Device_Vulkan::create_logical_device() {

    logger::ScopeLog log(_logger, "Device_Vulkan::create_logical_device");

    // Check extensions!
    const auto check_ext_res = check_extensions(
        _vk_physical_device.enumerateDeviceExtensionProperties(),
        _extension_optional.device,
        _extension_enabled.device
    );

    if (!check_ext_res.has_value()) [[unlikely]] {
        return std::unexpected("Could not find any valid extensions");
    }

    _extension_enabled.device.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    log.d("{}", check_ext_res.value());

    // Get supported features
    for (const auto& ext : _extension_enabled.device) {
        if (ext == VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME) {
            _physical_device_features.emplace(PhysicalDeviceFeatures::AccelStruct);
        }
        else if (ext == VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) {
            _physical_device_features.emplace(PhysicalDeviceFeatures::RayPipeline);
        }
        else if (ext == VK_KHR_RAY_QUERY_EXTENSION_NAME) {
            _physical_device_features.emplace(PhysicalDeviceFeatures::RayQueue);
        }
        else if (ext == VK_NV_MESH_SHADER_EXTENSION_NAME) {
            _physical_device_features.emplace(PhysicalDeviceFeatures::Meshlets);
        }
        else if (ext == VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME) {
            _physical_device_features.emplace(PhysicalDeviceFeatures::VRS);
        }
        else if (ext == VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME) {
            _physical_device_features.emplace(PhysicalDeviceFeatures::Synchronization2);
        }
        else if (ext == VK_KHR_MAINTENANCE_4_EXTENSION_NAME) {
            _physical_device_features.emplace(PhysicalDeviceFeatures::Maintenance4);
        }
        else if (ext == VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME) {
            _physical_device_features.emplace(PhysicalDeviceFeatures::SwapChainMutableFormat);
        }
    }

    //
    //  IDK what the fuck this mess is....
    //

    const vk::PhysicalDeviceProperties properties = _vk_physical_device.getProperties();

    #define APPEND_EXTENSION(condition, desc) if (condition) { (desc).pNext = pNext; pNext = &(desc); }
    void* pNext = nullptr;

    vk::PhysicalDeviceFeatures2 physical_device_features_2 = {};

    // Determine support for Buffer Device Address, the Vulkan 1.2 way
    auto buffer_device_address_features = vk::PhysicalDeviceBufferDeviceAddressFeatures();

    // Determine support for maintenance4
    auto maintenance4Features = vk::PhysicalDeviceMaintenance4Features();

    // Put the user-provided extension structure at the end of the chain
    pNext = nullptr;

    APPEND_EXTENSION(true, buffer_device_address_features);
    APPEND_EXTENSION(_physical_device_features.contains(PhysicalDeviceFeatures::Maintenance4), maintenance4Features);

    physical_device_features_2.pNext = pNext;
    _vk_physical_device.getFeatures2(&physical_device_features_2);

    std::unordered_set unique_queue_families = {
        _queue_family.graphics_queue,
        _queue_family.present_queue,
    };

    if (_settings.compute_queue) {
        unique_queue_families.insert(_queue_family.compute_queue);
    }

    if (_settings.copy_queue) {
        unique_queue_families.insert(_queue_family.transfer_queue);
    }

    float priority = 1.f;

    std::vector<vk::DeviceQueueCreateInfo> queue_description;
    queue_description.reserve(unique_queue_families.size());

    for(int queueFamily : unique_queue_families) {
        queue_description.push_back(vk::DeviceQueueCreateInfo()
            .setQueueFamilyIndex(queueFamily)
            .setQueueCount(1)
            .setPQueuePriorities(&priority));
    }

    auto acceleration_structure_features = vk::PhysicalDeviceAccelerationStructureFeaturesKHR()
        .setAccelerationStructure(true);

    // auto ray_pipeline_features = vk::PhysicalDeviceRayTracingPipelineFeaturesKHR()
    //     .setRayTracingPipeline(true)
    //     .setRayTraversalPrimitiveCulling(true);
    //
    // auto ray_queue_features = vk::PhysicalDeviceRayQueryFeaturesKHR()
    //     .setRayQuery(true);

    // APPEND_EXTENSION(ray_pipeline_supported, ray_pipeline_features)
    // APPEND_EXTENSION(ray_queue_supported, ray_queue_features)

    auto meshlet_features = vk::PhysicalDeviceMeshShaderFeaturesNV()
        .setTaskShader(true)
        .setMeshShader(true);

    auto vrs_features = vk::PhysicalDeviceFragmentShadingRateFeaturesKHR()
        .setPipelineFragmentShadingRate(true)
        .setPrimitiveFragmentShadingRate(true)
        .setAttachmentFragmentShadingRate(true);

    auto vulkan_1_3_features = vk::PhysicalDeviceVulkan13Features()
        .setSynchronization2(_physical_device_features.contains(PhysicalDeviceFeatures::Synchronization2))
        .setMaintenance4(maintenance4Features.maintenance4);

    pNext = nullptr;

    APPEND_EXTENSION(_physical_device_features.contains(PhysicalDeviceFeatures::AccelStruct),   acceleration_structure_features)
    APPEND_EXTENSION(_physical_device_features.contains(PhysicalDeviceFeatures::Meshlets),      meshlet_features)
    APPEND_EXTENSION(_physical_device_features.contains(PhysicalDeviceFeatures::VRS),           vrs_features)
    APPEND_EXTENSION(properties.apiVersion >= VK_API_VERSION_1_3,                                 vulkan_1_3_features)
    APPEND_EXTENSION(properties.apiVersion < VK_API_VERSION_1_3 && _physical_device_features.contains(PhysicalDeviceFeatures::Maintenance4), maintenance4Features);

    #undef APPEND_EXTENSION

    constexpr auto device_features = vk::PhysicalDeviceFeatures()
            .setShaderImageGatherExtended(true)
            .setSamplerAnisotropy(true)
            .setTessellationShader(true)
            .setTextureCompressionBC(true)
            .setGeometryShader(true)
            .setImageCubeArray(true)
            .setDualSrcBlend(true);

    const auto vulkan_1_2_features = vk::PhysicalDeviceVulkan12Features()
        .setDescriptorIndexing(true)
        .setRuntimeDescriptorArray(true)
        .setDescriptorBindingPartiallyBound(true)
        .setDescriptorBindingVariableDescriptorCount(true)
        .setTimelineSemaphore(true)
        .setShaderSampledImageArrayNonUniformIndexing(true)
        .setBufferDeviceAddress(buffer_device_address_features.bufferDeviceAddress)
        .setPNext(pNext);

    if (vulkan_1_2_features.bufferDeviceAddress) {
        _physical_device_features.emplace(PhysicalDeviceFeatures::BufferDeviceAddress);
    }

    const auto enabled_extension_device_vector = _extension_enabled.device
        | std::views::transform([](const std::string& x) { return x.c_str(); })
        | std::ranges::to<std::vector>();

    const auto enabled_extension_layer_vector = _extension_enabled.layer
        | std::views::transform([](const std::string& x) { return x.c_str(); })
        | std::ranges::to<std::vector>();

    const auto device_description = vk::DeviceCreateInfo()
        .setPQueueCreateInfos(queue_description.data())
        .setQueueCreateInfoCount(static_cast<uint32_t>(queue_description.size()))
        .setPEnabledFeatures(&device_features)
        .setEnabledExtensionCount(static_cast<uint32_t>(enabled_extension_device_vector.size()))
        .setPpEnabledExtensionNames(enabled_extension_device_vector.data())
        .setEnabledLayerCount(static_cast<uint32_t>(enabled_extension_layer_vector.size()))
        .setPpEnabledLayerNames(enabled_extension_layer_vector.data())
        .setPNext(&vulkan_1_2_features);

    const vk::Result vk_res = _vk_physical_device.createDevice(&device_description, nullptr, &_vk_device);

    if (vk_res != vk::Result::eSuccess) [[unlikely]] {
        log.e("Failed to create device, error code - {}", nvrhi::vulkan::resultToString(static_cast<VkResult>(vk_res)));
        return std::unexpected("Could not create device");
    }

    VULKAN_HPP_DEFAULT_DISPATCHER.init(_vk_device);
    log.d("Created device");

    return {};
}

void Device_Vulkan::destroy_swapchain() {
    if (_vk_device) {
        _vk_device.waitIdle();
    }

    if (_vk_swapchain) {
        _vk_device.destroySwapchainKHR(_vk_swapchain);
        _vk_swapchain = nullptr;
    }

    _vk_swapchain_images.clear();
}

VKAPI_ATTR VkBool32 VKAPI_CALL Device_Vulkan::vk_debug_callback(
    [[maybe_unused]] const VkDebugReportFlagsEXT flags,
    [[maybe_unused]] const VkDebugReportObjectTypeEXT objType,
    [[maybe_unused]] const uint64_t obj,
    const size_t location,
    const int32_t code,
    const char* layerPrefix,
    const char* msg,
    [[maybe_unused]] void* userData
) {
    const auto this_ptr = static_cast<Device_Vulkan*>(userData);
    this_ptr->_logger->d("vk-debug", "[Vulkan: location=0x{:x} code={}, layerPrefix='{}'] {}", location, code, layerPrefix, msg);

    return VK_FALSE;
}

//
// VIRTUAL FUNCTIONS
//

nvrhi::IDevice* Device_Vulkan::device() const {
    if (_nv_device_validation) {
        return _nv_device_validation;
    }
    return _nv_device;
}

const lib::point2Di& Device_Vulkan::back_buffer_size() const {
    return _back_buffer_size;
}

nvrhi::ITexture* Device_Vulkan::current_back_buffer() const {
    return _vk_swapchain_images.at(_vk_swapchain_index).texture;
}

nvrhi::ITexture* Device_Vulkan::back_buffer([[maybe_unused]] const uint32_t index) const {
    if (index >= _vk_swapchain_images.size()) [[unlikely]] {
        return nullptr;
    }
    return _vk_swapchain_images.at(index).texture;
}

uint32_t Device_Vulkan::current_back_buffer_index() const {
    return _vk_swapchain_index;
}

uint32_t Device_Vulkan::back_buffer_count() const {
    return _vk_swapchain_images.size();
}

void Device_Vulkan::update_screen_size(const point2Di& window_size) {

    // Call Resizing (pre resizing) callback if it exists
    if (const auto cb = _callback.find(CallbackState::Resizing); cb != _callback.end()) {
        cb->second(*this);
    }

    _back_buffer_size = window_size;

    if (_vk_device) {
        if (const auto res = create_vk_swapchain(); !res) [[unlikely]] {
            throw std::runtime_error(res.error());
        }
    }

    // Call resized callback if it exists
    if (const auto cb = _callback.find(CallbackState::Resized); cb != _callback.end()) {
        cb->second(*this);
    }
}

void Device_Vulkan::begin_frame() {

    // Max number of times to try to create frame before throwing
    constexpr size_t MAX_BEGIN_ATTEMPTS = 3;

    vk::Result res = {};
    const vk::Semaphore& semaphore = _present_semaphores[_present_semaphore_index];

    for (size_t i = 0; i < MAX_BEGIN_ATTEMPTS; ++i) {

        res = _vk_device.acquireNextImageKHR(
            _vk_swapchain,
            std::numeric_limits<uint64_t>::max(), // timeout
            semaphore,
            vk::Fence(),
            &_vk_swapchain_index
        );

        if (res == vk::Result::eErrorOutOfDateKHR) [[unlikely]] {

            logger::ScopeLog log(_logger, "Device_Vulkan::begin_frame");
            log.w("performance - swap chain out date, attempting to resize - please update screen size explicitly");

            // Update screen size if needed
            const vk::SurfaceCapabilitiesKHR surface_capabilities = _vk_physical_device.getSurfaceCapabilitiesKHR(_vk_surface);
            const point2Di surface_size = {
                static_cast<int>(surface_capabilities.currentExtent.width),
                static_cast<int>(surface_capabilities.currentExtent.height)
            };

            update_screen_size(surface_size);
        }
        else {
            break;
        }
    }

    // Throw if we couldn't update screen size successfully
    if (res != vk::Result::eSuccess) [[unlikely]] {
        throw std::runtime_error("Could not begin frame");
    }

    // Upcast allowed without RTTI here because we create and destroy device from this class!
    _nv_device->queueWaitForSemaphore(nvrhi::CommandQueue::Graphics, semaphore, 0);
}

void Device_Vulkan::present() {

    const vk::Semaphore& semaphore = _present_semaphores[_present_semaphore_index];
    _nv_device->queueSignalSemaphore(nvrhi::CommandQueue::Graphics, semaphore, 0);

    _nv_command_list_barrier->open();
    _nv_command_list_barrier->close();

    _nv_device->executeCommandList(_nv_command_list_barrier);

    const auto info = vk::PresentInfoKHR()
        .setWaitSemaphoreCount(1)
        .setPWaitSemaphores(&semaphore)
        .setSwapchainCount(1)
        .setPSwapchains(&_vk_swapchain)
        .setPImageIndices(&_vk_swapchain_index);

    if (const vk::Result res = _present_queue.presentKHR(&info); res != vk::Result::eSuccess) [[unlikely]] {
        throw std::runtime_error("Could not present");
    }

    // Increment semaphore index/frame in flight
    _present_semaphore_index = (_present_semaphore_index + 1) % _present_semaphores.size();

#ifndef CAMEL_PLATFORM_WINDOWS
    if (_settings.vsync) {
        _present_queue.waitIdle();
    }
#endif

    while (_nv_frames_in_flight.size() >= MAX_FRAMES_IN_FLIGHT) {
        auto query = std::move(_nv_frames_in_flight.front());
        _nv_frames_in_flight.pop();

        _nv_device->waitEventQuery(query);
        _nv_query_pool.push_back(query);
    }

    nvrhi::EventQueryHandle query;

    if (_nv_query_pool.empty()) {
        query = _nv_device->createEventQuery();
    }
    else {
        query = _nv_query_pool.back();
        _nv_query_pool.pop_back();
    }

    _nv_device->resetEventQuery(query);
    _nv_device->setEventQuery(query, nvrhi::CommandQueue::Graphics);

    _nv_frames_in_flight.push(query);
}

#endif
