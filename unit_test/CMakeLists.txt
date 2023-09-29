lib_create_options(LIB_UNIT_TEST_ENABLE "Build and run unit tests." false true)

if (LIB_UNIT_TEST_ENABLE STREQUAL true)

    # create test target
    project(custom_lib_tests)

    include(CTest)
    CPMAddPackage("gh:catchorg/Catch2@3.4.0")

    add_executable(unit_tests
            cryptography_tests.cpp
            math_tests.cpp
            threading_tests.cpp
    )

    target_link_libraries(unit_tests PRIVATE
            Catch2::Catch2WithMain
            custom_lib
    )

    # add target to CTest
    enable_testing()
    add_test(custom_lib_tests unit_tests)
endif()

