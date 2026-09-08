#ifndef VNLC_CONSTRUCTOR_PARSING_RESULT_HPP
#define VNLC_CONSTRUCTOR_PARSING_RESULT_HPP

#include "ast/declaration/VnlcFunctionDeclarationNode.hpp"
#include <memory>

struct VnlcConstructorParsingResult {
    std::unique_ptr<VnlcFunctionDeclarationNode> constructor;
};

#endif // VNLC_CONSTRUCTOR_PARSING_RESULT_HPP