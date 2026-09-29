#include "TypeReferenceNode.hpp"
#include "ast/typeref/CustomizedTypeReferenceNode.hpp"
#include "ast/typeref/PrimitiveTypeReferenceNode.hpp"

namespace vnlc {
    TypeReferenceNode::TypeReferenceNode(bool questionMarkSuffix, const Token& firstToken, const Token& lastToken) noexcept
        : AstNode(firstToken, lastToken),
          questionMarkSuffix(questionMarkSuffix) {}

    const bool TypeReferenceNode::hasQuestionMarkSuffix() const noexcept {
        return questionMarkSuffix;
    }

    std::string TypeReferenceNode::generateInternalNamePart() const {
        if (const auto* primitiveType = dynamic_cast<const PrimitiveTypeReferenceNode*>(this)) {
            return std::string(primitiveType->getPrimitiveTypeName());
        }

        const auto* customizedType = dynamic_cast<const CustomizedTypeReferenceNode*>(this);
        if (customizedType == nullptr) {
            return {};
        }

        std::string identifier;
        for (const auto& part : customizedType->getNameParts()) {
            identifier += std::string(part->getIdentifierString()) + ".";
        }

        if (identifier.ends_with(".")) {
            identifier.pop_back();
        }

        if (!customizedType->getGenericArguments().empty()) {
            identifier += ".-";
            for (const auto& genericArgument : customizedType->getGenericArguments()) {
                identifier += genericArgument->generateInternalNamePart() + "-";
            }
            identifier.pop_back();
            identifier += "-.";
        }

        return identifier;
    }
} // namespace vnlc
