# Shader main output location - programs that use these shaders can copy from this directory
set(_SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shader_spv CACHE INTERNAL "")

set (
    NVRHI_DEFAULT_VK_REGISTER_OFFSETS
        --tRegShift 0
        --sRegShift 128
        --bRegShift 256
        --uRegShift 384
)

# Compile shaders into shadermake bin
function(compile_shaders TARGET INPUT)

    # Name of compile target
    set(TARGET_NAME "compile_shaders_${TARGET}")

    # Find all shaders inside
    file(
        GLOB_RECURSE SOURCES
            "*.hlsl"
            "*.hlsli"
            "*.h"
    )

    # Config should be in root of input directory
    set(SHADER_CONFIG ${INPUT}/ShaderConfig.cfg)

    if (NOT EXISTS ${SHADER_CONFIG})
        message(FATAL_ERROR "compile_shaders: shader config ${SHADER_CONFIG} does not exist")
    endif ()

    # .bin output results
    set(OUTPUT_DXBC     ${_SHADER_OUTPUT_DIR}/dxbc)
    set(OUTPUT_DXIL     ${_SHADER_OUTPUT_DIR}/dxil)
    set(OUTPUT_SPIRV    ${_SHADER_OUTPUT_DIR}/spirv)

    # Create compile shader dependency
    if (TARGET ${TARGET_NAME})
        message(FATAL_ERROR "compile_shaders: target ${TARGET} already has shader compilation stage")
    else ()
        add_custom_target(${TARGET_NAME}
            DEPENDS ShaderMake
            SOURCES ${SOURCES}
        )
    endif()

    # Set shadermake compile commands
    if (WIN32)
        set(USE_API_ARG --useAPI)
    else()
        set(USE_API_ARG "")
    endif()

    if (CAMEL_NVRHI_DX12)
        if (NOT DXC_PATH)
            message(FATAL_ERROR "donut_compile_shaders: DXC not found --- please set DXC_PATH to the full path to the DXC binary")
        endif()

        set(
            COMPILE_COMMAND
            ShaderMake
                --config ${SHADER_CONFIG}
                --out ${OUTPUT_DXIL}
                --platform DXIL
                --binaryBlob --outputExt .bin
                -D TARGET_D3D12
                --compiler "${DXC_PATH}"
                --shaderModel 6_5
                ${use_api_arg}
        )
    endif()

    if (CAMEL_NVRHI_DX11)
        if (NOT FXC_PATH)
            message(FATAL_ERROR "donut_compile_shaders: FXC not found --- please set FXC_PATH to the full path to the FXC binary")
        endif()

        set(
            COMPILE_COMMAND
            ShaderMake
                --config ${SHADER_CONFIG}
                --out ${OUTPUT_DXBC}
                --platform DXBC
                --binaryBlob --outputExt .bin
                -D TARGET_D3D11
                --compiler "${FXC_PATH}"
                ${use_api_arg}
        )
    endif()

    if (CAMEL_NVRHI_VULKAN)
        if (NOT DXC_SPIRV_PATH)
            message(FATAL_ERROR "donut_comple_shaders: DXC for SPIR-V not found --- please set DXC_SPIRV_PATH to the full path to the DXC binary")
        endif()

        set(
            COMPILE_COMMAND
            ShaderMake
                --config ${SHADER_CONFIG}
                --out ${OUTPUT_SPIRV}
                --platform SPIRV
                --binaryBlob --outputExt .bin
                -D SPIRV
                -D TARGET_VULKAN
                --compiler "${DXC_SPIRV_PATH}"
                ${NVRHI_DEFAULT_VK_REGISTER_OFFSETS}
                --vulkanVersion 1.2
                ${use_api_arg}
        )
    endif()

    if (NOT COMPILE_COMMAND)
        message(FATAL_ERROR "compile_shaders: compile command empty - internal error")
    endif ()

    add_custom_command(
        TARGET
            ${TARGET_NAME} PRE_BUILD
        COMMAND
            ${COMPILE_COMMAND}
    )

    # Make sure we call this when building a specific target
    add_dependencies(${TARGET} ${TARGET_NAME})
endfunction()

# Copy shaders from default build directory into another directory
macro(copy_shaders TARGET OUTPUT)
    add_custom_command(
        TARGET
            ${TARGET} POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory
                ${_SHADER_OUTPUT_DIR}
                ${OUTPUT}
    )

    # Update install with shaders
    install(
        DIRECTORY ${OUTPUT}
        TYPE BIN
        TYPE LIB
    )
endmacro()

# Copy textures from texture folder into another directory
macro(copy_textures TARGET OUTPUT)
    set(_TEXTURE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/textures)

    if (IS_DIRECTORY ${_TEXTURE_DIR})
        add_custom_command(
            TARGET
                ${TARGET} POST_BUILD
            COMMAND
                ${CMAKE_COMMAND} -E copy_directory
                    ${_TEXTURE_DIR}
                    ${OUTPUT}
        )

        # Update install with shaders
        install(
            DIRECTORY ${OUTPUT}
            TYPE BIN
            TYPE LIB
        )
    endif ()
endmacro()