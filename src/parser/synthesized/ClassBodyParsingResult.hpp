#ifndef VNLC_CLASS_BODY_PARSING_RESULT_HPP
#define VNLC_CLASS_BODY_PARSING_RESULT_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct ClassBodyParsingResult {
        std::vector<std::unique_ptr<DeclarationNode>> declarations;
    };
} // namespace vnlc

#endif // VNLC_CLASS_BODY_PARSING_RESULT_HPP
