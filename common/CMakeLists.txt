add_subdirectory(threading)

target_sources(${PROJECT_NAME} PUBLIC
        config_manager.cpp
        timer.cpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        include
        )