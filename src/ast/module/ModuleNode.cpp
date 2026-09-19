#include "ModuleNode.hpp"

namespace vnlc {
    ModuleNode::ModuleNode(
        std::string&& name,
        std::string&& fullName,
        std::vector<std::unique_ptr<ImportDeclarationNode>>&& importDeclarations,
        std::vector<std::unique_ptr<DeclarationNode>>&& topIdentifierDeclarations,
        std::vector<std::unique_ptr<ExportDeclarationNode>>&& exportDeclarations,
        const Token& firstToken,
        const Token& lastToken
    ) noexcept
        : AstNode(firstToken, lastToken),
          name(std::move(name)),
          fullName(std::move(fullName)),
          importDeclarations(std::move(importDeclarations)),
          topIdentifierDeclarations(std::move(topIdentifierDeclarations)),
          exportDeclarations(std::move(exportDeclarations)) {}

    std::string_view ModuleNode::getName() const noexcept {
        return name;
    }

    std::string_view ModuleNode::getFullName() const noexcept {
        return fullName;
    }

    const std::vector<std::unique_ptr<ImportDeclarationNode>>& ModuleNode::getImportDeclarations() const noexcept {
        return importDeclarations;
    }

    const std::vector<std::unique_ptr<DeclarationNode>>& ModuleNode::getTopIdentifierDeclarations() const noexcept {
        return topIdentifierDeclarations;
    }

    const std::vector<std::unique_ptr<ExportDeclarationNode>>& ModuleNode::getExportDeclarations() const noexcept {
        return exportDeclarations;
    }
} // namespace vnlc
