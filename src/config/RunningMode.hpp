#ifndef VNLC_RUNNING_MODE_HPP
#define VNLC_RUNNING_MODE_HPP

namespace vnlc {
    enum class RunningMode {
        COMPILE,
        CHECK,
        LINT,
        FORMAT,
        DUMP_AST,
    };
}

#endif // VNLC_RUNNING_MODE_HPP