# custom_lib

- Custom lib is a library of code that I commonly use in between projects. 
- This library is designed to make starting new projects easier and reduce the amount of repeated code between all my personal projects.

## Support

- currently supports gcc and clang compilers (both gnu and msvc front ends)
- tested on x86 and arm64 (Apple M1 Max)

## Future goals

- 3D rendering engine, the goal is to eventually integrate a 3D rendering engine which can use any rendering backend that is implemented
- abstract SIMD trig approximation and math functions

## Usage

Note: if you include this library, it will add some predefined compiler options. These options change depending on `CMAKE_BUILD_TYPE` but they all share the following options: disable RTTI, disable exceptions, enable all warnings.

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
set(LIB_RENDERING gl3)
```

## Structure

- `cmake` holds all `.cmake` files which include automatic configuration scripts, custom functions and predefined configs.

- `components` contains all the modular components of the library. Each component can be enabled/disabled depending on what you are doing. 

- `core_sdk` contains core types and methods used some components. This is always included but is designed to be as minimal as possible.

- `dependencies` contains third party dependencies used by the library.

- `unit_test` contains all unit tests. if the library is loaded as the top level `CMakeLists.txt` then this subdirectory is included and is buildable and runnable.



