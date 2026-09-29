#include "TypeReferenceNodeUtil.hpp"
#include "ast/typeref/CustomizedTypeReferenceNode.hpp"
#include "ast/typeref/PrimitiveTypeReferenceNode.hpp"

namespace vnlc {
    std::string TypeReferenceNodeUtil::generateNamespaceIdFromTypeName(const TypeReferenceNode& typeNode) {
        if (const auto* primitiveType = dynamic_cast<const PrimitiveTypeReferenceNode*>(&typeNode)) {
            return std::string(primitiveType->getPrimitiveTypeName());
        }

        const auto* customizedType = dynamic_cast<const CustomizedTypeReferenceNode*>(&typeNode);
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
                identifier += generateNamespaceIdFromTypeName(*genericArgument) + "-";
            }
            identifier.pop_back();
            identifier += "-.";
        }

        return identifier;
    }
} // namespace vnlc
