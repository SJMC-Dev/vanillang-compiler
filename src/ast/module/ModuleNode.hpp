#ifndef VNLC_MODULE_NODE_HPP
#define VNLC_MODULE_NODE_HPP

#include "ast/AstNode.hpp"
#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/ExportDeclarationNode.hpp"
#include "ast/declaration/ImportDeclarationNode.hpp"
#include <memory>
#include <vector>

namespace vnlc {
    class ModuleNode : public AstNode {
    private:
        ModuleNode() = delete;

        std::string name;
        std::string fullName;

        std::vector<std::unique_ptr<ImportDeclarationNode>> importDeclarations;
        std::vector<std::unique_ptr<DeclarationNode>> topIdentifierDeclarations;
        std::vector<std::unique_ptr<ExportDeclarationNode>> exportDeclarations;

    public:
        ModuleNode(
            std::string&& name,
            std::string&& fullName,
            std::vector<std::unique_ptr<ImportDeclarationNode>>&& importDeclarations,
            std::vector<std::unique_ptr<DeclarationNode>>&& topIdentifierDeclarations,
            std::vector<std::unique_ptr<ExportDeclarationNode>>&& exportDeclarations,
            const Token& firstToken,
            const Token& lastToken
        ) noexcept;

        [[nodiscard]] std::string_view getName() const noexcept;
        [[nodiscard]] std::string_view getFullName() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ImportDeclarationNode>>& getImportDeclarations() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<DeclarationNode>>& getTopIdentifierDeclarations() const noexcept;
        [[nodiscard]] const std::vector<std::unique_ptr<ExportDeclarationNode>>& getExportDeclarations() const noexcept;
    };
} // namespace vnlc

#endif // VNLC_MODULE_NODE_HPP