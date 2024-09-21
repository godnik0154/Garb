#pragma once

#include "crow/settings.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

namespace crow
{
    enum class LogLevel
    {
#ifndef ERROR
#ifndef DEBUG
        DEBUG = 0,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
#endif
#endif

        Debug = 0,
        Info,
        Warning,
        Error,
        Critical,
    };

    class ILogHandler
    {
    public:
        virtual ~ILogHandler() = default;

        virtual void log(std::string message, LogLevel level) = 0;
    };

    class CerrLogHandler : public ILogHandler
    {
    public:
        void log(std::string message, LogLevel level) override
        {
            std::string prefix;
            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            long remaining_ns = (ns % 1000000000);
            while (remaining_ns >= 10000) remaining_ns /= 10;

            std::cerr << std::string("[") << timestamp() << "."  << remaining_ns << std::string("]");
            std::string Highlight;
            switch (level)
            {
                case LogLevel::Debug:
                    Highlight = CYAN;
                    prefix = "   [DEBUG]  ";
                    break;
                case LogLevel::Info:
                    Highlight = GREEN;
                    prefix = "    [INFO]  ";
                    break;
                case LogLevel::Warning:
                    Highlight = YELLOW;
                    prefix = "  [WARNING] ";
                    break;
                case LogLevel::Error:
                    Highlight = RED;
                    prefix = "   [ERROR]  ";
                    break;
                case LogLevel::Critical:
                    Highlight = MAGENTA;
                    prefix = " [CRITICAL] ";
                    break;
            }
            #ifdef CROW_FANCY_LOG
            std::cerr << BOLD << Highlight << prefix << std::string(" ") << RESET << message << std::endl;
            #else
            std::cerr << prefix << std::string(" ") << message << std::endl;
            #endif
        }

    private:
        static std::string timestamp()
        {
            char date[32];
            time_t t = time(0);

            tm my_tm;

#if defined(_MSC_VER) || defined(__MINGW32__)
#ifdef CROW_USE_LOCALTIMEZONE
            localtime_s(&my_tm, &t);
#else
            gmtime_s(&my_tm, &t);
#endif
#else
#ifdef CROW_USE_LOCALTIMEZONE
            localtime_r(&t, &my_tm);
#else
            gmtime_r(&t, &my_tm);
#endif
#endif

            size_t sz = strftime(date, sizeof(date), "%d-%m-%Y %H:%M:%S", &my_tm);
            return std::string(date, date + sz);
        }
    };

    class logger
    {
    public:
        logger(LogLevel level):
          level_(level)
        {}
        ~logger()
        {
#ifdef CROW_ENABLE_LOGGING
            if (level_ >= get_current_log_level())
            {
                get_handler_ref()->log(stringstream_.str(), level_);
            }
#endif
        }

        //
        template<typename T>
        logger& operator<<(T const& value)
        {
#ifdef CROW_ENABLE_LOGGING
            if (level_ >= get_current_log_level())
            {
                stringstream_ << value;
            }
#endif
            return *this;
        }

        //
        static void setLogLevel(LogLevel level) { get_log_level_ref() = level; }

        static void setHandler(ILogHandler* handler) { get_handler_ref() = handler; }

        static LogLevel get_current_log_level() { return get_log_level_ref(); }

    private:
        //
        static LogLevel& get_log_level_ref()
        {
            static LogLevel current_level = static_cast<LogLevel>(CROW_LOG_LEVEL);
            return current_level;
        }
        static ILogHandler*& get_handler_ref()
        {
            static CerrLogHandler default_handler;
            static ILogHandler* current_handler = &default_handler;
            return current_handler;
        }

        //
        std::ostringstream stringstream_;
        LogLevel level_;
    };
} // namespace crow

#define CROW_LOG_CRITICAL                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Critical) \
    crow::logger(crow::LogLevel::Critical)
#define CROW_LOG_ERROR                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Error) \
    crow::logger(crow::LogLevel::Error)
#define CROW_LOG_WARNING                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Warning) \
    crow::logger(crow::LogLevel::Warning)
#define CROW_LOG_INFO                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Info) \
    crow::logger(crow::LogLevel::Info)
#define CROW_LOG_DEBUG                                                  \
    if (crow::logger::get_current_log_level() <= crow::LogLevel::Debug) \
    crow::logger(crow::LogLevel::Debug)
