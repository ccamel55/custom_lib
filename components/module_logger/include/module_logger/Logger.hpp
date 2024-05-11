#pragma once

#include <module_logger/BaseHandler.hpp>
#include <module_core/Singleton.hpp>

#include <memory>
#include <vector>

namespace lib::logger
{
//! Lightweight logging class inspired by Timber for Java
class Logger: public Singleton<Logger> {
public:
    //! Plant a new log handler which will process incoming logs.
    void plant(const std::shared_ptr<BaseHandler>& handler);

    //! Remove a log handler.
    void uproot(const std::shared_ptr<BaseHandler>& handler);

    //! Remove all log handlers.
    void uproot_all();

    //! Print log at specific log level. This API should not be used directly, instead use the macros defined.
    void print(log_level log_level, const std::string& message) const;

private:
    std::vector<std::shared_ptr<BaseHandler>> _handlers = {};

};
}