#include "Collector.hpp"
#include "error/IllegalModuleOrPackageNameError.hpp"
#include "error/ModuleInterfaceReaderError.hpp"
#include "error/PackageReaderError.hpp"
#include "error/SyntaxError.hpp"
#include "vni/import/ImportedAlias.hpp"
#include "vni/import/ImportedClass.hpp"
#include "vni/import/ImportedEnum.hpp"
#include "vni/import/ImportedEnumMember.hpp"
#include "vni/import/ImportedEnumValue.hpp"
#include "vni/import/ImportedFunc.hpp"
#include "vni/import/ImportedInterface.hpp"
#include "vni/import/ImportedLet.hpp"
#include "vni/import/ImportedMethod.hpp"
#include "vni/import/ImportedModule.hpp"
#include "vni/import/ImportedParameter.hpp"
#include "vni/import/ImportedProperty.hpp"
#include "vni/import/ImportedTypeAlias.hpp"
#include "vni/import/PackageReader.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string_view>
#include <utility>

namespace vnlc {
    Collector::Collector(Lexer&& lexer, std::size_t maxBufferSize) : lexer(std::move(lexer)), tokenBuffer(), currentTokenIndex(0), bufferSize(maxBufferSize), endsWithNewlineOrEOF(false) {
        fillBuffer();
    }

    bool Collector::hasNextToken() const {
        return lexer.hasNext() || currentTokenIndex < bufferSize;
    }

    const Token& Collector::peek() const {
        return tokenBuffer[currentTokenIndex];
    }

    void Collector::fillBuffer() {
        bool blank = false;
        tokenBuffer.clear();

        for (std::size_t index = 0; index < bufferSize && lexer.hasNext(); index = blank ? index : index + 1) {
            Token token = lexer.next();
            if (token.getKind() == TokenKind::BLANK || token.getKind() == TokenKind::SINGLE_LINE_COMMENT || token.getKind() == TokenKind::MULTI_LINE_COMMENT) {
                blank = true;
            } else {
                blank = false;
                tokenBuffer.push_back(std::move(token));
            }
        }

        bufferSize = tokenBuffer.size();
    }

    void Collector::advanceRaw() {
        if (peek().getKind() == TokenKind::END_OF_FILE) {
            currentTokenIndex = bufferSize;
            return;
        }

        if (currentTokenIndex < bufferSize) {
            currentTokenIndex++;
            if (currentTokenIndex == bufferSize && lexer.hasNext()) {
                fillBuffer();
                currentTokenIndex = 0;
            }
        }
    }

    void Collector::advance() {
        advanceRaw();
        skipNewlines();
    }

    void Collector::skipNewlines() {
        bool skipped = false;

        while (hasNextToken() && peek().getKind() == TokenKind::NEWLINE) {
            advanceRaw();
            skipped = true;
        }

        endsWithNewlineOrEOF = skipped || peek().getKind() == TokenKind::END_OF_FILE;
    }

    bool Collector::check(TokenKind expectedKind) {
        return hasNextToken() && peek().getKind() == expectedKind;
    }

    bool Collector::match(TokenKind expectedKind) {
        if (check(expectedKind)) {
            advance();
            return true;
        }
        return false;
    }

    std::string Collector::consumeIdentifier() {
        if (!check(TokenKind::IDENTIFIER)) {
            throw SyntaxError("Expected identifier", peek().getLine(), peek().getColumn());
        }

        std::string name(peek().getValue());
        advance();
        return name;
    }

    void Collector::collectImport(const Config& config, std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& imports, std::vector<Diagnostic>& errors) {
        Token importToken = peek();
        if (!match(TokenKind::IMPORT)) {
            throw SyntaxError("Expected 'import' keyword", peek().getLine(), peek().getColumn());
        }

        std::vector<std::vector<std::string>> paths;
        collectImportPath({}, false, paths);

        if (!endsWithNewlineOrEOF) {
            throw SyntaxError("Expected newline after import declaration", peek().getLine(), peek().getColumn());
        }

        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> stagedImports;
        PackageReader reader(stagedImports);

        try {
            for (const auto& path : paths) {
                reader.readPackageFromPath(joinPath(path), config);
            }

            std::unordered_set<const ImportedModule*> visitedModules;
            std::unordered_set<std::string> loadedPaths;
            while (true) {
                std::unordered_set<std::string> dependencies;
                for (const auto& [name, package] : stagedImports) {
                    collectImportDependencies(*package, dependencies, visitedModules);
                }
                if (dependencies.empty()) {
                    break;
                }
                for (const auto& dependency : dependencies) {
                    if (loadedPaths.insert(dependency).second) {
                        reader.readPackageFromPath(dependency, config);
                    }
                }
            }
        } catch (const PackageReaderError& error) {
            errors.push_back(makeError(importToken, error.what()));
            return;
        } catch (const ModuleInterfaceFileReaderError& error) {
            errors.push_back(makeError(importToken, error.what()));
            return;
        } catch (const std::filesystem::filesystem_error& error) {
            errors.push_back(makeError(importToken, error.what()));
            return;
        } catch (const nlohmann::json::exception& error) {
            errors.push_back(makeError(importToken, error.what()));
            return;
        }

        mergeImports(imports, std::move(stagedImports));
    }

    void Collector::collectImportPath(std::vector<std::string> prefix, bool allowSelf, std::vector<std::vector<std::string>>& paths) {
        if (allowSelf && match(TokenKind::SELF)) {
            if (match(TokenKind::AS)) {
                if (!endsWithNewlineOrEOF && check(TokenKind::IDENTIFIER)) {
                    advance();
                }
            }
            paths.push_back(std::move(prefix));
            return;
        }

        if (allowSelf && match(TokenKind::ASTERISK)) {
            paths.push_back(std::move(prefix));
            return;
        }

        while (true) {
            prefix.emplace_back(consumeIdentifier());

            if (!match(TokenKind::DOT)) {
                break;
            }

            if (match(TokenKind::ASTERISK)) {
                paths.push_back(std::move(prefix));
                return;
            }

            if (match(TokenKind::LEFT_BRACE)) {
                collectImportPathList(std::move(prefix), paths);
                if (!match(TokenKind::RIGHT_BRACE)) {
                    throw SyntaxError("Expected '}' after import path list", peek().getLine(), peek().getColumn());
                }
                return;
            }
        }

        if (match(TokenKind::AS)) {
            if (!endsWithNewlineOrEOF && check(TokenKind::IDENTIFIER)) {
                advance();
            }
        }

        paths.push_back(std::move(prefix));
    }

    void Collector::collectImportPathList(std::vector<std::string> prefix, std::vector<std::vector<std::string>>& paths) {
        do {
            collectImportPath(prefix, true, paths);
        } while (match(TokenKind::COMMA));
    }

    std::unique_ptr<TypeDeclarationOutline> Collector::collectTypeDeclaration() {
        if (check(TokenKind::FINAL) || check(TokenKind::CLASS)) {
            return collectClassDeclaration();
        }
        if (check(TokenKind::INTERFACE)) {
            return collectInterfaceDeclaration();
        }
        if (check(TokenKind::ENUM)) {
            return collectEnumDeclaration();
        }
        if (check(TokenKind::TYPE)) {
            return collectTypeAliasDeclaration();
        }

        throw SyntaxError("Expected type declaration", peek().getLine(), peek().getColumn());
    }

    std::unique_ptr<ClassDeclarationOutline> Collector::collectClassDeclaration() {
        Token firstToken = peek();

        if (check(TokenKind::FINAL)) {
            advance();
        }
        if (!match(TokenKind::CLASS)) {
            throw SyntaxError("Expected 'class' keyword", peek().getLine(), peek().getColumn());
        }

        std::string name = consumeIdentifier();

        if (match(TokenKind::LEFT_ANGLE)) {
            skipBalanced(TokenKind::LEFT_ANGLE, TokenKind::RIGHT_ANGLE);
        }

        skipUntilLeftBrace();
        if (!match(TokenKind::LEFT_BRACE)) {
            throw SyntaxError("Expected '{' at the beginning of class body", peek().getLine(), peek().getColumn());
        }
        skipBalanced(TokenKind::LEFT_BRACE, TokenKind::RIGHT_BRACE);

        return std::make_unique<ClassDeclarationOutline>(std::move(name), firstToken, peek());
    }

    std::unique_ptr<InterfaceDeclarationOutline> Collector::collectInterfaceDeclaration() {
        Token firstToken = peek();

        if (!match(TokenKind::INTERFACE)) {
            throw SyntaxError("Expected 'interface' keyword", peek().getLine(), peek().getColumn());
        }

        std::string name = consumeIdentifier();

        if (match(TokenKind::LEFT_ANGLE)) {
            skipBalanced(TokenKind::LEFT_ANGLE, TokenKind::RIGHT_ANGLE);
        }

        skipUntilLeftBrace();
        if (!match(TokenKind::LEFT_BRACE)) {
            throw SyntaxError("Expected '{' at the beginning of interface body", peek().getLine(), peek().getColumn());
        }
        skipBalanced(TokenKind::LEFT_BRACE, TokenKind::RIGHT_BRACE);

        return std::make_unique<InterfaceDeclarationOutline>(std::move(name), firstToken, peek());
    }

    std::unique_ptr<EnumDeclarationOutline> Collector::collectEnumDeclaration() {
        Token firstToken = peek();

        if (!match(TokenKind::ENUM)) {
            throw SyntaxError("Expected 'enum' keyword", peek().getLine(), peek().getColumn());
        }

        std::string name = consumeIdentifier();

        if (match(TokenKind::LEFT_ANGLE)) {
            skipBalanced(TokenKind::LEFT_ANGLE, TokenKind::RIGHT_ANGLE);
        }

        if (!match(TokenKind::LEFT_BRACE)) {
            throw SyntaxError("Expected '{' at the beginning of enum body", peek().getLine(), peek().getColumn());
        }

        std::vector<std::unique_ptr<EnumMemberDeclarationOutline>> members;

        while (!check(TokenKind::RIGHT_BRACE)) {
            skipNewlines();

            if (check(TokenKind::METADATA)) {
                skipMetadata();
                skipNewlines();
            }

            Token memberFirstToken = peek();
            std::string memberName = consumeIdentifier();

            if (match(TokenKind::LEFT_PARENTHESIS)) {
                skipBalanced(TokenKind::LEFT_PARENTHESIS, TokenKind::RIGHT_PARENTHESIS);
            }

            members.emplace_back(std::make_unique<EnumMemberDeclarationOutline>(std::move(memberName), memberFirstToken, peek()));

            if (!(endsWithNewlineOrEOF || check(TokenKind::RIGHT_BRACE))) {
                throw SyntaxError("Expected newline after enum member declaration", peek().getLine(), peek().getColumn());
            }
        }

        if (!match(TokenKind::RIGHT_BRACE)) {
            throw SyntaxError("Expected '}' at the end of enum body", peek().getLine(), peek().getColumn());
        }

        return std::make_unique<EnumDeclarationOutline>(std::move(name), std::move(members), firstToken, peek());
    }

    std::unique_ptr<TypeAliasDeclarationOutline> Collector::collectTypeAliasDeclaration() {
        Token firstToken = peek();

        if (!match(TokenKind::TYPE)) {
            throw SyntaxError("Expected 'type' keyword", peek().getLine(), peek().getColumn());
        }

        std::string aliasName = consumeIdentifier();

        if (match(TokenKind::LEFT_ANGLE)) {
            skipBalanced(TokenKind::LEFT_ANGLE, TokenKind::RIGHT_ANGLE);
        }

        if (!match(TokenKind::EQUAL)) {
            throw SyntaxError("Expected '=' after type alias name", peek().getLine(), peek().getColumn());
        }

        return std::make_unique<TypeAliasDeclarationOutline>(std::move(aliasName), firstToken, peek());
    }

    void Collector::skipBalanced(TokenKind leftKind, TokenKind rightKind) {
        std::size_t depth = 1;

        while (depth > 0) {
            if (check(TokenKind::END_OF_FILE)) {
                throw SyntaxError("Unexpected end of file", peek().getLine(), peek().getColumn());
            }

            if (match(leftKind)) {
                depth++;
            } else if (match(rightKind)) {
                depth--;
            } else {
                advance();
            }
        }
    }

    void Collector::skipMetadata() {
        if (!match(TokenKind::METADATA) || !match(TokenKind::LEFT_PARENTHESIS)) {
            throw SyntaxError("Expected metadata declaration", peek().getLine(), peek().getColumn());
        }

        skipBalanced(TokenKind::LEFT_PARENTHESIS, TokenKind::RIGHT_PARENTHESIS);
    }

    void Collector::skipUntilLeftBrace() {
        while (!check(TokenKind::LEFT_BRACE)) {
            if (check(TokenKind::END_OF_FILE)) {
                throw SyntaxError("Expected '{'", peek().getLine(), peek().getColumn());
            }
            advance();
        }
    }

    std::string Collector::joinPath(const std::vector<std::string>& path) {
        std::string result;
        for (const auto& part : path) {
            if (!result.empty()) {
                result.push_back('.');
            }
            result.append(part);
        }
        return result;
    }

    Diagnostic Collector::makeError(const Token& token, std::string_view message) {
        return Diagnostic(DiagnosticPhase::COLLECTOR, DiagnosticSeverity::ERROR, std::string(message), token.getLine(), token.getColumn(), token.getOffset(), token.getLength());
    }

    void Collector::mergeImports(std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>& target, std::unordered_map<std::string, std::unique_ptr<ImportedPackage>>&& source) {
        for (auto& [name, package] : source) {
            auto existing = target.find(name);
            if (existing == target.end()) {
                target.emplace(name, std::move(package));
            } else {
                existing->second->merge(std::move(*package));
            }
        }
    }

    void Collector::collectTypeDependencies(std::string_view type, std::unordered_set<std::string>& dependencies) {
        constexpr std::string_view delimiters = "<>,? \t\r\n";
        auto begin = type.find_first_not_of(delimiters);
        while (begin != std::string_view::npos) {
            auto end = type.find_first_of(delimiters, begin);
            const auto name = type.substr(begin, end == std::string_view::npos ? type.size() - begin : end - begin);
            if (name.find('.') != std::string_view::npos) {
                dependencies.emplace(name);
            }
            begin = end == std::string_view::npos ? end : type.find_first_not_of(delimiters, end);
        }
    }

    void Collector::collectImportDependencies(const ImportedItem& item, std::unordered_set<std::string>& dependencies, std::unordered_set<const ImportedModule*>& visitedModules) {
        const auto collectChildren = [&](const auto& children) {
            for (const auto& [name, child] : children) {
                collectImportDependencies(*child, dependencies, visitedModules);
            }
        };

        if (const auto* package = dynamic_cast<const ImportedPackage*>(&item)) {
            collectChildren(package->getSubPackages());
            collectChildren(package->getModules());
        } else if (const auto* module = dynamic_cast<const ImportedModule*>(&item)) {
            if (visitedModules.insert(module).second) {
                collectChildren(module->getIdentifiers());
            }
        } else if (const auto* alias = dynamic_cast<const ImportedAlias*>(&item)) {
            dependencies.emplace(alias->getSource());
        } else if (const auto* variable = dynamic_cast<const ImportedLet*>(&item)) {
            collectTypeDependencies(variable->getType(), dependencies);
        } else if (const auto* property = dynamic_cast<const ImportedProperty*>(&item)) {
            collectTypeDependencies(property->getType(), dependencies);
        } else if (const auto* parameter = dynamic_cast<const ImportedParameter*>(&item)) {
            collectTypeDependencies(parameter->getType(), dependencies);
        } else if (const auto* enumValue = dynamic_cast<const ImportedEnumValue*>(&item)) {
            collectTypeDependencies(enumValue->getType(), dependencies);
        } else if (const auto* function = dynamic_cast<const ImportedFunc*>(&item)) {
            collectTypeDependencies(function->getReturnType(), dependencies);
            collectChildren(function->getParameters());
        } else if (const auto* method = dynamic_cast<const ImportedMethod*>(&item)) {
            collectTypeDependencies(method->getReturnType(), dependencies);
            collectChildren(method->getParameters());
        } else if (const auto* classType = dynamic_cast<const ImportedClass*>(&item)) {
            if (classType->getBaseClass().has_value()) {
                collectTypeDependencies(classType->getBaseClass().value(), dependencies);
            }
            for (const auto& interfaceType : classType->getImplementedInterfaces()) {
                collectTypeDependencies(interfaceType, dependencies);
            }
            collectChildren(classType->getProperties());
            collectChildren(classType->getMethods());
        } else if (const auto* interfaceType = dynamic_cast<const ImportedInterface*>(&item)) {
            collectChildren(interfaceType->getMethods());
        } else if (const auto* enumType = dynamic_cast<const ImportedEnum*>(&item)) {
            collectChildren(enumType->getMembers());
        } else if (const auto* enumMember = dynamic_cast<const ImportedEnumMember*>(&item)) {
            collectChildren(enumMember->getAssociatedValues());
        } else if (const auto* typeAlias = dynamic_cast<const ImportedTypeAlias*>(&item)) {
            collectTypeDependencies(typeAlias->getOriginalType(), dependencies);
        }
    }

    CollectionResult Collector::collectModule(const Config& config) {
        std::string name;
        std::string fullName;

        std::string prefix = config.packageRootPath.parent_path().string();
        std::string fullPath = config.inputFilePath.string();

        fullPath.erase(0, prefix.length());
        if (fullPath.starts_with(std::filesystem::path::preferred_separator)) {
            fullPath.erase(0, 1);
        }

        if (fullPath.ends_with(".vnl")) {
            fullPath.erase(fullPath.length() - 4);
        }

        std::stringstream pathStream(fullPath);
        std::string namePart;
        while (std::getline(pathStream, namePart, static_cast<char>(std::filesystem::path::preferred_separator))) {
            if (namePart.empty()) {
                throw IllegalModuleOrPackageNameError("Module name contains empty part");
            }

            std::stringstream namePartStream(namePart);
            Lexer namePartLexer(namePartStream);
            if (!(namePartLexer.hasNext() && namePartLexer.next().getKind() == TokenKind::IDENTIFIER)) {
                throw IllegalModuleOrPackageNameError(namePart);
            } else if (!(namePartLexer.hasNext() && namePartLexer.next().getKind() == TokenKind::END_OF_FILE)) {
                throw IllegalModuleOrPackageNameError(namePart);
            }

            fullName += namePart + ".";
        }

        if (fullName.ends_with(".")) {
            fullName.pop_back();
        }
        name = fullName.substr(fullName.find_last_of('.') + 1);

        Token firstToken = peek();

        skipNewlines();

        std::unordered_map<std::string, std::unique_ptr<ImportedPackage>> imports;
        std::vector<Diagnostic> errors;

        while (check(TokenKind::IMPORT)) {
            collectImport(config, imports, errors);
        }

        std::vector<std::unique_ptr<TypeDeclarationOutline>> typeDeclarations;
        std::size_t braceDepth = 0;

        while (!check(TokenKind::END_OF_FILE)) {
            if (braceDepth == 0 && check(TokenKind::EXPORT)) {
                break;
            }

            if (braceDepth == 0 && (check(TokenKind::FINAL) || check(TokenKind::CLASS) || check(TokenKind::INTERFACE) || check(TokenKind::ENUM) || check(TokenKind::TYPE))) {
                typeDeclarations.push_back(collectTypeDeclaration());
                continue;
            }

            if (check(TokenKind::LEFT_BRACE)) {
                braceDepth++;
            } else if (check(TokenKind::RIGHT_BRACE) && braceDepth > 0) {
                braceDepth--;
            }

            advance();
        }

        auto moduleOutline = std::make_unique<ModuleOutline>(std::move(name), std::move(fullName), std::move(typeDeclarations), firstToken, peek());
        return CollectionResult(std::move(moduleOutline), std::move(imports), std::move(errors));
    }

    CollectionResult Collector::collect(const Config& config) {
        return collectModule(config);
    }
} // namespace vnlc
