#ifndef VNLC_CUSTOMIZED_TYPE_HPP
#define VNLC_CUSTOMIZED_TYPE_HPP

#include "ast/declaration/VnlcTypeDeclarationNode.hpp"
#include "type/VnlcCustomizedTypeKind.hpp"
#include "type/VnlcCustomizedTypeOrigin.hpp"
#include "type/VnlcSemanticType.hpp"
#include "vni/import/VnlcImportedIdentifier.hpp"
#include <string>
#include <string_view>

class VnlcCustomizedType : public VnlcSemanticType {
private:
    VnlcCustomizedTypeKind customizedKind;
    VnlcCustomizedTypeOrigin origin;
    std::string fullTypeName;

    const VnlcTypeDeclarationNode* localDeclarationNode;
    const VnlcImportedIdentifier* importedDeclarationNode;

public:
    VnlcCustomizedType(VnlcCustomizedTypeKind customizedKind, std::string_view fullTypeName, const VnlcTypeDeclarationNode* localDeclarationNode);
    VnlcCustomizedType(VnlcCustomizedTypeKind customizedKind, std::string_view fullTypeName, const VnlcImportedIdentifier* importedDeclarationNode);

    [[nodiscard]] std::string_view getFullTypeName() const noexcept override;
    [[nodiscard]] VnlcCustomizedTypeKind getCustomizedKind() const noexcept;
    [[nodiscard]] VnlcCustomizedTypeOrigin getOrigin() const noexcept;

    [[nodiscard]] const VnlcTypeDeclarationNode* getLocalDeclaration() const noexcept;
    [[nodiscard]] const VnlcImportedIdentifier* getImportedDeclaration() const noexcept;
};

#endif // VNLC_CUSTOMIZED_TYPE_HPP