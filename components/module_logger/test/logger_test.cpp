#include <catch2/catch_test_macros.hpp>

#include <module_logger/Logger.hpp>
#include <module_logger/ScopedLog.hpp>

#include <module_logger/handler/Std_Handler.hpp>

namespace {
class Test_Handler {
public:
    void log(const lib::logger::log_message_t& message) {

    }

    void flush() {

    }

private:

};

}

TEST_CASE("Logger - true", "[logger]") {
    REQUIRE(true);
}

TEST_CASE("Logger - test", "[logger]") {
    lib::logger::Logger<Test_Handler>::get(); {
        lib::logger::ScopeLog<lib::logger::Std_Handler> scoped_log("poop");
        scoped_log.log(lib::logger::log_level::INFO, "Hello from the poop {}", 0);
    } {
        lib::logger::ScopeLog<lib::logger::Std_Handler> scoped_log("peep");
        scoped_log.log(lib::logger::log_level::INFO, "Hello from the peeo {}", 690);
    }
}
