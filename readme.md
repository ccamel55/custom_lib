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
