#include "RunningModeUtil.hpp"
#include <CLI/CLI.hpp>

namespace vnlc {
    RunningMode RunningModeUtil::getRunningMode(std::string_view modeLiteral) {
        if (modeLiteral == "compile") {
            return RunningMode::COMPILE;
        } else if (modeLiteral == "check") {
            return RunningMode::CHECK;
        } else if (modeLiteral == "lint") {
            return RunningMode::LINT;
        } else if (modeLiteral == "format") {
            return RunningMode::FORMAT;
        } else if (modeLiteral == "dump-ast") {
            return RunningMode::DUMP_AST;
        } else {
            throw CLI::ValidationError("Invalid running mode: " + std::string(modeLiteral));
        }
    }
} // namespace vnlc
