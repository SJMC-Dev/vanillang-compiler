#include "VnlcCustomizedType.hpp"
#include "type/VnlcCustomizedTypeOrigin.hpp"

VnlcCustomizedType::VnlcCustomizedType(VnlcCustomizedTypeKind customizedKind, std::string_view fullTypeName, const VnlcTypeDeclarationNode* localDeclarationNode)
    : VnlcSemanticType(),
      customizedKind(customizedKind),
      origin(VnlcCustomizedTypeOrigin::LOCAL),
      fullTypeName(fullTypeName),
      localDeclarationNode(localDeclarationNode),
      importedDeclarationNode(nullptr) {}

VnlcCustomizedType::VnlcCustomizedType(VnlcCustomizedTypeKind customizedKind, std::string_view fullTypeName, const VnlcImportedIdentifier* importedDeclarationNode)
    : VnlcSemanticType(),
      customizedKind(customizedKind),
      origin(VnlcCustomizedTypeOrigin::IMPORTED),
      fullTypeName(fullTypeName),
      localDeclarationNode(nullptr),
      importedDeclarationNode(importedDeclarationNode) {}

VnlcCustomizedTypeKind VnlcCustomizedType::getCustomizedKind() const noexcept {
    return customizedKind;
}

VnlcCustomizedTypeOrigin VnlcCustomizedType::getOrigin() const noexcept {
    return origin;
}

std::string_view VnlcCustomizedType::getFullTypeName() const noexcept {
    return fullTypeName;
}

const VnlcTypeDeclarationNode* VnlcCustomizedType::getLocalDeclaration() const noexcept {
    return localDeclarationNode;
}

const VnlcImportedIdentifier* VnlcCustomizedType::getImportedDeclaration() const noexcept {
    return importedDeclarationNode;
}
