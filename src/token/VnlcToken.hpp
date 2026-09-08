#ifndef VNLC_TOKEN_HPP
#define VNLC_TOKEN_HPP

#include "token/VnlcTokenType.hpp"

#include <string>
#include <string_view>

class VnlcToken {
private:
    VnlcTokenType type;
    std::string value;
    unsigned int offset;
    unsigned int line;
    unsigned int column;

public:
    VnlcToken(VnlcTokenType type, std::string_view value, unsigned int line, unsigned int column, unsigned int offset);
    VnlcToken(VnlcToken&& other) noexcept;
    VnlcToken(const VnlcToken& other);

    VnlcToken& operator=(VnlcToken&& other) noexcept;
    VnlcToken& operator=(const VnlcToken& other);

    [[nodiscard]] VnlcTokenType getType() const noexcept;
    [[nodiscard]] unsigned int getLine() const noexcept;
    [[nodiscard]] unsigned int getColumn() const noexcept;
    [[nodiscard]] unsigned int getOffset() const noexcept;
    [[nodiscard]] unsigned int getLength() const noexcept;
    [[nodiscard]] std::string_view getValue() const noexcept;
};

#endif // VNLC_TOKEN_HPP
