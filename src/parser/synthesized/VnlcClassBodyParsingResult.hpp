#ifndef VNLC_CLASS_BODY_PARSING_RESULT_HPP
#define VNLC_CLASS_BODY_PARSING_RESULT_HPP

#include "ast/declaration/VnlcDeclarationNode.hpp"
#include <memory>
#include <vector>

struct VnlcClassBodyParsingResult {
    std::vector<std::unique_ptr<VnlcDeclarationNode>> declarations;
};

#endif // VNLC_CLASS_BODY_PARSING_RESULT_HPP
