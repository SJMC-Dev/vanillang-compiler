#include "Diagnostic.hpp"

namespace vnlc {
    Diagnostic::Diagnostic(DiagnosticPhase phase, DiagnosticSeverity severity, std::string message, unsigned int line, unsigned int column, unsigned int offset, unsigned int length)
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

    unsigned int Diagnostic::getLine() const {
        return line;
    }

    unsigned int Diagnostic::getColumn() const {
        return column;
    }

    unsigned int Diagnostic::getOffset() const {
        return offset;
    }

    unsigned int Diagnostic::getLength() const {
        return length;
    }
} // namespace vnlc
