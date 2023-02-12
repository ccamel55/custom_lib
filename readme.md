# custom_lib

- Custom lib is a library of code that I commonly use in between projects

### Usage

- Clone this repository recursively 
- Add `custom_lib` as a subdirectory 
- Link the library to your project by adding the following
```cmake
target_link_libraries(${PROJECT_NAME}
        custom_lib
        )
```
- Specify the options defined within the `CMakeLists.txt` file found within the root directory of this repository 

#### Example

```cmake
cmake_minimum_required(VERSION 3.5)
project(opengl_sandbox)

set(CMAKE_CXX_STANDARD 20)

# config custom_lib
set(LIB_BACKEND_INPUT "glfw")
set(LIB_BACKEND_RENDER "opengl3")
set(LIB_BACKEND_WINDOW "glfw")

add_subdirectory(custom_lib)
add_executable(opengl_sandbox main.cpp)

target_link_libraries(${PROJECT_NAME}
        custom_lib
        )
```