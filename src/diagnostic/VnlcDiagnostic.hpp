#ifndef VNLC_DIAGNOSTIC_HPP
#define VNLC_DIAGNOSTIC_HPP

#include "diagnostic/VnlcDiagnosticPhase.hpp"
#include "diagnostic/VnlcDiagnosticSeverity.hpp"
#include <string>
#include <string_view>

class VnlcDiagnostic {
private:
    VnlcDiagnosticPhase phase;
    VnlcDiagnosticSeverity severity;
    std::string message;

    unsigned int line;
    unsigned int column;
    unsigned int offset;
    unsigned int length;

public:
    VnlcDiagnostic(VnlcDiagnosticPhase phase, VnlcDiagnosticSeverity severity, std::string message, unsigned int line, unsigned int column, unsigned int offset, unsigned int length);

    [[nodiscard]] VnlcDiagnosticPhase getPhase() const;
    [[nodiscard]] VnlcDiagnosticSeverity getSeverity() const;
    [[nodiscard]] std::string_view getMessage() const;
    [[nodiscard]] unsigned int getLine() const;
    [[nodiscard]] unsigned int getColumn() const;
    [[nodiscard]] unsigned int getOffset() const;
    [[nodiscard]] unsigned int getLength() const;
};

#endif // VNLC_DIAGNOSTIC_HPP