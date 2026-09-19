#ifndef VNLC_TOKEN_TYPE_UTIL_HPP
#define VNLC_TOKEN_TYPE_UTIL_HPP

#include "token/TokenType.hpp"
#include <string_view>

namespace vnlc {
    namespace TokenTypeUtil {
        [[nodiscard]] std::string_view toString(TokenType type);
        bool isGeneralizedIdentifier(TokenType type);
    } // namespace TokenTypeUtil
} // namespace vnlc

#endif // VNLC_TOKEN_TYPE_UTIL_HPP
