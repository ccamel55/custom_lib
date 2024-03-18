#include <core_sdk/logger/logger.hpp>

using namespace lib;

void logger::plant(const std::shared_ptr<log_handler>& handler)
{
    // push_back will make copy, hence increment reference counter for shared_ptr
    get()._handlers.push_back(handler);
}

void logger::uproot(const std::shared_ptr<log_handler>& handler)
{
    // check ptr addresses since it MUST be the same... RIGHT??
    auto& handlers = get()._handlers;
    std::erase_if(handlers, [&](const std::shared_ptr<log_handler>& i)
    {
        return i.get() == handler.get();
    });
}

void logger::uproot_all()
{
    get()._handlers.clear();
}