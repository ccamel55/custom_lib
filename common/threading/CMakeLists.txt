lib_create_options(LIB_COMMON_THREADING "Enables threading components" true false)

if (LIB_COMMON_THREADING STREQUAL true)
    target_sources(${PROJECT_NAME} PRIVATE
            async_caller.cpp
            thread_pool.cpp
            )
endif ()

