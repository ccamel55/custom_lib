#include <catch2/catch_test_macros.hpp>

#include <module_threading/ThreadPool.hpp>

TEST_CASE("Thread Pool - init", "[threading]") {
   lib::threading::ThreadPool thread_pool(1);

    REQUIRE(thread_pool.size() == 0);
    REQUIRE(thread_pool.empty() == true);
}

TEST_CASE("Thread pool - basic", "[threading]") {
    lib::threading::ThreadPool thread_pool(100);

    for (size_t i = 0; i < 10; i++) {
        thread_pool.emplace([&]{
            std::this_thread::sleep_for(std::chrono::seconds(1));
        });
    }

    REQUIRE(!thread_pool.empty());

    thread_pool.clear();

    REQUIRE(thread_pool.empty());
}

TEST_CASE("Thread pool - future", "[threading]") {
    lib::threading::ThreadPool thread_pool(100);

    std::atomic<int> counter = 0;
    std::vector<std::future<void>> futures = {};

    for (size_t i = 0; i < 10; i++) {
        futures.emplace_back(thread_pool.emplace([&](int increment){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            counter += increment;
        }, i));
    }

    REQUIRE(!thread_pool.empty());

    for (auto& future: futures) {
        future.wait();
    }

    REQUIRE(thread_pool.empty());
    REQUIRE(counter == 45);
}

TEST_CASE("Thread pool - different return types", "[threading]") {
    lib::threading::ThreadPool thread_pool(100);

    auto result_1 = thread_pool.emplace([](){
        return "Result 1";
    });

    auto result_2 = thread_pool.emplace([&](){
        return 0x0123;
    });

    auto result_3 = thread_pool.emplace([](float some_float){
        return some_float;
    }, 124.0);

    auto val_1 = std::string(result_1.get());
    REQUIRE(std::equal(val_1.begin(), val_1.end(),"Result 1"));

    REQUIRE(result_2.get() == 0x0123);
    REQUIRE(result_3.get() == 124.0);
}