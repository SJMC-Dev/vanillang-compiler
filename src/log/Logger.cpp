#include "Logger.hpp"
#include <iostream>

namespace vnlc {
    namespace {
        constexpr std::string_view COLOR_RESET = "\033[0m";    // reset color
        constexpr std::string_view COLOR_DEBUG = "\033[36m";   // cyan
        constexpr std::string_view COLOR_INFO = "\033[32m";    // green
        constexpr std::string_view COLOR_WARN = "\033[33m";    // yellow
        constexpr std::string_view COLOR_ERROR = "\033[31m";   // red
        constexpr std::string_view COLOR_FATAL = "\033[1;31m"; // bold red
    } // namespace

    void Logger::debug(std::string_view message) {
#ifndef NDEBUG
        std::clog << COLOR_DEBUG << "[DEBUG] " << message << COLOR_RESET << std::endl;
#endif
    }

    void Logger::info(std::string_view message) {
        if (!verbose) {
            return;
        }
        std::clog << COLOR_INFO << "[INFO] " << message << COLOR_RESET << std::endl;
    }

    void Logger::warn(std::string_view message) {
        std::clog << COLOR_WARN << "[WARN] " << message << COLOR_RESET << std::endl;
    }

    void Logger::error(std::string_view message) {
        std::cerr << COLOR_ERROR << "[ERROR] " << message << COLOR_RESET << std::endl;
    }

    void Logger::fatal(std::string_view message) {
        std::cerr << COLOR_FATAL << "[FATAL] " << message << COLOR_RESET << std::endl;
        std::exit(EXIT_FAILURE);
    }
} // namespace vnlc
