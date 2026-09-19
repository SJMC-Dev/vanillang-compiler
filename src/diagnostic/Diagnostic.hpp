#ifndef VNLC_DIAGNOSTIC_HPP
#define VNLC_DIAGNOSTIC_HPP

#include "diagnostic/DiagnosticPhase.hpp"
#include "diagnostic/DiagnosticSeverity.hpp"
#include <string>
#include <string_view>

namespace vnlc {
    class Diagnostic {
    private:
        DiagnosticPhase phase;
        DiagnosticSeverity severity;
        std::string message;

        unsigned int line;
        unsigned int column;
        unsigned int offset;
        unsigned int length;

    public:
        Diagnostic(DiagnosticPhase phase, DiagnosticSeverity severity, std::string message, unsigned int line, unsigned int column, unsigned int offset, unsigned int length);

        [[nodiscard]] DiagnosticPhase getPhase() const;
        [[nodiscard]] DiagnosticSeverity getSeverity() const;
        [[nodiscard]] std::string_view getMessage() const;
        [[nodiscard]] unsigned int getLine() const;
        [[nodiscard]] unsigned int getColumn() const;
        [[nodiscard]] unsigned int getOffset() const;
        [[nodiscard]] unsigned int getLength() const;
    };
} // namespace vnlc

#endif // VNLC_DIAGNOSTIC_HPP