#include "Diagnostic.hpp"

namespace vnlc {
    Diagnostic::Diagnostic(DiagnosticPhase phase, DiagnosticSeverity severity, std::string message, std::size_t line, std::size_t column, std::size_t offset, std::size_t length)
        : phase(phase),
          severity(severity),
          message(std::move(message)),
          line(line),
          column(column),
          offset(offset),
          length(length) {}

    DiagnosticPhase Diagnostic::getPhase() const {
        return phase;
    }

    DiagnosticSeverity Diagnostic::getSeverity() const {
        return severity;
    }

    std::string_view Diagnostic::getMessage() const {
        return message;
    }

    std::size_t Diagnostic::getLine() const {
        return line;
    }

    std::size_t Diagnostic::getColumn() const {
        return column;
    }

    std::size_t Diagnostic::getOffset() const {
        return offset;
    }

    std::size_t Diagnostic::getLength() const {
        return length;
    }
} // namespace vnlc
