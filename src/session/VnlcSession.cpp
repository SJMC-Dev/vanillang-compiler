#include "VnlcSession.hpp"
#include "log/VnlcLogger.hpp"

VnlcSession::VnlcSession(VnlcConfig&& config) : config(config) {}

void VnlcSession::run() {
    VNLC_LOG_INFO("Session started.");

    // TODO: Implement the main logic of the session, including lexing, parsing, semantic analysis, optimization and code generation.
}