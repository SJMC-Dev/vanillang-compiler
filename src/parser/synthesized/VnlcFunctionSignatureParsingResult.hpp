#ifndef VNLC_FUNCTION_SIGNATURE_PARSING_RESULT_HPP
#define VNLC_FUNCTION_SIGNATURE_PARSING_RESULT_HPP

#include "ast/declaration/VnlcValueDeclarationNode.hpp"
#include "ast/type/VnlcTypeNode.hpp"
#include <memory>
#include <optional>
#include <vector>

struct VnlcFunctionSignatureParsingResult {
    std::unique_ptr<VnlcIdentifierNode> name;
    std::vector<std::unique_ptr<VnlcValueDeclarationNode>> parameters;
    std::optional<std::unique_ptr<VnlcTypeNode>> returnType;
};

#endif // VNLC_FUNCTION_SIGNATURE_PARSING_RESULT_HPP