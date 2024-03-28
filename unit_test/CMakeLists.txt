project(custom_lib_tests)
include(CTest)

add_executable(${PROJECT_NAME}
        cryptography_tests.cpp
        macro_tests.cpp
        math_tests.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        custom_lib
        Catch2::Catch2WithMain
)

# add target to CTest
enable_testing()
add_test(Tests ${PROJECT_NAME})