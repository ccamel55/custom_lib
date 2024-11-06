#include <dep_nvrhi/nvrhi.hpp>

#ifdef CAMEL_NVRHI_VULKAN
// Define the Vulkan dynamic dispatcher - this needs to occur in exactly one cpp file in the program.
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif