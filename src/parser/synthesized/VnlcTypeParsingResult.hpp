#ifndef VNLC_TYPE_PARSING_RESULT_HPP
#define VNLC_TYPE_PARSING_RESULT_HPP

#include "ast/type/VnlcTypeNode.hpp"
#include <memory>

struct VnlcTypeParsingResult {
    std::unique_ptr<VnlcTypeNode> type;
};

#endif // VNLC_TYPE_PARSING_RESULT_HPP
