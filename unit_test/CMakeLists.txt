# create test target
project(custom_lib_tests)

include(CTest)
CPMAddPackage("gh:catchorg/Catch2@3.4.0")

add_executable(${PROJECT_NAME}
        cryptography_tests.cpp
        math_tests.cpp
        threading_tests.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Catch2::Catch2WithMain
        custom_lib
)

# add target to CTest
enable_testing()
add_test(Tests ${PROJECT_NAME})