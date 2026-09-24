#ifndef VNLC_DIAGNOSTIC_PHASE_HPP
#define VNLC_DIAGNOSTIC_PHASE_HPP

namespace vnlc {
    enum class DiagnosticPhase {
        LEXEME,
        COLLECTOR,
        SYNTAX,
        SEMANTIC,
        CODEGEN,
        INTERNAL,
    };
}

#endif // VNLC_DIAGNOSTIC_PHASE_HPP
