lib_create_options(LIB_COMMON_LOGGER "Set log level (0 = Off, 1 = Errors, 2 = Warnings, 3 = Info, 4 = Debug" 0 1 2 3 4)

add_subdirectory(threading)

target_sources(${PROJECT_NAME} PRIVATE
        config_manager.cpp
        math.cpp
        timer.cpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        include
        )