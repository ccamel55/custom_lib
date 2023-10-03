# custom_lib

- Custom lib is a library of code that I commonly use in between projects. 
- This library is designed to make starting new projects easier and reduce the amount of repeated code between all my personal projects.

## Support

- currently supports gcc and clang compilers (both gnu and msvc front ends)
- tested on x86 and arm64 (Apple M1 Max)

## Future goals

- add CI for formatting, building and running unit tests
- rust bindings so I am able to use features of this library in any new rust projects I want to work on
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
set(LIB_BUILD_DEPS false)
```

## Structure

- `backend` contains abstractions for specific features.

- `cmake` holds all `.cmake` files which include automatic configuration scripts, custom functions and predefined configs.

- `common` includes utilities that may be used in other subdirectories. typically everything here is not target specific and can be used freely without the need of any backends.

- `dependencies` contains third party dependencies used by the library. previously this held all the submodules this library uses, however I have switched to `CPM` for handling my dependencies. 

- `misc` contains scripts and helper macros. like `common` these can be used without any backends.

- `python` contains python script files which are used to help configure and clean the library.

- `scripts` is not the same as `scripts` in the `misc` subdirectory. this `scripts` folder hold bash scripts for the CI.

- `unit_test` contains all unit tests. if the library is loaded as the top level `CMakeLists.txt` then this subdirectory is included and is buildable and runnable.



