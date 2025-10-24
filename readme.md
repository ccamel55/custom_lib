# camel_lib

**Refactor Undergoing**

## Component structure

The `components` subdirectory holds modules and dependencies that make up `camel_lib`.
The library will automatically discover all dependencies and modules when configuring 
the project. 

Components must use the following file structure:
```text
component/
├── include/
│   ├── header_1.hpp
│   └── ...
├── include_private/
│   ├── header_private_1.hpp
│   └── ...
├── source/
│   ├── source_file_1.cpp
│   └── ...
├── test/
│   ├── test_1.cpp
│   └── ...
└── CMakeList.txt
```

### Adding modules

Modules are found under the `components` subdirectory. Each module can function 
as an isolated library or as a high level dependency of another module. 

A module must use the following naming structure: `module_[module name]` where `[module_name]` 
is unique. The name of the target must match the name of its directory.

A module must not have submodules. Any new dependencies must be included as
a dependency component. 

### Adding dependencies

Dependencies are found under the `components` subdirectory. Dependency components
wrap imported modules and configure them for use in our library. 

A dependency must use the following naming structure: `dep_[dependency name]`.
The name of the target must match the name of its directory.

Dependencies are imported using [CPM](https://github.com/cpm-cmake/CPM.cmake).

