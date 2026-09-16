#include "VnlcCustomizedType.hpp"
#include "type/VnlcCustomizedTypeOrigin.hpp"

VnlcCustomizedType::VnlcCustomizedType(VnlcCustomizedTypeKind customizedKind, std::string_view fullTypeName, const VnlcTypeDeclarationNode* localNode)
    : VnlcSemanticType(),
      customizedKind(customizedKind),
      origin(VnlcCustomizedTypeOrigin::LOCAL),
      fullTypeName(fullTypeName),
      localNode(localNode),
      importedNode(nullptr) {}

VnlcCustomizedType::VnlcCustomizedType(VnlcCustomizedTypeKind customizedKind, std::string_view fullTypeName, const VnlcImportedIdentifier* importedNode)
    : VnlcSemanticType(),
      customizedKind(customizedKind),
      origin(VnlcCustomizedTypeOrigin::IMPORTED),
      fullTypeName(fullTypeName),
      localNode(nullptr),
      importedNode(importedNode) {}

VnlcCustomizedTypeKind VnlcCustomizedType::getCustomizedKind() const noexcept {
    return customizedKind;
}

VnlcCustomizedTypeOrigin VnlcCustomizedType::getOrigin() const noexcept {
    return origin;
}

std::string_view VnlcCustomizedType::getFullTypeName() const noexcept {
    return fullTypeName;
}

const VnlcTypeDeclarationNode* VnlcCustomizedType::getLocalNode() const noexcept {
    return localNode;
}

const VnlcImportedIdentifier* VnlcCustomizedType::getImportedNode() const noexcept {
    return importedNode;
}
