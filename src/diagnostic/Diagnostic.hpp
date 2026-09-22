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

        std::size_t line;
        std::size_t column;
        std::size_t offset;
        std::size_t length;

    public:
        Diagnostic(DiagnosticPhase phase, DiagnosticSeverity severity, std::string message, std::size_t line, std::size_t column, std::size_t offset, std::size_t length);

        [[nodiscard]] DiagnosticPhase getPhase() const;
        [[nodiscard]] DiagnosticSeverity getSeverity() const;
        [[nodiscard]] std::string_view getMessage() const;
        [[nodiscard]] std::size_t getLine() const;
        [[nodiscard]] std::size_t getColumn() const;
        [[nodiscard]] std::size_t getOffset() const;
        [[nodiscard]] std::size_t getLength() const;
    };
} // namespace vnlc

#endif // VNLC_DIAGNOSTIC_HPP