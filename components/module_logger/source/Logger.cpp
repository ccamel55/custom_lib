#include <module_logger/Logger.hpp>

using namespace lib::logger;

void Logger::plant(const std::shared_ptr<BaseHandler>& handler) {
    // push_back will make copy, hence increment reference counter for shared_ptr
    _handlers.push_back(handler);
}

void Logger::uproot(const std::shared_ptr<BaseHandler>& handler) {
    // check ptr addresses since it MUST be the same... RIGHT??
    std::erase_if(_handlers, [&](const std::shared_ptr<BaseHandler>& i) {
        return i.get() == handler.get();
    });
}

void Logger::uproot_all() {
    _handlers.clear();
}

void Logger::print(log_level log_level, const std::string& message) const {
    for (const auto& handler: _handlers) {
        handler->write_to_log(log_level, message);
    }
}
