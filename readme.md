# custom_lib

- Custom lib is a library of code that I commonly use in between projects

## Usage

1) Include as a submodule
```cmake
add_subdirectory(custom_lib)
```

2) Link the library
```cmake
target_link_libraries(${PROJECT_NAME}
    custom_lib
)
```

3) Specify options
```cmake
set(LIB_BUILD_DEPS false)
```

4) (Optional) use predefined build flags and/or configs
```cmake
include([location to custom_lib]/cmake/build_options/gcc_base.cmake)
include([location to custom_lib]/cmake/build_options/gcc_debug.cmake)
include([location to custom_lib]/cmake/configs/minimal.cmake)
```

## Structure

`backend` contains abstractions for specific features

`common` includes most of the higher level stuff

`dependencies` contains third part dependencies used by the library




