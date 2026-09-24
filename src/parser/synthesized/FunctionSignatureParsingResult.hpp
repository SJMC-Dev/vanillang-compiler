#ifndef VNLC_FUNCTION_SIGNATURE_PARSING_RESULT_HPP
#define VNLC_FUNCTION_SIGNATURE_PARSING_RESULT_HPP

#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/typeref/TypeReferenceNode.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace vnlc {
    struct FunctionSignatureParsingResult {
        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
        std::optional<std::unique_ptr<TypeReferenceNode>> returnType;
    };
} // namespace vnlc

#endif // VNLC_FUNCTION_SIGNATURE_PARSING_RESULT_HPP