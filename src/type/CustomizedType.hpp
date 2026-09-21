#ifndef VNLC_CUSTOMIZED_TYPE_HPP
#define VNLC_CUSTOMIZED_TYPE_HPP

#include "ast/declaration/TypeDeclarationNode.hpp"
#include "type/CustomizedTypeKind.hpp"
#include "type/CustomizedTypeOrigin.hpp"
#include "type/SemanticType.hpp"
#include "vni/import/ImportedIdentifier.hpp"
#include <string>
#include <string_view>
#include <vector>

namespace vnlc {
    class CustomizedType : public SemanticType {
    private:
        CustomizedTypeKind customizedKind;
        CustomizedTypeOrigin origin;
        std::string fullTypeName;
        std::vector<const SemanticType*> genericArguments;
        bool optional;

        const TypeDeclarationNode* localNode;
        const ImportedIdentifier* importedNode;

    public:
        CustomizedType(
            CustomizedTypeKind customizedKind,
            std::string_view fullTypeName,
            std::vector<const SemanticType*>&& genericArguments,
            bool optional,
            const TypeDeclarationNode* localNode
        );
        CustomizedType(
            CustomizedTypeKind customizedKind,
            std::string_view fullTypeName,
            std::vector<const SemanticType*>&& genericArguments,
            bool optional,
            const ImportedIdentifier* importedNode
        );

        [[nodiscard]] std::string_view getFullTypeName() const noexcept override;
        [[nodiscard]] const std::vector<const SemanticType*> getGenericArguments() const noexcept;
        [[nodiscard]] bool isOptional() const noexcept;
        [[nodiscard]] CustomizedTypeKind getCustomizedKind() const noexcept;
        [[nodiscard]] CustomizedTypeOrigin getOrigin() const noexcept;

        [[nodiscard]] const TypeDeclarationNode* getLocalNode() const noexcept;
        [[nodiscard]] const ImportedIdentifier* getImportedNode() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_CUSTOMIZED_TYPE_HPP