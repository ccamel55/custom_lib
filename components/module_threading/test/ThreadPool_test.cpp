#include <catch2/catch_test_macros.hpp>

#include <module_threading/ThreadPool.hpp>

TEST_CASE("Thread Pool - init", "[threading]") {
   lib::threading::ThreadPool thread_pool(1);

    REQUIRE(thread_pool.size() == 0);
    REQUIRE(thread_pool.empty() == true);
}

TEST_CASE("Thread pool - basic", "[threading]") {
    lib::threading::ThreadPool thread_pool(100);
    volatile std::atomic<int> counter = 0;

    for (size_t i = 0; i < 10; i++) {
        thread_pool.emplace([&]{
            std::this_thread::sleep_for(std::chrono::seconds(1));
            counter += 1;
        });
    }

    REQUIRE(!thread_pool.empty());

    thread_pool.clear();

    REQUIRE(thread_pool.empty());
}