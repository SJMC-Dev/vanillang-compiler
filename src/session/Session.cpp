#include "Session.hpp"
#include "log/Logger.hpp"

namespace vnlc {
    Session::Session(Config&& config) : config(config) {}

    void Session::run() {
        VNLC_LOG_INFO("Session started.");

        // TODO: Implement the main logic of the session, including lexing, parsing, semantic analysis, optimization and code generation.
    }
} // namespace vnlc
