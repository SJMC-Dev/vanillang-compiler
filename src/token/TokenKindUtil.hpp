#ifndef VNLC_TOKEN_KIND_UTIL_HPP
#define VNLC_TOKEN_KIND_UTIL_HPP

#include "token/TokenKind.hpp"
#include <string_view>

namespace vnlc {
    namespace TokenKindUtil {
        [[nodiscard]] std::string_view toString(TokenKind kind);
        bool isGeneralizedIdentifier(TokenKind kind);
    } // namespace TokenKindUtil
} // namespace vnlc

#endif // VNLC_TOKEN_KIND_UTIL_HPP
