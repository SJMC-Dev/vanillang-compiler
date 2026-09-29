#ifndef VNLC_INTERFACE_BODY_PARSING_RESULT_HPP
#define VNLC_INTERFACE_BODY_PARSING_RESULT_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    struct InterfaceBodyParsingResult {
        std::vector<std::unique_ptr<DeclarationNode>> declarations;
    };
} // namespace vnlc

#endif // VNLC_INTERFACE_BODY_PARSING_RESULT_HPP
