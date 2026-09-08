#ifndef VNLC_TOKEN_TYPE_UTIL_HPP
#define VNLC_TOKEN_TYPE_UTIL_HPP

#include "token/VnlcTokenType.hpp"
#include <string_view>

namespace VnlcTokenTypeUtil {
    [[nodiscard]] std::string_view toString(VnlcTokenType type);
    bool isGeneralizedIdentifier(VnlcTokenType type);
} // namespace VnlcTokenTypeUtil

#endif // VNLC_TOKEN_TYPE_UTIL_HPP
