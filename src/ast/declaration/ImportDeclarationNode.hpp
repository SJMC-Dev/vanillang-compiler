#ifndef VNLC_IMPORT_DECLARATION_NODE_HPP
#define VNLC_IMPORT_DECLARATION_NODE_HPP

#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/ImportDeclarationItem.hpp"

namespace vnlc {
    class ImportDeclarationNode : public DeclarationNode {
    private:
        ImportDeclarationNode() = delete;

        std::unique_ptr<ImportDeclarationItem> paths;

    public:
        ImportDeclarationNode(std::unique_ptr<ImportDeclarationItem>&& paths, const Token& firstToken, const Token& lastToken) noexcept;

        [[nodiscard]] const ImportDeclarationItem& getNamePartsListWithAliases() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_IMPORT_DECLARATION_NODE_HPP