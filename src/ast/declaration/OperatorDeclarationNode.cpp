#include "OperatorDeclarationNode.hpp"
#include "util/TypeReferenceNodeUtil.hpp"

namespace vnlc {
    OperatorDeclarationNode::OperatorDeclarationNode(
        OperatorDeclarationKind::Kind kind,
        OperatorDeclarationKind::Context context,
        OperatorDeclarationKind::AccessModifier accessModifier,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::optional<std::unique_ptr<TypeReferenceNode>>&& returnType,
        std::optional<std::unique_ptr<BlockStatementNode>>&& body,
        const Token& firstToken,
        const Token& lastToken
    )
        : DeclarationNode(firstToken, lastToken),
          kind(kind),
          context(context),
          accessModifier(accessModifier),
          parameters(std::move(parameters)),
          returnType(std::move(returnType)),
          body(std::move(body)) {
        internalName = generateInternalName();
    }

    OperatorDeclarationNode::OperatorDeclarationNode(
        OperatorDeclarationKind::Kind kind,
        OperatorDeclarationKind::Context context,
        OperatorDeclarationKind::AccessModifier accessModifier,
        std::vector<std::unique_ptr<ValueDeclarationNode>>&& parameters,
        std::optional<std::unique_ptr<TypeReferenceNode>>&& returnType,
        std::optional<std::unique_ptr<BlockStatementNode>>&& body,
        const Token& firstToken,
        const Token& lastToken,
        std::vector<DeclarationItem::MetadataTerm>&& metadataTerms
    )
        : DeclarationNode(firstToken, lastToken, std::move(metadataTerms)),
          kind(kind),
          context(context),
          accessModifier(accessModifier),
          parameters(std::move(parameters)),
          returnType(std::move(returnType)),
          body(std::move(body)) {
        internalName = generateInternalName();
    }

    std::string OperatorDeclarationNode::generateInternalName() const {
        std::string identifier = "__vnl_operator_" + std::string(getOperatorName());

        if (!parameters.empty()) {
            identifier += '-';
            for (const auto& parameter : parameters) {
                if (parameter->getType().has_value()) {
                    identifier += '-' + TypeReferenceNodeUtil::generateNamespaceIdFromTypeName(*parameter->getType().value());
                }
            }
        }

        identifier += "__";
        return identifier;
    }

    const OperatorDeclarationKind::Kind OperatorDeclarationNode::getKind() const noexcept {
        return kind;
    }

    const OperatorDeclarationKind::Context OperatorDeclarationNode::getContext() const noexcept {
        return context;
    }

    const OperatorDeclarationKind::AccessModifier OperatorDeclarationNode::getAccessModifier() const noexcept {
        return accessModifier;
    }

    std::string_view OperatorDeclarationNode::getInternalName() const noexcept {
        return internalName;
    }

    std::string_view OperatorDeclarationNode::getOperatorName() const noexcept {
        switch (kind) {
            case OperatorDeclarationKind::Kind::ADDITION:
                return "addition";
            case OperatorDeclarationKind::Kind::SUBTRACTION:
                return "subtraction";
            case OperatorDeclarationKind::Kind::MULTIPLICATION:
                return "multiplication";
            case OperatorDeclarationKind::Kind::DIVISION:
                return "division";
            case OperatorDeclarationKind::Kind::INTEGER_DIVISION:
                return "integerDivision";
            case OperatorDeclarationKind::Kind::MODULO:
                return "modulo";
            case OperatorDeclarationKind::Kind::EXPONENT:
                return "exponent";
            case OperatorDeclarationKind::Kind::EQUAL:
                return "equality";
            case OperatorDeclarationKind::Kind::NOT_EQUAL:
                return "inequality";
            case OperatorDeclarationKind::Kind::LESS_THAN:
                return "lessThan";
            case OperatorDeclarationKind::Kind::GREATER_THAN:
                return "greaterThan";
            case OperatorDeclarationKind::Kind::LESS_THAN_OR_EQUAL:
                return "lessThanOrEqual";
            case OperatorDeclarationKind::Kind::GREATER_THAN_OR_EQUAL:
                return "greaterThanOrEqual";
            case OperatorDeclarationKind::Kind::BITWISE_AND:
                return "bitwiseAnd";
            case OperatorDeclarationKind::Kind::BITWISE_OR:
                return "bitwiseOr";
            case OperatorDeclarationKind::Kind::BITWISE_XOR:
                return "bitwiseXor";
            case OperatorDeclarationKind::Kind::BITWISE_NOT:
                return "bitwiseNot";
            case OperatorDeclarationKind::Kind::SHIFT_LEFT:
                return "shiftLeft";
            case OperatorDeclarationKind::Kind::SHIFT_RIGHT:
                return "shiftRight";
            case OperatorDeclarationKind::Kind::SHIFT_RIGHT_UNSIGNED:
                return "unsignedShiftRight";
            case OperatorDeclarationKind::Kind::CALL:
                return "call";
            case OperatorDeclarationKind::Kind::SUBSCRIPT:
                return "subscript";
            case OperatorDeclarationKind::Kind::RANGE:
                return "range";
        }
        return {};
    }

    const std::vector<std::unique_ptr<ValueDeclarationNode>>& OperatorDeclarationNode::getParameters() const noexcept {
        return parameters;
    }

    const std::optional<std::unique_ptr<TypeReferenceNode>>& OperatorDeclarationNode::getReturnType() const noexcept {
        return returnType;
    }

    const std::optional<std::unique_ptr<BlockStatementNode>>& OperatorDeclarationNode::getBody() const noexcept {
        return body;
    }
} // namespace vnlc
