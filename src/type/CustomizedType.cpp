#include "CustomizedType.hpp"
#include "type/CustomizedTypeOrigin.hpp"

namespace vnlc {
    CustomizedType::CustomizedType(CustomizedTypeKind customizedKind, std::string_view fullTypeName, std::vector<const SemanticType*>&& genericArguments, const TypeDeclarationNode* localNode)
        : SemanticType(),
          customizedKind(customizedKind),
          origin(CustomizedTypeOrigin::LOCAL),
          fullTypeName(fullTypeName),
          genericArguments(std::move(genericArguments)),
          localNode(localNode),
          importedNode(nullptr) {}

    CustomizedType::CustomizedType(
        CustomizedTypeKind customizedKind,
        std::string_view fullTypeName,
        std::vector<const SemanticType*>&& genericArguments,
        const ImportedIdentifier* importedNode
    )
        : SemanticType(),
          customizedKind(customizedKind),
          origin(CustomizedTypeOrigin::IMPORTED),
          fullTypeName(fullTypeName),
          genericArguments(std::move(genericArguments)),
          localNode(nullptr),
          importedNode(importedNode) {}

    CustomizedTypeKind CustomizedType::getCustomizedKind() const noexcept {
        return customizedKind;
    }

    CustomizedTypeOrigin CustomizedType::getOrigin() const noexcept {
        return origin;
    }

    std::string_view CustomizedType::getFullTypeName() const noexcept {
        return fullTypeName;
    }

    const std::vector<const SemanticType*> CustomizedType::getGenericArguments() const noexcept {
        return genericArguments;
    }

    const TypeDeclarationNode* CustomizedType::getLocalNode() const noexcept {
        return localNode;
    }

    const ImportedIdentifier* CustomizedType::getImportedNode() const noexcept {
        return importedNode;
    }
} // namespace vnlc
