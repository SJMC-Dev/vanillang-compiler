#ifndef VNLC_MODULE_NODE_HPP
#define VNLC_MODULE_NODE_HPP

#include "ast/VnlcAstNode.hpp"
#include "ast/declaration/VnlcDeclarationNode.hpp"
#include "ast/declaration/VnlcExportDeclarationNode.hpp"
#include "ast/declaration/VnlcImportDeclarationNode.hpp"
#include <memory>
#include <vector>

class VnlcModuleNode : public VnlcAstNode {
private:
    VnlcModuleNode() = delete;

    std::string name;
    std::string fullName;

    std::vector<std::unique_ptr<VnlcImportDeclarationNode>> importDeclarations;
    std::vector<std::unique_ptr<VnlcDeclarationNode>> topIdentifierDeclarations;
    std::vector<std::unique_ptr<VnlcExportDeclarationNode>> exportDeclarations;

public:
    VnlcModuleNode(
        std::string&& name,
        std::string&& fullName,
        std::vector<std::unique_ptr<VnlcImportDeclarationNode>>&& importDeclarations,
        std::vector<std::unique_ptr<VnlcDeclarationNode>>&& topIdentifierDeclarations,
        std::vector<std::unique_ptr<VnlcExportDeclarationNode>>&& exportDeclarations,
        const VnlcToken& firstToken,
        const VnlcToken& lastToken
    ) noexcept;

    [[nodiscard]] std::string_view getName() const noexcept;
    [[nodiscard]] std::string_view getFullName() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcImportDeclarationNode>>& getImportDeclarations() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcDeclarationNode>>& getTopIdentifierDeclarations() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<VnlcExportDeclarationNode>>& getExportDeclarations() const noexcept;
};

#endif // VNLC_MODULE_NODE_HPP