#pragma once

#include <module_logger/Logger.hpp>

namespace lib::logger {
    //! Log manager than handles logs in the current scope.
    class ScopeLog {
    public:
        ScopeLog(const std::shared_ptr<Logger>& logger, const std::string& tag, bool flush_on_scope_leave = false)
            : _logger(logger)
            , _tag(tag)
            , _flush_on_scope_leave(flush_on_scope_leave) {

        }

        ~ScopeLog() {
            const auto logger = _logger.lock();
            if (!_flush_on_scope_leave || !logger) {
                return;
            }

            logger->flush();
        }

        //! Write a string log.
        //! \param level Log level.
        //! \param message String containing message body.
        //! \param args Packed arguments list containing things to be formatted.
        template<typename... Args>
        void print(log_level level, const fmt::format_string<Args...>& message, Args&&... args) {
            _logger.lock()->print(level, _tag, message, std::forward<Args>(args)...);
        }

        //! Write a log of type ERROR
        //! \param message String containing message body.
        //! \param args Packed arguments list containing things to be formatted.
        template<typename... Args>
        void e(const fmt::format_string<Args...>& message, Args&&... args) {
            print(log_level::ERROR, message, std::forward<Args>(args)...);
        }

        //! Write a log of type WARNING
        //! \param message String containing message body.
        //! \param args Packed arguments list containing things to be formatted.
        template<typename... Args>
        void w(const fmt::format_string<Args...>& message, Args&&... args) {
            print(log_level::WARNING, message, std::forward<Args>(args)...);
        }

        //! Write a log of type INFO
        //! \param message String containing message body.
        //! \param args Packed arguments list containing things to be formatted.
        template<typename... Args>
        void i(const fmt::format_string<Args...>& message, Args&&... args) {
            print(log_level::INFO, message, std::forward<Args>(args)...);
        }

        //! Write a log of type DEBUG
        //! \param message String containing message body.
        //! \param args Packed arguments list containing things to be formatted.
        template<typename... Args>
        void d(const fmt::format_string<Args...>& message, Args&&... args) {
            print(log_level::DEBUG, message, std::forward<Args>(args)...);
        }

        //! Write a log of type VERBOSE
        //! \param message String containing message body.
        //! \param args Packed arguments list containing things to be formatted.
        template<typename... Args>
        void v(const fmt::format_string<Args...>& message, Args&&... args) {
            print(log_level::VERBOSE, message, std::forward<Args>(args)...);
        }

    private:
        std::weak_ptr<Logger> _logger   = {};
        std::string _tag                = {};
        bool _flush_on_scope_leave      = false;

    };
}
