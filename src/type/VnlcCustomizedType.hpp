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

    const VnlcTypeDeclarationNode* localNode;
    const VnlcImportedIdentifier* importedNode;

public:
    VnlcCustomizedType(VnlcCustomizedTypeKind customizedKind, std::string_view fullTypeName, const VnlcTypeDeclarationNode* localNode);
    VnlcCustomizedType(VnlcCustomizedTypeKind customizedKind, std::string_view fullTypeName, const VnlcImportedIdentifier* importedNode);

    [[nodiscard]] std::string_view getFullTypeName() const noexcept override;
    [[nodiscard]] VnlcCustomizedTypeKind getCustomizedKind() const noexcept;
    [[nodiscard]] VnlcCustomizedTypeOrigin getOrigin() const noexcept;

    [[nodiscard]] const VnlcTypeDeclarationNode* getLocalNode() const noexcept;
    [[nodiscard]] const VnlcImportedIdentifier* getImportedNode() const noexcept;
};

#endif // VNLC_CUSTOMIZED_TYPE_HPP