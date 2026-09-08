#ifndef VNLC_INTERFACE_BODY_PARSING_RESULT_HPP
#define VNLC_INTERFACE_BODY_PARSING_RESULT_HPP

#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include <memory>
#include <vector>

struct VnlcInterfaceBodyParsingResult {
    std::vector<std::unique_ptr<VnlcFunctionDeclarationNode>> declarations;
};

#endif // VNLC_INTERFACE_BODY_PARSING_RESULT_HPP
