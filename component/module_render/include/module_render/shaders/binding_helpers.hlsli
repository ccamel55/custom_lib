#ifndef BINDING_HELPERS_HLSLI
#define BINDING_HELPERS_HLSLI

#if defined(SPIRV) || defined(TARGET_VULKAN) // Support old-style and new-style platform macros

#define VK_PUSH_CONSTANT [[vk::push_constant]]
#define VK_BINDING(reg,dset) [[vk::binding(reg,dset)]]
#define VK_DESCRIPTOR_SET(dset) ,space##dset

#else

#define VK_PUSH_CONSTANT
#define VK_BINDING(reg,dset) 
#define VK_DESCRIPTOR_SET(dset)

#endif

// Helper macro to expand the register and space macros before concatenating tokens.
// Declares a register space explicitly on DX12 and Vulkan, skips it on DX11.
#ifdef TARGET_D3D11
#define REGISTER_HELPER(TY,REG,SPACE) register(TY##REG)
#else
#define REGISTER_HELPER(TY,REG,SPACE) register(TY##REG, space##SPACE)
#endif

// Macros to declare bindings for various resource types in a cross-platform way
// using register and space indices coming from other preprocessor macros.
#define REGISTER_CBUFFER(reg,space) REGISTER_HELPER(b,reg,space)
#define REGISTER_SAMPLER(reg,space) REGISTER_HELPER(s,reg,space)
#define REGISTER_SRV(reg,space)     REGISTER_HELPER(t,reg,space)
#define REGISTER_UAV(reg,space)     REGISTER_HELPER(u,reg,space)

// Macro to declare a constant buffer in a cross-platform way, compatible with the VK_PUSH_CONSTANT attribute.
#ifdef TARGET_D3D11
#define DECLARE_CBUFFER(ty,name,reg,space) cbuffer c_##name : REGISTER_CBUFFER(reg,space) { ty name; }
#else
#define DECLARE_CBUFFER(ty,name,reg,space) ConstantBuffer<ty> name : REGISTER_CBUFFER(reg,space)
#endif

// Macro to declare a push constant block on Vulkan and a regular cbuffer on other platforms.
#define DECLARE_PUSH_CONSTANTS(ty,name,reg,space) VK_PUSH_CONSTANT DECLARE_CBUFFER(ty,name,reg,space)

#endif // BINDING_HELPERS_HLSLI