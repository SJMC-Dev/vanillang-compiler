#include "Parser.hpp"
#include "ast/declaration/DeclarationNode.hpp"
#include "ast/declaration/ExportDeclarationNode.hpp"
#include "ast/declaration/ImportDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationNode.hpp"
#include "ast/declaration/ValueDeclarationType.hpp"
#include "ast/expression/BinaryExpressionNode.hpp"
#include "ast/expression/BinaryExpressionType.hpp"
#include "ast/expression/ConditionalExpressionNode.hpp"
#include "ast/expression/DictLiteralExpressionNode.hpp"
#include "ast/expression/FunctionCallExpressionNode.hpp"
#include "ast/expression/ListLikeLiteralExpressionNode.hpp"
#include "ast/expression/ListLikeLiteralExpressionType.hpp"
#include "ast/expression/MemberAccessExpressionNode.hpp"
#include "ast/expression/MemberAccessExpressionType.hpp"
#include "ast/expression/RangeExpressionNode.hpp"
#include "ast/expression/SelectorLiteralExpressionNode.hpp"
#include "ast/expression/SelectorLiteralExpressionType.hpp"
#include "ast/expression/SimpleLiteralExpressionNode.hpp"
#include "ast/expression/StringLiteralExpressionNode.hpp"
#include "ast/expression/StringLiteralExpressionType.hpp"
#include "ast/expression/SubscriptExpressionNode.hpp"
#include "ast/expression/SuperExpressionNode.hpp"
#include "ast/expression/ThisExpressionNode.hpp"
#include "ast/expression/UnaryExpressionNode.hpp"
#include "ast/expression/UnaryExpressionType.hpp"
#include "ast/module/ModuleNode.hpp"
#include "ast/statement/SwitchStatementItem.hpp"
#include "ast/statement/SwitchStatementNode.hpp"
#include "ast/statement/SwitchStatementType.hpp"
#include "error/IllegalModuleOrPackageNameError.hpp"
#include "error/OutOfRangeError.hpp"
#include "error/SyntaxError.hpp"
#include "util/TokenTypeUtil.hpp"
#include <memory>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace vnlc {
    Parser::Parser(Lexer&& lexer, unsigned int maxBufferSize) : lexer(std::move(lexer)), tokenBuffer(), currentTokenIndex(0), bufferSize(maxBufferSize) {
        fillBuffer();
    }

    bool Parser::hasNextToken() const {
        return lexer.hasNext() || currentTokenIndex < bufferSize;
    }

    const Token& Parser::peek(unsigned int offset) const {
        if (currentTokenIndex + offset < bufferSize) {
            return tokenBuffer[currentTokenIndex + offset];
        } else {
            throw OutOfRangeError("Peek offset exceeds buffer size");
        }
    }

    const Token& Parser::peek() const {
        return peek(0);
    }

    void Parser::fillBuffer() {
        bool blank = false;
        tokenBuffer.clear();

        for (unsigned int i = 0; i < bufferSize && lexer.hasNext(); i = blank ? i : i + 1) {
            Token token = lexer.next();
            if (token.getType() == TokenType::BLANK || token.getType() == TokenType::SINGLE_LINE_COMMENT || token.getType() == TokenType::MULTI_LINE_COMMENT) {
                blank = true;
            } else {
                blank = false;
                tokenBuffer.push_back(std::move(token));
            }
        }

        bufferSize = tokenBuffer.size();
    }

    void Parser::advanceRaw() {
        if (peek().getType() == TokenType::END_OF_FILE) {
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

    void Parser::advance() {
        advanceRaw();
        skipNewlines();
    }

    void Parser::skipNewlines() {
        bool skipped = false;

        while (hasNextToken() && peek().getType() == TokenType::NEWLINE) {
            advanceRaw();
            skipped = true;
        }

        endsWithNewlineOrEOF = skipped || peek().getType() == TokenType::END_OF_FILE;
    }

    bool Parser::check(TokenType expectedType) {
        return hasNextToken() && peek().getType() == expectedType;
    }

    bool Parser::checkGeneralizedIdentifier() {
        return hasNextToken() && TokenTypeUtil::isGeneralizedIdentifier(peek().getType());
    }

    bool Parser::checkAny(const std::unordered_set<TokenType>& expectedTypes) {
        return hasNextToken() && expectedTypes.contains(peek().getType());
    }

    bool Parser::match(TokenType expectedType) {
        if (check(expectedType)) {
            advance();
            return true;
        }
        return false;
    }

    bool Parser::matchAny(const std::unordered_set<TokenType>& expectedTypes) {
        if (checkAny(expectedTypes)) {
            advance();
            return true;
        }
        return false;
    }

    bool Parser::consumeRightAngleInType() {
        if (match(TokenType::RIGHT_ANGLE)) {
            return true;
        }

        if (check(TokenType::DOUBLE_RIGHT_ANGLE)) {
            tokenBuffer[currentTokenIndex] = Token(TokenType::RIGHT_ANGLE, ">", peek().getLine(), peek().getColumn() + 1, peek().getOffset() + 1);
            return true;
        }

        if (check(TokenType::TRIPLE_RIGHT_ANGLE)) {
            tokenBuffer[currentTokenIndex] = Token(TokenType::DOUBLE_RIGHT_ANGLE, ">>", peek().getLine(), peek().getColumn() + 1, peek().getOffset() + 1);
            return true;
        }

        return false;
    }

    std::unique_ptr<IdentifierNode> Parser::constructCurrentIdentifierNode() {
        Token firstToken = peek();
        std::string nameValue = std::string(peek().getValue());
        advance();
        Token lastToken = peek();

        return std::make_unique<IdentifierNode>(std::move(nameValue), firstToken, lastToken);
    }

    std::string Parser::generateNamespaceIdFromTypeName(const TypeNode& typeNode) {
        std::string name;

        for (auto& part : typeNode.getNameParts()) {
            std::string partName = std::string(part->getIdentifierString());
            name += partName + ".";
        }

        if (name.ends_with(".")) {
            name.pop_back();
        }

        if (!typeNode.getGenericArguments().empty()) {
            name += ".-";

            for (auto& generic : typeNode.getGenericArguments()) {
                name += generateNamespaceIdFromTypeName(*generic);
                name += "-";
            }

            if (name.ends_with("-")) {
                name.pop_back();
            }

            name += "-.";
        }

        return name;
    }

    std::unique_ptr<ModuleNode> Parser::parse(const Config& config) {
        ModuleParsingContext context(config);
        auto result = parseModule(context);

        return std::move(result.moduleNode);
    }

    ModuleParsingResult Parser::parseModule(ModuleParsingContext context) {
        std::vector<std::unique_ptr<ImportDeclarationNode>> importDeclarations;
        std::vector<std::unique_ptr<DeclarationNode>> declarations;
        std::vector<std::unique_ptr<ExportDeclarationNode>> exportDeclarations;
        std::string name;
        std::string fullName;

        std::string prefix = context.config.packageRootPath.parent_path().string();
        std::string fullPath = context.config.inputFilePath.string();

        fullPath.erase(0, prefix.length());
        if (fullPath.starts_with(std::filesystem::path::preferred_separator)) {
            fullPath.erase(0, 1);
        }

        if (fullPath.ends_with(".vnl")) {
            fullPath.erase(fullPath.length() - 4);
        }

        std::stringstream ss(fullPath);
        std::string namePart;
        while (std::getline(ss, namePart, static_cast<char>(std::filesystem::path::preferred_separator))) {
            if (namePart.empty()) {
                throw IllegalModuleOrPackageNameError("Module name contains empty part");
            }

            std::stringstream namePartStream(namePart);
            Lexer namePartLexer(namePartStream);
            if (!(namePartLexer.hasNext() && namePartLexer.next().getType() == TokenType::IDENTIFIER)) {
                throw IllegalModuleOrPackageNameError(namePart);
            } else if (!(namePartLexer.hasNext() && namePartLexer.next().getType() == TokenType::END_OF_FILE)) {
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

        while (check(TokenType::IMPORT)) {
            auto result = parseImportDeclaration();
            importDeclarations.push_back(std::move(result.declaration));
        }

        while (!check(TokenType::EXPORT) && !check(TokenType::END_OF_FILE)) {
            auto result = parseTopIdentifierDeclaration();
            declarations.push_back(std::move(result.declaration));
        }

        while (check(TokenType::EXPORT)) {
            auto result = parseExportDeclaration();
            exportDeclarations.push_back(std::move(result.declaration));
        }

        if (!check(TokenType::END_OF_FILE)) {
            throw SyntaxError("Unexpected declaration after export declaration", peek().getLine(), peek().getColumn());
        }

        Token lastToken = peek();

        std::unique_ptr<ModuleNode> node =
            std::make_unique<ModuleNode>(std::move(name), std::move(fullName), std::move(importDeclarations), std::move(declarations), std::move(exportDeclarations), firstToken, lastToken);

        return ModuleParsingResult{
            .moduleNode = std::move(node),
        };
    }

    TopIdentifierDeclarationParsingResult Parser::parseTopIdentifierDeclaration() {
        bool hasMetadata = false;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;

        Token firstToken = peek();

        if (check(TokenType::METADATA)) {
            hasMetadata = true;

            auto result = parseMetadata();
            metadataTerms = std::move(result.metadata);
        }

        if (check(TokenType::VAR) || check(TokenType::LET) || check(TokenType::CONST)) {
            VariableDeclarationParsingContext context{
                .position = VariableDeclarationParsingContext::Position::TOP_LEVEL,
                .hasMetadata = hasMetadata,
                .metadataTerms = std::move(metadataTerms),
            };
            auto result = parseVariableDeclaration(std::move(context));

            Token lastToken = peek();
            result.declaration->resetPosition(firstToken, lastToken);

            if (!endsWithNewlineOrEOF) {
                throw SyntaxError("Expected newline after variable declaration", peek().getLine(), peek().getColumn());
            }

            return TopIdentifierDeclarationParsingResult{
                .declaration = std::move(result.declaration),
            };
        } else if (check(TokenType::FUNC) || check(TokenType::NATIVE)) {
            FunctionDeclarationParsingContext context{
                .context = FunctionDeclarationType::Context::TOP_LEVEL,
                .accessModifier = FunctionDeclarationType::AccessModifier::PUBLIC,
                .binding = FunctionDeclarationType::Binding::STATIC,
                .hasMetadata = hasMetadata,
                .metadataTerms = std::move(metadataTerms),
            };
            auto result = parseFunctionDeclaration(std::move(context));

            Token lastToken = peek();
            result.declaration->resetPosition(firstToken, lastToken);

            if (!endsWithNewlineOrEOF) {
                throw SyntaxError("Expected newline after function declaration", peek().getLine(), peek().getColumn());
            }

            return TopIdentifierDeclarationParsingResult{
                .declaration = std::move(result.declaration),
            };
        } else if (check(TokenType::CLASS) || check(TokenType::INTERFACE) || check(TokenType::ENUM) || check(TokenType::TYPE) || check(TokenType::FINAL)) {
            TypeDeclarationParsingContext context{
                .hasMetadata = hasMetadata,
                .metadataTerms = std::move(metadataTerms),
            };
            auto result = parseTypeDeclaration(std::move(context));

            Token lastToken = peek();
            result.declaration->resetPosition(firstToken, lastToken);

            if (!endsWithNewlineOrEOF) {
                throw SyntaxError("Expected newline after type declaration", peek().getLine(), peek().getColumn());
            }

            return TopIdentifierDeclarationParsingResult{
                .declaration = std::move(result.declaration),
            };
        } else {
            throw SyntaxError("Expected variable, function or type declaration", peek().getLine(), peek().getColumn());
        }
    }

    ImportDeclarationParsingResult Parser::parseImportDeclaration() {
        Token firstToken = peek();

        if (!match(TokenType::IMPORT)) {
            throw SyntaxError("Expected 'import' keyword", peek().getLine(), peek().getColumn());
        }

        auto result = parseImportPath();

        Token lastToken = peek();

        std::unique_ptr<ImportDeclarationNode> node = std::make_unique<ImportDeclarationNode>(std::move(result.paths), firstToken, lastToken);

        if (!endsWithNewlineOrEOF) {
            throw SyntaxError("Expected newline after import declaration", peek().getLine(), peek().getColumn());
        }

        return ImportDeclarationParsingResult{
            .declaration = std::move(node),
        };
    }

    ExportDeclarationParsingResult Parser::parseExportDeclaration() {
        Token firstToken = peek();

        if (!match(TokenType::EXPORT)) {
            throw SyntaxError("Expected 'export' keyword", peek().getLine(), peek().getColumn());
        }

        auto result = parseExportList();

        Token lastToken = peek();

        std::unique_ptr<ExportDeclarationNode> node = std::make_unique<ExportDeclarationNode>(std::move(result.items), firstToken, lastToken);

        if (!endsWithNewlineOrEOF) {
            throw SyntaxError("Expected newline after export declaration", peek().getLine(), peek().getColumn());
        }

        return ExportDeclarationParsingResult{
            .declaration = std::move(node),
        };
    }

    VariableDeclarationParsingResult Parser::parseVariableDeclaration(VariableDeclarationParsingContext context) {
        Token firstToken = peek();

        auto pos = context.position == VariableDeclarationParsingContext::Position::TOP_LEVEL ? ValueDeclarationType::Context::TOP_LEVEL : ValueDeclarationType::Context::BLOCK;

        VariableDeclarationPrimaryParsingContext primaryContext{
            .kind = ValueDeclarationType::Kind::LET,
        };
        auto primaryResult = parseVariableDeclarationPrimary(std::move(primaryContext));

        if (!match(TokenType::EQUAL)) {
            throw SyntaxError("Expected '=' after variable declaration", peek().getLine(), peek().getColumn());
        }

        auto initializerResult = parseExpression();

        Token lastToken = peek();

        std::unique_ptr<ValueDeclarationNode> node = nullptr;
        if (context.hasMetadata) {
            node = std::make_unique<ValueDeclarationNode>(
                primaryResult.kind,
                pos,
                ValueDeclarationType::AccessModifier::PUBLIC,
                std::move(primaryResult.name),
                std::move(primaryResult.type),
                std::move(initializerResult.expression),
                firstToken,
                lastToken,
                std::move(context.metadataTerms)
            );
        } else {
            node = std::make_unique<ValueDeclarationNode>(
                primaryResult.kind,
                pos,
                ValueDeclarationType::AccessModifier::PUBLIC,
                std::move(primaryResult.name),
                std::move(primaryResult.type),
                std::move(initializerResult.expression),
                firstToken,
                lastToken
            );
        };

        return VariableDeclarationParsingResult{
            .declaration = std::move(node),
        };
    }

    FunctionDeclarationParsingResult Parser::parseFunctionDeclaration(FunctionDeclarationParsingContext context) {
        if (check(TokenType::FUNC)) {
            RegularFunctionDeclarationParsingContext regularContext{
                .context = context.context,
                .hasMetadata = context.hasMetadata,
                .metadataTerms = std::move(context.metadataTerms),
            };
            auto result = parseRegularFunctionDeclaration(std::move(regularContext));

            return FunctionDeclarationParsingResult{
                .declaration = std::move(result.declaration),
            };
        } else if (check(TokenType::NATIVE)) {
            NativeFunctionDeclarationParsingContext nativeContext{
                .context = context.context,
                .hasMetadata = context.hasMetadata,
                .metadataTerms = std::move(context.metadataTerms),
            };
            auto result = parseNativeFunctionDeclaration(std::move(nativeContext));

            return FunctionDeclarationParsingResult{
                .declaration = std::move(result.declaration),
            };
        } else {
            throw SyntaxError("Expected 'func' or 'native' keyword", peek().getLine(), peek().getColumn());
        }
    }

    TypeDeclarationParsingResult Parser::parseTypeDeclaration(TypeDeclarationParsingContext context) {
        Token firstToken = peek();

        if (check(TokenType::CLASS) || check(TokenType::FINAL)) {
            ClassDeclarationParsingContext classContext{
                .hasMetadata = context.hasMetadata,
                .metadataTerms = std::move(context.metadataTerms),
            };

            auto result = parseClassDeclaration(std::move(classContext));

            Token lastToken = peek();

            return TypeDeclarationParsingResult{
                .declaration = std::move(result.declaration),
            };
        } else if (check(TokenType::INTERFACE)) {
            InterfaceDeclarationParsingContext interfaceContext{
                .hasMetadata = context.hasMetadata,
                .metadataTerms = std::move(context.metadataTerms),
            };

            auto result = parseInterfaceDeclaration(std::move(interfaceContext));

            Token lastToken = peek();

            return TypeDeclarationParsingResult{
                .declaration = std::move(result.declaration),
            };
        } else if (check(TokenType::ENUM)) {
            EnumDeclarationParsingContext enumContext{
                .hasMetadata = context.hasMetadata,
                .metadataTerms = std::move(context.metadataTerms),
            };

            auto result = parseEnumDeclaration(std::move(enumContext));

            Token lastToken = peek();

            return TypeDeclarationParsingResult{
                .declaration = std::move(result.declaration),
            };
        } else if (check(TokenType::TYPE)) {
            TypeAliasDeclarationParsingContext typeAliasContext{
                .hasMetadata = context.hasMetadata,
                .metadataTerms = std::move(context.metadataTerms),
            };

            auto result = parseTypeAliasDeclaration(std::move(typeAliasContext));

            Token lastToken = peek();

            return TypeDeclarationParsingResult{
                .declaration = std::move(result.declaration),
            };
        } else {
            throw SyntaxError("Expected 'class', 'interface', 'enum' or 'type' keyword", peek().getLine(), peek().getColumn());
        }
    }

    PropertyDeclarationParsingResult Parser::parsePropertyDeclaration(PropertyDeclarationParsingContext context) {
        Token firstToken = peek();

        std::unique_ptr<IdentifierNode> name;
        if (!check(TokenType::IDENTIFIER)) {
            throw SyntaxError("Expected property name", peek().getLine(), peek().getColumn());
        } else {
            name = constructCurrentIdentifierNode();
        }

        if (!match(TokenType::COLON)) {
            throw SyntaxError("Expected ':' after property name", peek().getLine(), peek().getColumn());
        }

        auto typeResult = parseType();

        if (match(TokenType::EQUAL)) {
            auto initializerResult = parseExpression();

            Token lastToken = peek();

            std::unique_ptr<PropertyDeclarationNode> node;

            if (context.hasMetadata) {
                node = std::make_unique<PropertyDeclarationNode>(
                    context.accessModifier,
                    context.binding,
                    std::move(name),
                    std::move(typeResult.type),
                    std::make_optional<std::unique_ptr<ExpressionNode>>(std::move(initializerResult.expression)),
                    firstToken,
                    lastToken,
                    std::move(context.metadataTerms)
                );
            } else {
                node = std::make_unique<PropertyDeclarationNode>(
                    context.accessModifier,
                    context.binding,
                    std::move(name),
                    std::move(typeResult.type),
                    std::make_optional<std::unique_ptr<ExpressionNode>>(std::move(initializerResult.expression)),
                    firstToken,
                    lastToken
                );
            }

            return PropertyDeclarationParsingResult{
                .declaration = std::move(node),
            };
        } else {
            Token lastToken = peek();

            std::unique_ptr<PropertyDeclarationNode> node;

            if (context.hasMetadata) {
                node = std::make_unique<PropertyDeclarationNode>(
                    context.accessModifier,
                    context.binding,
                    std::move(name),
                    std::move(typeResult.type),
                    std::nullopt,
                    firstToken,
                    lastToken,
                    std::move(context.metadataTerms)
                );
            } else {
                node = std::make_unique<PropertyDeclarationNode>(context.accessModifier, context.binding, std::move(name), std::move(typeResult.type), std::nullopt, firstToken, lastToken);
            }

            return PropertyDeclarationParsingResult{
                .declaration = std::move(node),
            };
        }
    }

    InterfaceMethodDeclarationParsingResult Parser::parseInterfaceMethodDeclaration() {
        Token firstToken = peek();

        bool hasMetadata = false;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
        if (check(TokenType::METADATA)) {
            hasMetadata = true;
            auto metadataResult = parseMetadata();
            metadataTerms = std::move(metadataResult.metadata);
        }

        auto result = parseFunctionSignature();

        Token lastToken = peek();

        if (!endsWithNewlineOrEOF) {
            throw SyntaxError("Expected newline after interface method declaration", peek().getLine(), peek().getColumn());
        }

        std::unique_ptr<FunctionDeclarationNode> node;
        if (hasMetadata) {
            node = std::make_unique<FunctionDeclarationNode>(
                FunctionDeclarationType::Kind::REGULAR,
                FunctionDeclarationType::Context::INTERFACE,
                FunctionDeclarationType::AccessModifier::PUBLIC,
                FunctionDeclarationType::Binding::INSTANCE,
                std::move(result.name),
                std::move(result.parameters),
                std::move(result.returnType),
                std::nullopt,
                firstToken,
                lastToken,
                std::move(metadataTerms)
            );
        } else {
            node = std::make_unique<FunctionDeclarationNode>(
                FunctionDeclarationType::Kind::REGULAR,
                FunctionDeclarationType::Context::INTERFACE,
                FunctionDeclarationType::AccessModifier::PUBLIC,
                FunctionDeclarationType::Binding::INSTANCE,
                std::move(result.name),
                std::move(result.parameters),
                std::move(result.returnType),
                std::nullopt,
                firstToken,
                lastToken
            );
        }

        return InterfaceMethodDeclarationParsingResult{
            .declaration = std::move(node),
        };
    }

    MetadataParsingResult Parser::parseMetadata() {
        if (!match(TokenType::METADATA)) {
            throw SyntaxError("Expected 'metadata' keyword", peek().getLine(), peek().getColumn());
        }

        if (!match(TokenType::LEFT_PARENTHESIS)) {
            throw SyntaxError("Expected '(' after 'metadata' keyword", peek().getLine(), peek().getColumn());
        }

        std::vector<DeclarationItem::MetadataTerm> metadataTerms;

        do {
            auto result = parseMetadataTerm();
            metadataTerms.push_back(std::move(result.term));

        } while (match(TokenType::COMMA));

        if (!match(TokenType::RIGHT_PARENTHESIS)) {
            throw SyntaxError("Expected ')' after metadata terms", peek().getLine(), peek().getColumn());
        }

        return MetadataParsingResult{
            .metadata = std::move(metadataTerms),
        };
    }

    VariableDeclarationPrimaryParsingResult Parser::parseVariableDeclarationPrimary(VariableDeclarationPrimaryParsingContext context) {
        std::unique_ptr<IdentifierNode> name;
        std::optional<std::unique_ptr<TypeNode>> type = std::nullopt;

        if (!match(TokenType::LET)) {
            throw SyntaxError("Expected 'var', 'let' or 'const' keyword", peek().getLine(), peek().getColumn());
        }

        if (!check(TokenType::IDENTIFIER)) {
            throw SyntaxError("Expected variable name", peek().getLine(), peek().getColumn());
        } else {
            name = constructCurrentIdentifierNode();
        }

        if (match(TokenType::COLON)) {
            auto typeResult = parseType();
            type = std::make_optional<std::unique_ptr<TypeNode>>(std::move(typeResult.type));
        }

        return VariableDeclarationPrimaryParsingResult{
            .kind = context.kind,
            .name = std::move(name),
            .type = std::move(type),
        };
    }

    RegularFunctionDeclarationParsingResult Parser::parseRegularFunctionDeclaration(RegularFunctionDeclarationParsingContext context) {
        Token firstToken = peek();

        auto signatureResult = parseFunctionSignature();
        auto bodyResult = parseFunctionBody();

        Token lastToken = peek();

        if (context.hasMetadata) {
            return RegularFunctionDeclarationParsingResult{
                .declaration = std::make_unique<FunctionDeclarationNode>(
                    FunctionDeclarationType::Kind::REGULAR,
                    context.context,
                    context.accessModifier,
                    context.binding,
                    std::move(signatureResult.name),
                    std::move(signatureResult.parameters),
                    std::move(signatureResult.returnType),
                    std::move(bodyResult.body),
                    firstToken,
                    lastToken,
                    std::move(context.metadataTerms)
                ),
            };
        } else {
            return RegularFunctionDeclarationParsingResult{
                .declaration = std::make_unique<FunctionDeclarationNode>(
                    FunctionDeclarationType::Kind::REGULAR,
                    context.context,
                    context.accessModifier,
                    context.binding,
                    std::move(signatureResult.name),
                    std::move(signatureResult.parameters),
                    std::move(signatureResult.returnType),
                    std::move(bodyResult.body),
                    firstToken,
                    lastToken
                ),
            };
        }
    }

    NativeFunctionDeclarationParsingResult Parser::parseNativeFunctionDeclaration(NativeFunctionDeclarationParsingContext context) {
        Token firstToken = peek();

        if (!match(TokenType::NATIVE)) {
            throw SyntaxError("Expected 'native' keyword", peek().getLine(), peek().getColumn());
        }

        auto signatureResult = parseFunctionSignature();

        Token lastToken = peek();

        if (context.hasMetadata) {
            return NativeFunctionDeclarationParsingResult{
                .declaration = std::make_unique<FunctionDeclarationNode>(
                    FunctionDeclarationType::Kind::NATIVE,
                    context.context,
                    context.accessModifier,
                    context.binding,
                    std::move(signatureResult.name),
                    std::move(signatureResult.parameters),
                    std::move(signatureResult.returnType),
                    std::nullopt,
                    firstToken,
                    lastToken,
                    std::move(context.metadataTerms)
                ),
            };
        } else {
            return NativeFunctionDeclarationParsingResult{
                .declaration = std::make_unique<FunctionDeclarationNode>(
                    FunctionDeclarationType::Kind::NATIVE,
                    context.context,
                    context.accessModifier,
                    context.binding,
                    std::move(signatureResult.name),
                    std::move(signatureResult.parameters),
                    std::move(signatureResult.returnType),
                    std::nullopt,
                    firstToken,
                    lastToken
                ),
            };
        }
    }

    ParameterListParsingResult Parser::parseParameterList() {
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;

        do {
            auto result = parseParameter();
            parameters.emplace_back(std::move(result.declaration));
        } while (match(TokenType::COMMA));

        return ParameterListParsingResult{
            .parameters = std::move(parameters),
        };
    }

    ClassDeclarationParsingResult Parser::parseClassDeclaration(ClassDeclarationParsingContext context) {
        bool final = false;
        std::unique_ptr<IdentifierNode> name;
        std::optional<std::unique_ptr<TypeNode>> baseClass = std::nullopt;
        std::vector<std::unique_ptr<TypeNode>> implementedInterfaces;
        std::vector<std::unique_ptr<IdentifierNode>> genericParameterNames;
        std::vector<std::unique_ptr<DeclarationNode>> memberDeclarations;

        Token firstToken = peek();

        if (match(TokenType::FINAL)) {
            final = true;
        }

        if (!match(TokenType::CLASS)) {
            throw SyntaxError("Expected 'class' keyword", peek().getLine(), peek().getColumn());
        }

        if (!check(TokenType::IDENTIFIER)) {
            throw SyntaxError("Expected class name", peek().getLine(), peek().getColumn());
        } else {
            name = constructCurrentIdentifierNode();
        }

        if (match(TokenType::LEFT_ANGLE)) {

            auto genericParameterListResult = parseGenericParameterList();
            genericParameterNames = std::move(genericParameterListResult.parameters);

            if (!match(TokenType::RIGHT_ANGLE)) {
                throw SyntaxError("Expected '>' after generic parameter list", peek().getLine(), peek().getColumn());
            }
        }

        if (match(TokenType::EXTENDS)) {

            auto typeResult = parseType();
            baseClass = std::make_optional<std::unique_ptr<TypeNode>>(std::move(typeResult.type));
        }

        if (match(TokenType::IMPLEMENTS)) {
            do {
                auto typeResult = parseType();
                implementedInterfaces.push_back(std::move(typeResult.type));
            } while (match(TokenType::COMMA));
        }

        auto bodyResult = parseClassBody();

        Token lastToken = peek();

        if (context.hasMetadata) {
            return ClassDeclarationParsingResult{
                .declaration = std::make_unique<ClassDeclarationNode>(
                    final,
                    std::move(name),
                    std::move(baseClass),
                    std::move(implementedInterfaces),
                    std::move(genericParameterNames),
                    std::move(bodyResult.declarations),
                    firstToken,
                    lastToken,
                    std::move(context.metadataTerms)
                ),
            };
        } else {
            return ClassDeclarationParsingResult{
                .declaration = std::make_unique<ClassDeclarationNode>(
                    final,
                    std::move(name),
                    std::move(baseClass),
                    std::move(implementedInterfaces),
                    std::move(genericParameterNames),
                    std::move(bodyResult.declarations),
                    firstToken,
                    lastToken
                ),
            };
        }
    }

    InterfaceDeclarationParsingResult Parser::parseInterfaceDeclaration(InterfaceDeclarationParsingContext context) {
        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<IdentifierNode>> genericParameterNames;
        std::vector<std::unique_ptr<FunctionDeclarationNode>> methodDeclarations;

        Token firstToken = peek();

        if (!match(TokenType::INTERFACE)) {
            throw SyntaxError("Expected 'interface' keyword", peek().getLine(), peek().getColumn());
        }

        if (!check(TokenType::IDENTIFIER)) {
            throw SyntaxError("Expected interface name", peek().getLine(), peek().getColumn());
        } else {
            name = constructCurrentIdentifierNode();
        }

        if (match(TokenType::LEFT_ANGLE)) {
            auto genericParameterListResult = parseGenericParameterList();
            genericParameterNames = std::move(genericParameterListResult.parameters);

            if (!match(TokenType::RIGHT_ANGLE)) {
                throw SyntaxError("Expected '>' after generic parameter list", peek().getLine(), peek().getColumn());
            }
        }

        auto bodyResult = parseInterfaceBody();

        Token lastToken = peek();

        if (context.hasMetadata) {
            return InterfaceDeclarationParsingResult{
                .declaration = std::make_unique<
                    InterfaceDeclarationNode>(std::move(name), std::move(genericParameterNames), std::move(bodyResult.declarations), firstToken, lastToken, std::move(context.metadataTerms)),
            };
        } else {
            return InterfaceDeclarationParsingResult{
                .declaration = std::make_unique<InterfaceDeclarationNode>(std::move(name), std::move(genericParameterNames), std::move(bodyResult.declarations), firstToken, lastToken),
            };
        }
    }

    EnumDeclarationParsingResult Parser::parseEnumDeclaration(EnumDeclarationParsingContext context) {
        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<IdentifierNode>> genericParameterNames;
        std::vector<std::unique_ptr<EnumMemberDeclarationNode>> memberDeclarations;

        Token firstToken = peek();

        if (!match(TokenType::ENUM)) {
            throw SyntaxError("Expected 'enum' keyword", peek().getLine(), peek().getColumn());
        }

        if (!check(TokenType::IDENTIFIER)) {
            throw SyntaxError("Expected enum name", peek().getLine(), peek().getColumn());
        } else {
            name = constructCurrentIdentifierNode();
        }

        if (match(TokenType::LEFT_ANGLE)) {
            auto genericParameterListResult = parseGenericParameterList();
            genericParameterNames = std::move(genericParameterListResult.parameters);

            if (!match(TokenType::RIGHT_ANGLE)) {
                throw SyntaxError("Expected '>' after generic parameter list", peek().getLine(), peek().getColumn());
            }
        }

        auto bodyResult = parseEnumBody();

        Token lastToken = peek();

        if (context.hasMetadata) {
            return EnumDeclarationParsingResult{
                .declaration = std::make_unique<
                    EnumDeclarationNode>(std::move(name), std::move(genericParameterNames), std::move(bodyResult.declarations), firstToken, lastToken, std::move(context.metadataTerms)),
            };
        } else {
            return EnumDeclarationParsingResult{
                .declaration = std::make_unique<EnumDeclarationNode>(std::move(name), std::move(genericParameterNames), std::move(bodyResult.declarations), firstToken, lastToken),
            };
        }
    }

    TypeAliasDeclarationParsingResult Parser::parseTypeAliasDeclaration(TypeAliasDeclarationParsingContext context) {
        std::unique_ptr<IdentifierNode> aliasName;
        std::vector<std::unique_ptr<IdentifierNode>> genericParameterNames;
        std::unique_ptr<TypeNode> originalType;

        Token firstToken = peek();

        if (!match(TokenType::TYPE)) {
            throw SyntaxError("Expected 'type' keyword", peek().getLine(), peek().getColumn());
        }

        if (!check(TokenType::IDENTIFIER)) {
            throw SyntaxError("Expected type alias name", peek().getLine(), peek().getColumn());
        } else {
            aliasName = constructCurrentIdentifierNode();
        }

        if (match(TokenType::LEFT_ANGLE)) {
            auto genericParameterListResult = parseGenericParameterList();
            genericParameterNames = std::move(genericParameterListResult.parameters);

            if (!match(TokenType::RIGHT_ANGLE)) {
                throw SyntaxError("Expected '>' after generic parameter list", peek().getLine(), peek().getColumn());
            }
        }

        if (!match(TokenType::EQUAL)) {
            throw SyntaxError("Expected '=' after type alias name", peek().getLine(), peek().getColumn());
        }

        auto typeResult = parseType();

        Token lastToken = peek();

        if (context.hasMetadata) {
            return TypeAliasDeclarationParsingResult{
                .declaration = std::make_unique<
                    TypeAliasDeclarationNode>(std::move(aliasName), std::move(genericParameterNames), std::move(typeResult.type), firstToken, lastToken, std::move(context.metadataTerms)),
            };
        } else {
            return TypeAliasDeclarationParsingResult{
                .declaration = std::make_unique<TypeAliasDeclarationNode>(std::move(aliasName), std::move(genericParameterNames), std::move(typeResult.type), firstToken, lastToken),
            };
        }
    }

    ExportListParsingResult Parser::parseExportList() {
        std::vector<ExportDeclarationItem> items;

        do {
            std::unique_ptr<IdentifierNode> name;
            std::optional<std::unique_ptr<IdentifierNode>> alias = std::nullopt;

            if (!check(TokenType::IDENTIFIER)) {
                throw SyntaxError("Expected identifier in export list", peek().getLine(), peek().getColumn());
            } else {
                name = constructCurrentIdentifierNode();
            }

            items.emplace_back(ExportDeclarationItem{ .name = std::move(name) });
        } while (match(TokenType::COMMA));

        return ExportListParsingResult{
            .items = std::move(items),
        };
    }

    MetadataTermParsingResult Parser::parseMetadataTerm() {
        std::unique_ptr<IdentifierNode> key;
        std::optional<std::string> value = std::nullopt;

        if (!checkGeneralizedIdentifier()) {
            throw SyntaxError("Expected metadata term key", peek().getLine(), peek().getColumn());
        } else {
            key = constructCurrentIdentifierNode();
        }

        if (check(TokenType::STRING)) {
            std::string literal(peek().getValue());

            if (!literal.starts_with('"') || !literal.ends_with('"')) {
                throw SyntaxError("Metadata term value must be a simple string literal", peek().getLine(), peek().getColumn());
            }

            value = std::make_optional<std::string>(literal.substr(1, literal.size() - 2));
            advance();
        }

        return MetadataTermParsingResult{
            .term =
                DeclarationItem::MetadataTerm{
                    .key = std::move(key),
                    .value = std::move(value),
                },
        };
    }

    FunctionSignatureParsingResult Parser::parseFunctionSignature() {
        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;
        std::optional<std::unique_ptr<TypeNode>> returnType;

        if (!match(TokenType::FUNC)) {
            throw SyntaxError("Expected 'func' keyword", peek().getLine(), peek().getColumn());
        }

        if (!check(TokenType::IDENTIFIER)) {
            throw SyntaxError("Expected function name", peek().getLine(), peek().getColumn());
        } else {
            name = constructCurrentIdentifierNode();
        }

        if (!match(TokenType::LEFT_PARENTHESIS)) {
            throw SyntaxError("Expected '(' after function name", peek().getLine(), peek().getColumn());
        }

        if (!check(TokenType::RIGHT_PARENTHESIS)) {
            auto parameterListResult = parseParameterList();

            for (auto& parameter : parameterListResult.parameters) {
                parameters.emplace_back(std::move(parameter));
            }
        }

        if (!match(TokenType::RIGHT_PARENTHESIS)) {
            throw SyntaxError("Expected ')' after parameter list", peek().getLine(), peek().getColumn());
        }

        if (match(TokenType::ARROW)) {
            if (!match(TokenType::VOID)) {
                auto typeResult = parseType();
                returnType = std::make_optional<std::unique_ptr<TypeNode>>(std::move(typeResult.type));
            }
        }

        return FunctionSignatureParsingResult{
            .name = std::move(name),
            .parameters = std::move(parameters),
            .returnType = std::move(returnType),
        };
    }

    ImportPathParsingResult Parser::parseImportPath() {
        std::unique_ptr<ImportDeclarationItem> paths;
        std::vector<std::unique_ptr<IdentifierNode>> namePrefix;

        while (true) {
            if (!check(TokenType::IDENTIFIER)) {
                throw SyntaxError("Expected identifier in import path", peek().getLine(), peek().getColumn());
            } else {
                namePrefix.emplace_back(constructCurrentIdentifierNode());
            }

            if (!check(TokenType::DOT)) {
                break;
            } else {
                advance();

                if (match(TokenType::ASTERISK)) {
                    paths = std::make_unique<ImportDeclarationItem>(ImportDeclarationItem{
                        .namePrefix = std::move(namePrefix),
                        .nameSuffixes = std::vector<std::unique_ptr<ImportDeclarationItem>>{},
                        .alias = std::nullopt,
                        .self = false,
                        .wildcard = true,
                    });

                    return ImportPathParsingResult{
                        .paths = std::move(paths),
                    };
                } else {
                    if (check(TokenType::IDENTIFIER)) {
                        continue;
                    } else if (check(TokenType::LEFT_BRACE)) {
                        break;
                    } else {
                        throw SyntaxError("Expected identifier, '*' or '{' after '.'", peek().getLine(), peek().getColumn());
                    }
                }
            }
        }

        if (match(TokenType::AS)) {
            if (!check(TokenType::IDENTIFIER)) {
                throw SyntaxError("Expected identifier after 'as' keyword in import path", peek().getLine(), peek().getColumn());
            } else {
                std::unique_ptr<IdentifierNode> alias = constructCurrentIdentifierNode();

                paths = std::make_unique<ImportDeclarationItem>(ImportDeclarationItem{
                    .namePrefix = std::move(namePrefix),
                    .nameSuffixes = std::vector<std::unique_ptr<ImportDeclarationItem>>{},
                    .alias = std::make_optional<std::unique_ptr<IdentifierNode>>(std::move(alias)),
                    .self = false,
                    .wildcard = false,
                });
            }
        } else if (match(TokenType::LEFT_BRACE)) {

            auto listResult = parseImportPathList();

            paths = std::make_unique<ImportDeclarationItem>(ImportDeclarationItem{
                .namePrefix = std::move(namePrefix),
                .nameSuffixes = std::vector<std::unique_ptr<ImportDeclarationItem>>(std::move(listResult.paths)),
                .alias = std::nullopt,
                .self = false,
                .wildcard = false,
            });

            if (!match(TokenType::RIGHT_BRACE)) {
                throw SyntaxError("Expected '}' after import path list", peek().getLine(), peek().getColumn());
            }
        } else {
            paths = std::make_unique<ImportDeclarationItem>(ImportDeclarationItem{
                .namePrefix = std::move(namePrefix),
                .nameSuffixes = std::vector<std::unique_ptr<ImportDeclarationItem>>{},
                .alias = std::nullopt,
                .self = false,
                .wildcard = false,
            });
        }

        return ImportPathParsingResult{
            .paths = std::move(paths),
        };
    }

    TypeParsingResult Parser::parseType() {
        Token firstToken = peek();

        bool questionMarkSuffix = false;
        std::vector<std::unique_ptr<IdentifierNode>> nameParts;
        std::vector<std::unique_ptr<TypeNode>> genericArguments;

        std::unordered_set<TokenType> primitiveTypes = {
            TokenType::BYTE_TYPE, TokenType::SHORT_TYPE, TokenType::INT_TYPE, TokenType::LONG_TYPE, TokenType::FLOAT_TYPE, TokenType::DOUBLE_TYPE, TokenType::BOOL_TYPE, TokenType::STRING_TYPE,
        };

        if (std::find(primitiveTypes.begin(), primitiveTypes.end(), peek().getType()) != primitiveTypes.end()) {
            nameParts.emplace_back(constructCurrentIdentifierNode());
        } else {
            do {
                if (!check(TokenType::IDENTIFIER)) {
                    throw SyntaxError("Expected identifier in type", peek().getLine(), peek().getColumn());
                } else {
                    nameParts.emplace_back(constructCurrentIdentifierNode());
                }
            } while (match(TokenType::DOT));

            if (match(TokenType::LEFT_ANGLE)) {
                auto genericArgumentListResult = parseGenericArgumentList();
                genericArguments = std::move(genericArgumentListResult.arguments);

                if (!consumeRightAngleInType()) {
                    throw SyntaxError("Expected '>' after generic argument list in type", peek().getLine(), peek().getColumn());
                }
            }
        }

        if (match(TokenType::QUESTION)) {
            questionMarkSuffix = true;
        }

        Token lastToken = peek();

        return TypeParsingResult{
            .type = std::make_unique<TypeNode>(questionMarkSuffix, std::move(nameParts), std::move(genericArguments), firstToken, lastToken),
        };
    }

    ParameterParsingResult Parser::parseParameter() {
        std::unique_ptr<IdentifierNode> name;
        std::unique_ptr<TypeNode> type;

        Token firstToken = peek();

        if (!check(TokenType::IDENTIFIER)) {
            throw SyntaxError("Expected parameter name", peek().getLine(), peek().getColumn());
        } else {
            name = constructCurrentIdentifierNode();
        }

        if (!match(TokenType::COLON)) {
            throw SyntaxError("Expected ':' after parameter name", peek().getLine(), peek().getColumn());
        }

        auto typeResult = parseType();
        type = std::move(typeResult.type);

        Token lastToken = peek();

        return ParameterParsingResult{
            .declaration = std::make_unique<ValueDeclarationNode>(
                ValueDeclarationType::Kind::PARAMETER,
                ValueDeclarationType::Context::FUNCTION,
                ValueDeclarationType::AccessModifier::PUBLIC,
                std::move(name),
                std::move(type),
                std::nullopt,
                firstToken,
                lastToken
            ),
        };
    }

    GenericParameterListParsingResult Parser::parseGenericParameterList() {
        std::vector<std::unique_ptr<IdentifierNode>> parameters;

        do {
            if (!check(TokenType::IDENTIFIER)) {
                throw SyntaxError("Expected identifier in generic parameter list", peek().getLine(), peek().getColumn());
            } else {
                parameters.emplace_back(constructCurrentIdentifierNode());
            }
        } while (match(TokenType::COMMA));

        return GenericParameterListParsingResult{
            .parameters = std::move(parameters),
        };
    }

    GenericArgumentListParsingResult Parser::parseGenericArgumentList() {
        std::vector<std::unique_ptr<TypeNode>> arguments;

        do {
            auto typeResult = parseType();
            arguments.push_back(std::move(typeResult.type));
        } while (match(TokenType::COMMA));

        return GenericArgumentListParsingResult{
            .arguments = std::move(arguments),
        };
    }

    ImportPathListParsingResult Parser::parseImportPathList() {
        std::vector<std::unique_ptr<ImportDeclarationItem>> paths;

        do {
            auto itemResult = parseImportPathItem();
            paths.push_back(std::move(itemResult.paths));
        } while (match(TokenType::COMMA));

        return ImportPathListParsingResult{
            .paths = std::move(paths),
        };
    }

    FunctionBodyParsingResult Parser::parseFunctionBody() {
        auto result = parseBlockStatement();

        return FunctionBodyParsingResult{
            .body = std::move(result.statement),
        };
    }

    ClassBodyParsingResult Parser::parseClassBody() {
        std::vector<std::unique_ptr<DeclarationNode>> declarations;

        if (!match(TokenType::LEFT_BRACE)) {
            throw SyntaxError("Expected '{' at the beginning of class body", peek().getLine(), peek().getColumn());
        }

        while (!check(TokenType::RIGHT_BRACE)) {
            auto result = parseClassMember();
            declarations.push_back(std::move(result.declaration));
        }

        if (!match(TokenType::RIGHT_BRACE)) {
            throw SyntaxError("Expected '}' at the end of class body", peek().getLine(), peek().getColumn());
        }

        return ClassBodyParsingResult{
            .declarations = std::move(declarations),
        };
    }

    InterfaceBodyParsingResult Parser::parseInterfaceBody() {
        std::vector<std::unique_ptr<FunctionDeclarationNode>> declarations;

        if (!match(TokenType::LEFT_BRACE)) {
            throw SyntaxError("Expected '{' at the beginning of interface body", peek().getLine(), peek().getColumn());
        }

        while (!check(TokenType::RIGHT_BRACE)) {
            auto result = parseInterfaceMethodDeclaration();
            declarations.push_back(std::move(result.declaration));
        }

        if (!match(TokenType::RIGHT_BRACE)) {
            throw SyntaxError("Expected '}' at the end of interface body", peek().getLine(), peek().getColumn());
        }

        return InterfaceBodyParsingResult{
            .declarations = std::move(declarations),
        };
    }

    EnumBodyParsingResult Parser::parseEnumBody() {
        std::vector<std::unique_ptr<EnumMemberDeclarationNode>> declarations;

        if (!match(TokenType::LEFT_BRACE)) {
            throw SyntaxError("Expected '{' at the beginning of enum body", peek().getLine(), peek().getColumn());
        }

        while (!check(TokenType::RIGHT_BRACE)) {
            auto result = parseEnumMemberDeclaration();
            declarations.push_back(std::move(result.declaration));
        }

        if (!match(TokenType::RIGHT_BRACE)) {
            throw SyntaxError("Expected '}' at the end of enum body", peek().getLine(), peek().getColumn());
        }

        return EnumBodyParsingResult{
            .declarations = std::move(declarations),
        };
    }

    ImportPathItemParsingResult Parser::parseImportPathItem() {
        std::vector<std::unique_ptr<IdentifierNode>> namePrefix;
        std::optional<std::unique_ptr<IdentifierNode>> alias = std::nullopt;

        if (check(TokenType::SELF)) {
            namePrefix.emplace_back(constructCurrentIdentifierNode());

            if (match(TokenType::AS)) {
                if (!check(TokenType::IDENTIFIER)) {
                    throw SyntaxError("Expected identifier after 'as' keyword in import path", peek().getLine(), peek().getColumn());
                } else {
                    alias = std::make_optional<std::unique_ptr<IdentifierNode>>(constructCurrentIdentifierNode());
                }
            }

            return ImportPathItemParsingResult{
                .paths = std::make_unique<ImportDeclarationItem>(ImportDeclarationItem{
                    .namePrefix = std::move(namePrefix),
                    .nameSuffixes = std::vector<std::unique_ptr<ImportDeclarationItem>>{},
                    .alias = std::move(alias),
                    .self = true,
                    .wildcard = false,
                }),
            };
        } else if (check(TokenType::ASTERISK)) {
            namePrefix.emplace_back(constructCurrentIdentifierNode());

            return ImportPathItemParsingResult{
                .paths = std::make_unique<ImportDeclarationItem>(ImportDeclarationItem{
                    .namePrefix = std::move(namePrefix),
                    .nameSuffixes = std::vector<std::unique_ptr<ImportDeclarationItem>>{},
                    .alias = std::nullopt,
                    .self = false,
                    .wildcard = true,
                }),
            };
        } else {
            auto result = parseImportPath();

            return ImportPathItemParsingResult{
                .paths = std::move(result.paths),
            };
        }
    }

    ClassMemberParsingResult Parser::parseClassMember() {
        Token firstToken = peek();

        bool hasMetadata = false;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;

        enum class AccessModifier {
            PUBLIC,
            PROTECTED,
            PRIVATE,
        };

        enum class Binding {
            INSTANCE,
            STATIC,
        };

        AccessModifier accessModifier = AccessModifier::PUBLIC;
        Binding binding = Binding::INSTANCE;

        if (check(TokenType::METADATA)) {
            hasMetadata = true;
            auto metadataResult = parseMetadata();
            metadataTerms = std::move(metadataResult.metadata);
        }

        if (check(TokenType::INIT)) {
            ConstructorParsingContext constructorContext{
                .hasMetadata = hasMetadata,
                .metadataTerms = std::move(metadataTerms),
            };

            auto constructorResult = parseConstructor(std::move(constructorContext));

            Token lastToken = peek();
            constructorResult.constructor->resetPosition(firstToken, lastToken);

            return ClassMemberParsingResult{
                .declaration = std::move(constructorResult.constructor),
            };
        }

        if (match(TokenType::PUBLIC)) {
            accessModifier = AccessModifier::PUBLIC;
        } else if (match(TokenType::PRIVATE)) {
            accessModifier = AccessModifier::PRIVATE;
        }

        if (match(TokenType::STATIC)) {
            binding = Binding::STATIC;
        } else if (match(TokenType::OVERRIDE)) {
            FunctionDeclarationParsingContext functionDeclarationContext{
                .context = FunctionDeclarationType::Context::CLASS,
                .accessModifier = static_cast<FunctionDeclarationType::AccessModifier>(accessModifier),
                .binding = static_cast<FunctionDeclarationType::Binding>(binding),
                .hasMetadata = hasMetadata,
                .metadataTerms = std::move(metadataTerms),
            };
            auto functionDeclarationResult = parseFunctionDeclaration(std::move(functionDeclarationContext));

            Token lastToken = peek();
            functionDeclarationResult.declaration->resetPosition(firstToken, lastToken);

            if (!endsWithNewlineOrEOF) {
                throw SyntaxError("Expected newline after method declaration", peek().getLine(), peek().getColumn());
            }

            return ClassMemberParsingResult{
                .declaration = std::move(functionDeclarationResult.declaration),
            };
        }

        if (check(TokenType::FUNC) || check(TokenType::NATIVE)) {
            FunctionDeclarationParsingContext functionDeclarationContext{
                .context = FunctionDeclarationType::Context::CLASS,
                .accessModifier = static_cast<FunctionDeclarationType::AccessModifier>(accessModifier),
                .binding = static_cast<FunctionDeclarationType::Binding>(binding),
                .hasMetadata = hasMetadata,
                .metadataTerms = std::move(metadataTerms),
            };
            auto functionDeclarationResult = parseFunctionDeclaration(std::move(functionDeclarationContext));

            Token lastToken = peek();
            functionDeclarationResult.declaration->resetPosition(firstToken, lastToken);

            if (!endsWithNewlineOrEOF) {
                throw SyntaxError("Expected newline after method declaration", peek().getLine(), peek().getColumn());
            }

            return ClassMemberParsingResult{
                .declaration = std::move(functionDeclarationResult.declaration),
            };
        } else {
            PropertyDeclarationParsingContext propertyDeclarationContext{
                .accessModifier = static_cast<PropertyDeclarationType::AccessModifier>(accessModifier),
                .binding = static_cast<PropertyDeclarationType::Binding>(binding),
                .hasMetadata = hasMetadata,
                .metadataTerms = std::move(metadataTerms),
            };
            auto propertyDeclarationResult = parsePropertyDeclaration(std::move(propertyDeclarationContext));

            Token lastToken = peek();
            propertyDeclarationResult.declaration->resetPosition(firstToken, lastToken);

            if (!endsWithNewlineOrEOF) {
                throw SyntaxError("Expected newline after property declaration", peek().getLine(), peek().getColumn());
            }

            return ClassMemberParsingResult{
                .declaration = std::move(propertyDeclarationResult.declaration),
            };
        }
    }

    ConstructorParsingResult Parser::parseConstructor(ConstructorParsingContext context) {
        Token firstToken = peek();

        std::vector<std::unique_ptr<ValueDeclarationNode>> parameters;

        Token identifierFirstToken = peek();
        if (!match(TokenType::INIT)) {
            throw SyntaxError("Expected 'init' keyword", peek().getLine(), peek().getColumn());
        }
        Token identifierLastToken = peek();

        if (!match(TokenType::LEFT_PARENTHESIS)) {
            throw SyntaxError("Expected '('", peek().getLine(), peek().getColumn());
        }

        if (!check(TokenType::RIGHT_PARENTHESIS)) {
            auto parameterListResult = parseParameterList();

            for (auto& parameter : parameterListResult.parameters) {
                parameters.emplace_back(std::move(parameter));
            }
        }

        if (!match(TokenType::RIGHT_PARENTHESIS)) {
            throw SyntaxError("Expected ')'", peek().getLine(), peek().getColumn());
        }

        auto bodyResult = parseFunctionBody();

        std::string name = "__vnl_constructor";

        if (!parameters.empty()) {
            name += '-';
            for (auto& parameter : parameters) {
                if (parameter->getType().has_value()) {
                    name += '-' + generateNamespaceIdFromTypeName(*parameter->getType().value());
                }
            }
        }

        name += "__";
        std::unique_ptr<IdentifierNode> nameNode = std::make_unique<IdentifierNode>(name, identifierFirstToken, identifierLastToken);

        Token lastToken = peek();

        if (context.hasMetadata) {
            return ConstructorParsingResult{
                .constructor = std::make_unique<FunctionDeclarationNode>(
                    FunctionDeclarationType::Kind::REGULAR,
                    FunctionDeclarationType::Context::CLASS,
                    FunctionDeclarationType::AccessModifier::PUBLIC,
                    FunctionDeclarationType::Binding::INSTANCE,
                    std::move(nameNode),
                    std::move(parameters),
                    std::nullopt,
                    std::move(bodyResult.body),
                    firstToken,
                    lastToken,
                    std::move(context.metadataTerms)
                ),
            };
        } else {
            return ConstructorParsingResult{
                .constructor = std::make_unique<FunctionDeclarationNode>(
                    FunctionDeclarationType::Kind::REGULAR,
                    FunctionDeclarationType::Context::CLASS,
                    FunctionDeclarationType::AccessModifier::PUBLIC,
                    FunctionDeclarationType::Binding::INSTANCE,
                    std::move(nameNode),
                    std::move(parameters),
                    std::nullopt,
                    std::move(bodyResult.body),
                    firstToken,
                    lastToken
                ),
            };
        }
    }

    EnumMemberDeclarationParsingResult Parser::parseEnumMemberDeclaration() {
        Token firstToken = peek();

        bool hasMetadata = false;
        std::vector<DeclarationItem::MetadataTerm> metadataTerms;
        std::unique_ptr<IdentifierNode> name;
        std::vector<std::unique_ptr<ValueDeclarationNode>> associatedValues;

        if (check(TokenType::METADATA)) {
            hasMetadata = true;
            auto metadataResult = parseMetadata();
            metadataTerms = std::move(metadataResult.metadata);
        }

        if (!check(TokenType::IDENTIFIER)) {
            throw SyntaxError("Expected enum member name", peek().getLine(), peek().getColumn());
        } else {
            name = constructCurrentIdentifierNode();
        }

        if (match(TokenType::LEFT_PARENTHESIS)) {
            if (!check(TokenType::RIGHT_PARENTHESIS)) {
                auto enumAssoicatedValueListResult = parseEnumAssociatedValueList();
                for (auto& associatedValue : enumAssoicatedValueListResult.associatedValues) {
                    associatedValues.emplace_back(std::move(associatedValue));
                }
            }

            if (!match(TokenType::RIGHT_PARENTHESIS)) {
                throw SyntaxError("Expected ')' after enum member associated value list", peek().getLine(), peek().getColumn());
            }
        }

        Token lastToken = peek();

        if (!endsWithNewlineOrEOF) {
            throw SyntaxError("Expected newline after enum member declaration", peek().getLine(), peek().getColumn());
        }

        if (hasMetadata) {
            return EnumMemberDeclarationParsingResult{
                .declaration = std::make_unique<EnumMemberDeclarationNode>(std::move(name), std::move(associatedValues), firstToken, lastToken, std::move(metadataTerms)),
            };
        } else {
            return EnumMemberDeclarationParsingResult{
                .declaration = std::make_unique<EnumMemberDeclarationNode>(std::move(name), std::move(associatedValues), firstToken, lastToken),
            };
        }
    }

    EnumAssociatedValueListParsingResult Parser::parseEnumAssociatedValueList() {
        std::vector<std::unique_ptr<ValueDeclarationNode>> items;

        do {
            auto result = parseEnumAssociatedValue();
            items.emplace_back(std::move(result.declaration));
        } while (match(TokenType::COMMA));

        return EnumAssociatedValueListParsingResult{
            .associatedValues = std::move(items),
        };
    }

    EnumAssociatedValueParsingResult Parser::parseEnumAssociatedValue() {
        std::unique_ptr<IdentifierNode> name;
        std::unique_ptr<TypeNode> type;

        Token firstToken = peek();

        if (!check(TokenType::IDENTIFIER)) {
            throw SyntaxError("Expected parameter name", peek().getLine(), peek().getColumn());
        } else {
            name = constructCurrentIdentifierNode();
        }

        if (!match(TokenType::COLON)) {
            throw SyntaxError("Expected ':' after parameter name", peek().getLine(), peek().getColumn());
        }

        auto typeResult = parseType();
        type = std::move(typeResult.type);

        Token lastToken = peek();

        return EnumAssociatedValueParsingResult{
            .declaration = std::make_unique<ValueDeclarationNode>(
                ValueDeclarationType::Kind::ENUM_ASSOCIATED_VALUE,
                ValueDeclarationType::Context::ENUM_MEMBER,
                ValueDeclarationType::AccessModifier::PUBLIC,
                std::move(name),
                std::move(type),
                std::nullopt,
                firstToken,
                lastToken
            ),
        };
    }

    ExpressionParsingResult Parser::parseExpression() {
        auto result = parseAssignmentExpression();

        return ExpressionParsingResult{
            .expression = std::move(result.expression),
        };
    }

    AssignmentExpressionParsingResult Parser::parseAssignmentExpression() {
        static const std::unordered_set<TokenType> assignmentOperators = {
            TokenType::EQUAL,
            TokenType::DOUBLE_QUESTION_EQUAL,
            TokenType::PLUS_EQUAL,
            TokenType::MINUS_EQUAL,
            TokenType::ASTERISK_EQUAL,
            TokenType::SLASH_EQUAL,
            TokenType::DOUBLE_SLASH_EQUAL,
            TokenType::PERCENT_EQUAL,
            TokenType::DOUBLE_ASTERISK_EQUAL,
            TokenType::AMPERSAND_EQUAL,
            TokenType::CARET_EQUAL,
            TokenType::PIPE_EQUAL,
            TokenType::DOUBLE_LEFT_ANGLE,
            TokenType::DOUBLE_RIGHT_ANGLE,
            TokenType::TRIPLE_RIGHT_ANGLE,
        };

        static const std::unordered_map<TokenType, BinaryExpressionType> assignmentExpressionTypes = {
            { TokenType::EQUAL, BinaryExpressionType::ASSIGNMENT },
            { TokenType::DOUBLE_QUESTION_EQUAL, BinaryExpressionType::NULLISH_COALESCING_ASSIGNMENT },
            { TokenType::PLUS_EQUAL, BinaryExpressionType::ADDITION_ASSIGNMENT },
            { TokenType::MINUS_EQUAL, BinaryExpressionType::SUBTRACTION_ASSIGNMENT },
            { TokenType::ASTERISK_EQUAL, BinaryExpressionType::MULTIPLICATION_ASSIGNMENT },
            { TokenType::SLASH_EQUAL, BinaryExpressionType::DIVISION_ASSIGNMENT },
            { TokenType::DOUBLE_SLASH_EQUAL, BinaryExpressionType::INTEGER_DIVISION_ASSIGNMENT },
            { TokenType::PERCENT_EQUAL, BinaryExpressionType::MODULO_ASSIGNMENT },
            { TokenType::DOUBLE_ASTERISK_EQUAL, BinaryExpressionType::EXPONENT_ASSIGNMENT },
            { TokenType::AMPERSAND_EQUAL, BinaryExpressionType::BITWISE_AND_ASSIGNMENT },
            { TokenType::CARET_EQUAL, BinaryExpressionType::BITWISE_XOR_ASSIGNMENT },
            { TokenType::PIPE_EQUAL, BinaryExpressionType::BITWISE_OR_ASSIGNMENT },
            { TokenType::DOUBLE_LEFT_ANGLE, BinaryExpressionType::SHIFT_LEFT_ASSIGNMENT },
            { TokenType::DOUBLE_RIGHT_ANGLE, BinaryExpressionType::SHIFT_RIGHT_ASSIGNMENT },
            { TokenType::TRIPLE_RIGHT_ANGLE, BinaryExpressionType::SHIFT_RIGHT_UNSIGNED_ASSIGNMENT },
        };

        Token firstToken = peek();

        auto leftResult = parseConditionalExpression();

        if (checkAny(assignmentOperators)) {
            BinaryExpressionType operatorType = assignmentExpressionTypes.at(peek().getType());
            advance();
            auto rightResult = parseAssignmentExpression();

            Token lastToken = peek();

            return AssignmentExpressionParsingResult{
                .expression = std::make_unique<BinaryExpressionNode>(operatorType, std::move(leftResult.expression), std::move(rightResult.expression), firstToken, lastToken),
            };
        } else {
            return AssignmentExpressionParsingResult{
                .expression = std::move(leftResult.expression),
            };
        }
    }

    ConditionalExpressionParsingResult Parser::parseConditionalExpression() {
        Token firstToken = peek();

        auto leftResult = parseNullishCoalescingExpression();

        if (match(TokenType::QUESTION)) {
            auto middleResult = parseAssignmentExpression();

            if (!match(TokenType::COLON)) {
                throw SyntaxError("Expected ':' in conditional expression", peek().getLine(), peek().getColumn());
            }

            auto rightResult = parseConditionalExpression();

            Token lastToken = peek();

            return ConditionalExpressionParsingResult{
                .expression =
                    std::make_unique<ConditionalExpressionNode>(std::move(leftResult.expression), std::move(middleResult.expression), std::move(rightResult.expression), firstToken, lastToken),
            };
        } else {
            return ConditionalExpressionParsingResult{
                .expression = std::move(leftResult.expression),
            };
        }
    }

    NullishCoalescingExpressionParsingResult Parser::parseNullishCoalescingExpression() {
        Token firstToken = peek();

        auto leftResult = parseLogicalOrExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(leftResult.expression);

        while (match(TokenType::DOUBLE_QUESTION)) {
            auto rightResult = parseLogicalOrExpression();

            Token lastToken = peek();

            currentNode = std::make_unique<BinaryExpressionNode>(BinaryExpressionType::NULLISH_COALESCING, std::move(currentNode), std::move(rightResult.expression), firstToken, lastToken);
        }

        return NullishCoalescingExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    LogicalOrExpressionParsingResult Parser::parseLogicalOrExpression() {
        Token firstToken = peek();

        auto leftResult = parseLogicalAndExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(leftResult.expression);

        while (match(TokenType::DOUBLE_PIPE)) {
            auto rightResult = parseLogicalAndExpression();

            Token lastToken = peek();

            currentNode = std::make_unique<BinaryExpressionNode>(BinaryExpressionType::LOGICAL_OR, std::move(currentNode), std::move(rightResult.expression), firstToken, lastToken);
        }

        return LogicalOrExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    LogicalAndExpressionParsingResult Parser::parseLogicalAndExpression() {
        Token firstToken = peek();

        auto leftResult = parseEqualityExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(leftResult.expression);

        while (match(TokenType::DOUBLE_AMPERSAND)) {
            auto rightResult = parseEqualityExpression();

            Token lastToken = peek();

            currentNode = std::make_unique<BinaryExpressionNode>(BinaryExpressionType::LOGICAL_AND, std::move(currentNode), std::move(rightResult.expression), firstToken, lastToken);
        }

        return LogicalAndExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    EqualityExpressionParsingResult Parser::parseEqualityExpression() {
        static const std::unordered_set<TokenType> equalityOperators = {
            TokenType::DOUBLE_EQUAL,
            TokenType::EXCLAMATION_EQUAL,
        };

        static const std::unordered_map<TokenType, BinaryExpressionType> equalityExpressionTypes = {
            { TokenType::DOUBLE_EQUAL, BinaryExpressionType::EQUAL },
            { TokenType::EXCLAMATION_EQUAL, BinaryExpressionType::NOT_EQUAL },
        };

        Token firstToken = peek();

        auto leftResult = parseRelationalExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(leftResult.expression);

        while (checkAny(equalityOperators)) {
            BinaryExpressionType operatorType = equalityExpressionTypes.at(peek().getType());
            advance();
            auto rightResult = parseRelationalExpression();

            Token lastToken = peek();

            currentNode = std::make_unique<BinaryExpressionNode>(operatorType, std::move(currentNode), std::move(rightResult.expression), firstToken, lastToken);
        }

        return EqualityExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    RelationalExpressionParsingResult Parser::parseRelationalExpression() {
        static const std::unordered_set<TokenType> relationalOperators = {
            TokenType::LEFT_ANGLE,
            TokenType::LEFT_ANGLE_EQUAL,
            TokenType::RIGHT_ANGLE,
            TokenType::RIGHT_ANGLE_EQUAL,
        };

        static const std::unordered_map<TokenType, BinaryExpressionType> relationalExpressionTypes = {
            { TokenType::LEFT_ANGLE, BinaryExpressionType::LESS_THAN },
            { TokenType::LEFT_ANGLE_EQUAL, BinaryExpressionType::LESS_THAN_OR_EQUAL },
            { TokenType::RIGHT_ANGLE, BinaryExpressionType::GREATER_THAN },
            { TokenType::RIGHT_ANGLE_EQUAL, BinaryExpressionType::GREATER_THAN_OR_EQUAL },
        };

        Token firstToken = peek();

        auto leftResult = parseRangeExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(leftResult.expression);

        while (checkAny(relationalOperators)) {
            BinaryExpressionType operatorType = relationalExpressionTypes.at(peek().getType());
            advance();
            auto rightResult = parseRangeExpression();

            Token lastToken = peek();

            currentNode = std::make_unique<BinaryExpressionNode>(operatorType, std::move(currentNode), std::move(rightResult.expression), firstToken, lastToken);
        }

        return RelationalExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    RangeExpressionParsingResult Parser::parseRangeExpression() {
        Token firstToken = peek();

        if (match(TokenType::DOUBLE_DOT)) {
            auto endResult = parseBitwiseOrExpression();
            Token lastToken = peek();

            return RangeExpressionParsingResult{
                .expression = std::make_unique<RangeExpressionNode>(std::nullopt, std::make_optional(std::move(endResult.expression)), firstToken, lastToken),
            };
        } else {
            auto startResult = parseBitwiseOrExpression();

            if (match(TokenType::DOUBLE_DOT)) {
                static const std::unordered_set<TokenType> rangeEndExpressionStarters = {
                    TokenType::IDENTIFIER,  TokenType::NUMBER,           TokenType::STRING,       TokenType::CHAR,       TokenType::TRUE, TokenType::FALSE, TokenType::THIS,
                    TokenType::SUPER,       TokenType::LEFT_PARENTHESIS, TokenType::LEFT_BRACKET, TokenType::LEFT_BRACE, TokenType::PLUS, TokenType::MINUS, TokenType::TILDE,
                    TokenType::EXCLAMATION, TokenType::SELECTOR_PREFIX,
                };

                if (checkAny(rangeEndExpressionStarters)) {
                    auto endResult = parseBitwiseOrExpression();
                    Token lastToken = peek();

                    return RangeExpressionParsingResult{
                        .expression = std::make_unique<RangeExpressionNode>(
                            std::make_optional(std::move(startResult.expression)),
                            std::make_optional(std::move(endResult.expression)),
                            firstToken,
                            lastToken
                        ),
                    };
                } else {
                    Token lastToken = peek();

                    return RangeExpressionParsingResult{
                        .expression = std::make_unique<RangeExpressionNode>(std::make_optional(std::move(startResult.expression)), std::nullopt, firstToken, lastToken),
                    };
                }
            } else {
                return RangeExpressionParsingResult{
                    .expression = std::move(startResult.expression),
                };
            }
        }
    }

    BitwiseOrExpressionParsingResult Parser::parseBitwiseOrExpression() {
        Token firstToken = peek();

        auto leftResult = parseBitwiseXorExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(leftResult.expression);

        while (match(TokenType::PIPE)) {
            auto rightResult = parseBitwiseXorExpression();

            Token lastToken = peek();

            currentNode = std::make_unique<BinaryExpressionNode>(BinaryExpressionType::BITWISE_OR, std::move(currentNode), std::move(rightResult.expression), firstToken, lastToken);
        }

        return BitwiseOrExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    BitwiseXorExpressionParsingResult Parser::parseBitwiseXorExpression() {
        Token firstToken = peek();

        auto leftResult = parseBitwiseAndExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(leftResult.expression);

        while (match(TokenType::CARET)) {
            auto rightResult = parseBitwiseAndExpression();

            Token lastToken = peek();

            currentNode = std::make_unique<BinaryExpressionNode>(BinaryExpressionType::BITWISE_XOR, std::move(currentNode), std::move(rightResult.expression), firstToken, lastToken);
        }

        return BitwiseXorExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    BitwiseAndExpressionParsingResult Parser::parseBitwiseAndExpression() {
        Token firstToken = peek();

        auto leftResult = parseShiftExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(leftResult.expression);

        while (match(TokenType::AMPERSAND)) {
            auto rightResult = parseShiftExpression();

            Token lastToken = peek();

            currentNode = std::make_unique<BinaryExpressionNode>(BinaryExpressionType::BITWISE_AND, std::move(currentNode), std::move(rightResult.expression), firstToken, lastToken);
        }

        return BitwiseAndExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    ShiftExpressionParsingResult Parser::parseShiftExpression() {
        static const std::unordered_set<TokenType> shiftOperators = {
            TokenType::DOUBLE_LEFT_ANGLE,
            TokenType::DOUBLE_RIGHT_ANGLE,
            TokenType::TRIPLE_RIGHT_ANGLE,
        };

        static const std::unordered_map<TokenType, BinaryExpressionType> shiftExpressionTypes = {
            { TokenType::DOUBLE_LEFT_ANGLE, BinaryExpressionType::SHIFT_LEFT },
            { TokenType::DOUBLE_RIGHT_ANGLE, BinaryExpressionType::SHIFT_RIGHT },
            { TokenType::TRIPLE_RIGHT_ANGLE, BinaryExpressionType::SHIFT_RIGHT_UNSIGNED },
        };

        Token firstToken = peek();

        auto leftResult = parseAdditiveExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(leftResult.expression);

        while (checkAny(shiftOperators)) {
            BinaryExpressionType operatorType = shiftExpressionTypes.at(peek().getType());
            advance();
            auto rightResult = parseAdditiveExpression();

            Token lastToken = peek();

            currentNode = std::make_unique<BinaryExpressionNode>(operatorType, std::move(currentNode), std::move(rightResult.expression), firstToken, lastToken);
        }

        return ShiftExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    AdditiveExpressionParsingResult Parser::parseAdditiveExpression() {
        static const std::unordered_set<TokenType> additiveOperators = {
            TokenType::PLUS,
            TokenType::MINUS,
        };

        static const std::unordered_map<TokenType, BinaryExpressionType> additiveExpressionTypes = {
            { TokenType::PLUS, BinaryExpressionType::ADDITION },
            { TokenType::MINUS, BinaryExpressionType::SUBTRACTION },
        };

        Token firstToken = peek();

        auto leftResult = parseMultiplicativeExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(leftResult.expression);

        while (checkAny(additiveOperators)) {
            BinaryExpressionType operatorType = additiveExpressionTypes.at(peek().getType());
            advance();
            auto rightResult = parseMultiplicativeExpression();

            Token lastToken = peek();

            currentNode = std::make_unique<BinaryExpressionNode>(operatorType, std::move(currentNode), std::move(rightResult.expression), firstToken, lastToken);
        }

        return AdditiveExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    MultiplicativeExpressionParsingResult Parser::parseMultiplicativeExpression() {
        static const std::unordered_set<TokenType> multiplicativeOperators = {
            TokenType::ASTERISK,
            TokenType::SLASH,
            TokenType::DOUBLE_SLASH,
            TokenType::PERCENT,
        };

        static const std::unordered_map<TokenType, BinaryExpressionType> multiplicativeExpressionTypes = {
            { TokenType::ASTERISK, BinaryExpressionType::MULTIPLICATION },
            { TokenType::SLASH, BinaryExpressionType::DIVISION },
            { TokenType::DOUBLE_SLASH, BinaryExpressionType::INTEGER_DIVISION },
            { TokenType::PERCENT, BinaryExpressionType::MODULO },
        };

        Token firstToken = peek();

        auto leftResult = parseUnaryExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(leftResult.expression);

        while (checkAny(multiplicativeOperators)) {
            BinaryExpressionType operatorType = multiplicativeExpressionTypes.at(peek().getType());
            advance();
            auto rightResult = parseUnaryExpression();

            Token lastToken = peek();

            currentNode = std::make_unique<BinaryExpressionNode>(operatorType, std::move(currentNode), std::move(rightResult.expression), firstToken, lastToken);
        }

        return MultiplicativeExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    UnaryExpressionParsingResult Parser::parseUnaryExpression() {
        static const std::unordered_set<TokenType> unaryOperators = {
            TokenType::PLUS,
            TokenType::MINUS,
            TokenType::TILDE,
            TokenType::EXCLAMATION,
        };

        static const std::unordered_map<TokenType, UnaryExpressionType> unaryExpressionTypes = {
            { TokenType::PLUS, UnaryExpressionType::UNARY_PLUS },
            { TokenType::MINUS, UnaryExpressionType::UNARY_MINUS },
            { TokenType::TILDE, UnaryExpressionType::BITWISE_NOT },
            { TokenType::EXCLAMATION, UnaryExpressionType::LOGICAL_NOT },
        };

        Token firstToken = peek();

        if (checkAny(unaryOperators)) {
            UnaryExpressionType operatorType = unaryExpressionTypes.at(peek().getType());
            advance();
            auto operandResult = parseExponentialExpression();

            Token lastToken = peek();

            return UnaryExpressionParsingResult{
                .expression = std::make_unique<UnaryExpressionNode>(operatorType, std::move(operandResult.expression), firstToken, lastToken),
            };
        } else {
            auto result = parseExponentialExpression();

            return UnaryExpressionParsingResult{
                .expression = std::move(result.expression),
            };
        }
    }

    ExponentialExpressionParsingResult Parser::parseExponentialExpression() {
        Token firstToken = peek();

        auto leftResult = parsePostfixExpression();

        if (match(TokenType::DOUBLE_ASTERISK)) {
            auto rightResult = parseUnaryExpression();

            Token lastToken = peek();

            return ExponentialExpressionParsingResult{
                .expression =
                    std::make_unique<BinaryExpressionNode>(BinaryExpressionType::EXPONENT, std::move(leftResult.expression), std::move(rightResult.expression), firstToken, lastToken),
            };
        } else {
            return ExponentialExpressionParsingResult{
                .expression = std::move(leftResult.expression),
            };
        }
    }

    PostfixExpressionParsingResult Parser::parsePostfixExpression() {
        Token firstToken = peek();

        auto primaryResult = parsePrimaryExpression();
        std::unique_ptr<ExpressionNode> currentNode = std::move(primaryResult.expression);

        static const std::unordered_set<TokenType> postfixOperators = {
            TokenType::DOT,
            TokenType::QUESTION_DOT,
            TokenType::LEFT_PARENTHESIS,
            TokenType::LEFT_BRACKET,
        };

        while (checkAny(postfixOperators)) {
            if (match(TokenType::DOT)) {
                Token identifierFirstToken = peek();

                std::unique_ptr<IdentifierNode> name;
                if (!checkGeneralizedIdentifier()) {
                    throw SyntaxError("Expected generalized identifier after '.'", peek().getLine(), peek().getColumn());
                } else {
                    name = constructCurrentIdentifierNode();
                }

                Token lastToken = peek();

                std::unique_ptr<IdentifierExpressionNode> nameNode = std::make_unique<IdentifierExpressionNode>(std::move(name), identifierFirstToken, lastToken);

                currentNode = std::make_unique<MemberAccessExpressionNode>(MemberAccessExpressionType::DOT, std::move(currentNode), std::move(nameNode), firstToken, lastToken);
            } else if (match(TokenType::QUESTION_DOT)) {
                Token identifierFirstToken = peek();

                std::unique_ptr<IdentifierNode> name;
                if (!checkGeneralizedIdentifier()) {
                    throw SyntaxError("Expected generalized identifier after '?.'", peek().getLine(), peek().getColumn());
                } else {
                    name = constructCurrentIdentifierNode();
                }

                Token lastToken = peek();

                std::unique_ptr<IdentifierExpressionNode> nameNode = std::make_unique<IdentifierExpressionNode>(std::move(name), identifierFirstToken, lastToken);

                currentNode = std::make_unique<MemberAccessExpressionNode>(MemberAccessExpressionType::OPTIONAL_CHAINING, std::move(currentNode), std::move(nameNode), firstToken, lastToken);
            } else if (endsWithNewlineOrEOF) {
                break;
            } else if (match(TokenType::LEFT_PARENTHESIS)) {
                std::vector<std::unique_ptr<ExpressionNode>> arguments;
                std::optional<std::unique_ptr<ExpressionNode>> context = std::nullopt;

                if (!check(TokenType::RIGHT_PARENTHESIS)) {
                    auto argumentListResult = parseArgumentList();
                    arguments = std::move(argumentListResult.arguments);
                    context = std::move(argumentListResult.context);
                }

                if (!match(TokenType::RIGHT_PARENTHESIS)) {
                    throw SyntaxError("Expected ')' after argument list", peek().getLine(), peek().getColumn());
                }

                Token lastToken = peek();

                if (context.has_value()) {
                    currentNode = std::make_unique<FunctionCallExpressionNode>(std::move(currentNode), std::move(arguments), std::move(context.value()), firstToken, lastToken);
                } else {
                    currentNode = std::make_unique<FunctionCallExpressionNode>(std::move(currentNode), std::move(arguments), firstToken, lastToken);
                }
            } else if (match(TokenType::LEFT_BRACKET)) {
                auto indexResult = parseExpression();

                if (!match(TokenType::RIGHT_BRACKET)) {
                    throw SyntaxError("Expected ']' after index expression", peek().getLine(), peek().getColumn());
                }

                Token lastToken = peek();

                currentNode = std::make_unique<SubscriptExpressionNode>(std::move(currentNode), std::move(indexResult.expression), firstToken, lastToken);
            }
        }

        return PostfixExpressionParsingResult{
            .expression = std::move(currentNode),
        };
    }

    PrimaryExpressionParsingResult Parser::parsePrimaryExpression() {
        static const std::unordered_set<TokenType> literalStarters = {
            TokenType::NUMBER, TokenType::STRING, TokenType::CHAR, TokenType::TRUE, TokenType::FALSE, TokenType::LEFT_BRACKET, TokenType::LEFT_BRACE, TokenType::SELECTOR_PREFIX,
        };

        Token firstToken = peek();

        if (match(TokenType::LEFT_PARENTHESIS)) {
            auto expressionResult = parseExpression();

            if (!match(TokenType::RIGHT_PARENTHESIS)) {
                throw SyntaxError("Expected ')' after expression", peek().getLine(), peek().getColumn());
            }

            Token lastToken = peek();
            expressionResult.expression->resetPosition(firstToken, lastToken);

            return PrimaryExpressionParsingResult{
                .expression = std::move(expressionResult.expression),
            };
        } else if (match(TokenType::THIS)) {
            Token lastToken = peek();

            return PrimaryExpressionParsingResult{
                .expression = std::make_unique<ThisExpressionNode>(firstToken, lastToken),
            };
        } else if (match(TokenType::SUPER)) {
            Token lastToken = peek();

            return PrimaryExpressionParsingResult{
                .expression = std::make_unique<SuperExpressionNode>(firstToken, lastToken),
            };
        } else if (check(TokenType::IDENTIFIER)) {
            auto name = constructCurrentIdentifierNode();

            Token lastToken = peek();

            return PrimaryExpressionParsingResult{
                .expression = std::make_unique<IdentifierExpressionNode>(std::move(name), firstToken, lastToken),
            };
        } else if (checkAny(literalStarters)) {
            auto literalResult = parseLiteral();

            return PrimaryExpressionParsingResult{
                .expression = std::move(literalResult.expression),
            };
        } else {
            throw SyntaxError("Expected primary expression", peek().getLine(), peek().getColumn());
        }
    }

    LiteralParsingResult Parser::parseLiteral() {
        Token firstToken = peek();

        if (match(TokenType::NUMBER)) {
            Token lastToken = peek();

            SimpleLiteralExpressionType literalType = SimpleLiteralExpressionType::DECIMAL_INTEGER;

            if (firstToken.getValue().find('.') != std::string::npos || firstToken.getValue().find('e') != std::string::npos || firstToken.getValue().find('E') != std::string::npos) {
                if (firstToken.getValue().ends_with('f') || firstToken.getValue().ends_with('F')) {
                    literalType = SimpleLiteralExpressionType::DECIMAL_FLOAT;
                } else {
                    literalType = SimpleLiteralExpressionType::DECIMAL_DOUBLE;
                }
            } else if (firstToken.getValue().starts_with("0x")) {
                literalType = SimpleLiteralExpressionType::HEXADECIMAL;
            } else if (firstToken.getValue().starts_with("0b")) {
                literalType = SimpleLiteralExpressionType::BINARY;
            } else if (firstToken.getValue().starts_with("0o")) {
                literalType = SimpleLiteralExpressionType::OCTAL;
            } else if (firstToken.getValue().ends_with(('b')) || firstToken.getValue().ends_with(('B'))) {
                literalType = SimpleLiteralExpressionType::DECIMAL_BYTE;
            } else if (firstToken.getValue().ends_with(('s')) || firstToken.getValue().ends_with(('S'))) {
                literalType = SimpleLiteralExpressionType::DECIMAL_SHORT;
            } else if (firstToken.getValue().ends_with(('l')) || firstToken.getValue().ends_with(('L'))) {
                literalType = SimpleLiteralExpressionType::DECIMAL_LONG;
            }

            return LiteralParsingResult{
                .expression = std::make_unique<SimpleLiteralExpressionNode>(literalType, firstToken.getValue(), firstToken, lastToken),
            };
        } else if (match(TokenType::CHAR)) {
            Token lastToken = peek();

            return LiteralParsingResult{
                .expression = std::make_unique<SimpleLiteralExpressionNode>(
                    SimpleLiteralExpressionType::CHARACTER,
                    firstToken.getValue().substr(1, firstToken.getValue().length() - 2), // Remove the surrounding single quotes
                    firstToken,
                    lastToken
                ),
            };
        } else if (check(TokenType::STRING)) {
            auto result = parseString();

            return LiteralParsingResult{
                .expression = std::move(result.expression),
            };
        } else if (check(TokenType::TRUE) || check(TokenType::FALSE)) {
            auto result = parseBoolean();

            return LiteralParsingResult{
                .expression = std::move(result.expression),
            };
        } else if (check(TokenType::LEFT_BRACKET)) {
            auto result = parseListLikeLiteral();

            return LiteralParsingResult{
                .expression = std::move(result.expression),
            };
        } else if (check(TokenType::LEFT_BRACE)) {
            auto result = parseDictLiteral();

            return LiteralParsingResult{
                .expression = std::move(result.expression),
            };
        } else if (check(TokenType::SELECTOR_PREFIX)) {
            auto result = parseSelector();

            return LiteralParsingResult{
                .expression = std::move(result.expression),
            };
        } else {
            throw SyntaxError("Expected literal", peek().getLine(), peek().getColumn());
        }
    }

    StringParsingResult Parser::parseString() {
        Token firstToken = peek();
        std::vector<std::variant<std::string, std::unique_ptr<ExpressionNode>>> parts;

        if (!check(TokenType::STRING)) {
            throw SyntaxError("Expected string literal", peek().getLine(), peek().getColumn());
        }

        if (peek().getValue().starts_with("\"")) {
            std::string value(peek().getValue().substr(1, peek().getValue().length() - 2));
            parts.emplace_back(std::move(value));
            advance();

            Token lastToken = peek();

            return StringParsingResult{
                .expression = std::make_unique<StringLiteralExpressionNode>(StringLiteralExpressionType::STRING, std::move(parts), firstToken, lastToken),
            };
        } else if (peek().getValue().starts_with("f\"")) {
            std::string value(peek().getValue().substr(2));
            if (value.ends_with('\"') && !value.ends_with("\\\"")) {
                value.pop_back();
            }
            parts.emplace_back(std::move(value));
            advance();

            while (check(TokenType::INTERPOLATION_START) || check(TokenType::STRING)) {
                if (check(TokenType::STRING)) {
                    std::string value(peek().getValue());
                    if (value.ends_with('\"') && !value.ends_with("\\\"")) {
                        value.pop_back();
                    }
                    parts.emplace_back(std::move(value));
                    advance();
                } else if (check(TokenType::INTERPOLATION_START)) {
                    auto interpolationResult = parseInterpolation();
                    parts.emplace_back(std::move(interpolationResult.expression));
                } else {
                    throw SyntaxError("Expected string literal or interpolation in formatted string", peek().getLine(), peek().getColumn());
                }
            }

            Token lastToken = peek();

            return StringParsingResult{
                .expression = std::make_unique<StringLiteralExpressionNode>(StringLiteralExpressionType::FORMAT_STRING, std::move(parts), firstToken, lastToken),
            };

        } else if (peek().getValue().starts_with("r\"")) {
            std::string value(peek().getValue().substr(2, peek().getValue().length() - 3)); // Remove the r" prefix and the surrounding quotes
            parts.emplace_back(std::move(value));
            advance();

            Token lastToken = peek();

            return StringParsingResult{
                .expression = std::make_unique<StringLiteralExpressionNode>(StringLiteralExpressionType::RAW_STRING, std::move(parts), firstToken, lastToken),
            };
        } else {
            throw SyntaxError("Invalid string literal", peek().getLine(), peek().getColumn());
        }
    }

    BooleanParsingResult Parser::parseBoolean() {
        Token firstToken = peek();

        if (match(TokenType::TRUE) || match(TokenType::FALSE)) {
            bool value = firstToken.getType() == TokenType::TRUE;

            Token lastToken = peek();

            return BooleanParsingResult{
                .expression = std::make_unique<SimpleLiteralExpressionNode>(SimpleLiteralExpressionType::BOOLEAN, value ? "true" : "false", firstToken, lastToken),
            };
        } else {
            throw SyntaxError("Expected boolean literal", peek().getLine(), peek().getColumn());
        }
    }

    ListLikeLiteralParsingResult Parser::parseListLikeLiteral() {
        Token firstToken = peek();

        ListLikeLiteralExpressionType literalType = ListLikeLiteralExpressionType::LIST;

        if (!match(TokenType::LEFT_BRACKET)) {
            throw SyntaxError("Expected '[' to start list literal", peek().getLine(), peek().getColumn());
        }

        std::vector<std::unique_ptr<ExpressionNode>> elements;
        bool comma = false;

        if (check(TokenType::IDENTIFIER)) {
            auto name = constructCurrentIdentifierNode();

            if ((name->getIdentifierString() == "B" || name->getIdentifierString() == "I" || name->getIdentifierString() == "L")) {
                if (!match(TokenType::SEMICOLON)) {
                    elements.push_back(std::make_unique<IdentifierExpressionNode>(std::move(name), firstToken, peek()));
                    comma = match(TokenType::COMMA);
                } else {
                    if (name->getIdentifierString() == "B") {
                        literalType = ListLikeLiteralExpressionType::BYTE_SNBT_ARRAY;
                    } else if (name->getIdentifierString() == "I") {
                        literalType = ListLikeLiteralExpressionType::INT_SNBT_ARRAY;
                    } else if (name->getIdentifierString() == "L") {
                        literalType = ListLikeLiteralExpressionType::LONG_SNBT_ARRAY;
                    }
                }
            } else {
                elements.push_back(std::make_unique<IdentifierExpressionNode>(std::move(name), firstToken, peek()));
                comma = match(TokenType::COMMA);
            }
        }

        if (comma || !check(TokenType::RIGHT_BRACKET)) {
            do {
                auto expressionResult = parseExpression();
                elements.push_back(std::move(expressionResult.expression));
            } while (match(TokenType::COMMA));
        }

        if (!match(TokenType::RIGHT_BRACKET)) {
            throw SyntaxError("Expected ']' to end list literal", peek().getLine(), peek().getColumn());
        }

        Token lastToken = peek();

        return ListLikeLiteralParsingResult{
            .expression = std::make_unique<ListLikeLiteralExpressionNode>(literalType, std::move(elements), firstToken, lastToken),
        };
    }

    DictLiteralParsingResult Parser::parseDictLiteral() {
        Token firstToken = peek();

        if (!match(TokenType::LEFT_BRACE)) {
            throw SyntaxError("Expected '{' to start dict literal", peek().getLine(), peek().getColumn());
        }

        std::unordered_map<std::string, std::unique_ptr<ExpressionNode>> entries;

        if (!check(TokenType::RIGHT_BRACE)) {
            do {
                auto entryResult = parseDictEntry();
                entries.emplace(std::move(entryResult.key), std::move(entryResult.value));
            } while (match(TokenType::COMMA));
        }

        if (!match(TokenType::RIGHT_BRACE)) {
            throw SyntaxError("Expected '}' to end dict literal", peek().getLine(), peek().getColumn());
        }

        Token lastToken = peek();

        return DictLiteralParsingResult{
            .expression = std::make_unique<DictLiteralExpressionNode>(std::move(entries), firstToken, lastToken),
        };
    }

    SelectorParsingResult Parser::parseSelector() {
        Token firstToken = peek();

        SelectorLiteralExpressionType literalType = SelectorLiteralExpressionType::NEAREST_PLAYER;
        std::unordered_map<std::string, std::unique_ptr<ExpressionNode>> arguments;

        if (!check(TokenType::SELECTOR_PREFIX)) {
            throw SyntaxError("Expected '@' to start selector", peek().getLine(), peek().getColumn());
        }

        if (peek().getValue() == "@p") {
            literalType = SelectorLiteralExpressionType::NEAREST_PLAYER;
        } else if (peek().getValue() == "@r") {
            literalType = SelectorLiteralExpressionType::RANDOM_PLAYER;
        } else if (peek().getValue() == "@a") {
            literalType = SelectorLiteralExpressionType::ALL_PLAYERS;
        } else if (peek().getValue() == "@e") {
            literalType = SelectorLiteralExpressionType::ALL_ENTITIES;
        } else if (peek().getValue() == "@s") {
            literalType = SelectorLiteralExpressionType::CURRENT_EXECUTOR;
        } else if (peek().getValue() == "@n") {
            literalType = SelectorLiteralExpressionType::NEAREST_ENTITY;
        } else {
            throw SyntaxError("Invalid selector type", peek().getLine(), peek().getColumn());
        }

        advance();

        if (match(TokenType::LEFT_BRACKET)) {
            if (!check(TokenType::RIGHT_BRACKET)) {
                auto argumentListResult = parseSelectorArgumentList();
                arguments = std::move(argumentListResult.arguments);
            }

            if (!match(TokenType::RIGHT_BRACKET)) {
                throw SyntaxError("Expected ']' to end selector arguments", peek().getLine(), peek().getColumn());
            }
        }

        Token lastToken = peek();

        return SelectorParsingResult{
            .expression = std::make_unique<SelectorLiteralExpressionNode>(literalType, std::move(arguments), firstToken, lastToken),
        };
    }

    ArgumentListParsingResult Parser::parseArgumentList() {
        std::vector<std::unique_ptr<ExpressionNode>> arguments;
        std::optional<std::unique_ptr<ExpressionNode>> context = std::nullopt;

        do {
            if (match(TokenType::CONTEXT)) {
                if (!match(TokenType::EQUAL)) {
                    throw SyntaxError("Expected '=' after 'context'", peek().getLine(), peek().getColumn());
                }

                auto contextResult = parseExpression();
                context = std::make_optional(std::move(contextResult.expression));

                return ArgumentListParsingResult{
                    .arguments = std::move(arguments),
                    .context = std::move(context),
                };
            }

            auto expressionResult = parseExpression();
            arguments.push_back(std::move(expressionResult.expression));
        } while (match(TokenType::COMMA));

        return ArgumentListParsingResult{
            .arguments = std::move(arguments),
            .context = std::move(context),
        };
    }

    InterpolationParsingResult Parser::parseInterpolation() {
        if (!match(TokenType::INTERPOLATION_START)) {
            throw SyntaxError("Expected '$(' to start interpolation", peek().getLine(), peek().getColumn());
        }

        auto expressionResult = parseExpression();

        if (!match(TokenType::RIGHT_PARENTHESIS)) {
            throw SyntaxError("Expected ')' to end interpolation", peek().getLine(), peek().getColumn());
        }

        return InterpolationParsingResult{
            .expression = std::move(expressionResult.expression),
        };
    }

    DictEntryParsingResult Parser::parseDictEntry() {
        std::string key;
        if (check(TokenType::STRING)) {
            if (!peek().getValue().starts_with("\"")) {
                throw SyntaxError("Expected simple string literal for dict key", peek().getLine(), peek().getColumn());
            }

            key = peek().getValue().substr(1, peek().getValue().length() - 2); // Remove the surrounding quotes
            advance();
        } else if (checkGeneralizedIdentifier()) {
            key = peek().getValue();
            advance();
        } else {
            throw SyntaxError("Expected string literal or identifier for dict key", peek().getLine(), peek().getColumn());
        }

        if (!match(TokenType::COLON)) {
            throw SyntaxError("Expected ':' after dict key", peek().getLine(), peek().getColumn());
        }

        auto valueResult = parseExpression();

        return DictEntryParsingResult{
            .key = std::move(key),
            .value = std::move(valueResult.expression),
        };
    }

    SelectorArgumentListParsingResult Parser::parseSelectorArgumentList() {
        std::unordered_map<std::string, std::unique_ptr<ExpressionNode>> arguments;

        do {
            auto argumentResult = parseSelectorArgument();
            arguments.emplace(std::move(argumentResult.key), std::move(argumentResult.value));
        } while (match(TokenType::COMMA));

        return SelectorArgumentListParsingResult{
            .arguments = std::move(arguments),
        };
    }

    SelectorArgumentParsingResult Parser::parseSelectorArgument() {
        std::string key;

        if (!checkGeneralizedIdentifier()) {
            throw SyntaxError("Expected identifier for selector argument key", peek().getLine(), peek().getColumn());
        } else {
            key = peek().getValue();
            advance();
        }

        if (!match(TokenType::EQUAL)) {
            throw SyntaxError("Expected '=' after selector argument key", peek().getLine(), peek().getColumn());
        }

        auto valueResult = parseExpression();

        return SelectorArgumentParsingResult{
            .key = std::move(key),
            .value = std::move(valueResult.expression),
        };
    }

    StatementParsingResult Parser::parseStatement() {
        static const std::unordered_set<TokenType> expressionStarters = {
            TokenType::IDENTIFIER, TokenType::NUMBER,      TokenType::STRING,           TokenType::CHAR,         TokenType::TRUE, TokenType::FALSE,
            TokenType::THIS,       TokenType::SUPER,       TokenType::LEFT_PARENTHESIS, TokenType::LEFT_BRACKET, TokenType::PLUS, TokenType::MINUS,
            TokenType::TILDE,      TokenType::EXCLAMATION, TokenType::SELECTOR_PREFIX,
        };

        static const std::unordered_set<TokenType> variableDeclarationStarters = {
            TokenType::VAR,
            TokenType::LET,
            TokenType::CONST,
        };

        static const std::unordered_set<TokenType> controlFlowStarters = {
            TokenType::IF, TokenType::SWITCH, TokenType::LABEL, TokenType::WHILE, TokenType::FOR, TokenType::RETURN, TokenType::BREAK, TokenType::CONTINUE, TokenType::RELOAD,
        };

        std::unique_ptr<StatementNode> statement;

        if (checkAny(expressionStarters)) {
            auto result = parseExpressionStatement();

            statement = std::move(result.statement);
        } else if (checkAny(variableDeclarationStarters)) {
            auto result = parseVariableDeclarationStatement();

            statement = std::move(result.statement);
        } else if (check(TokenType::LEFT_BRACE)) {
            auto result = parseBlockStatement();

            statement = std::move(result.statement);
        } else if (checkAny(controlFlowStarters)) {
            auto result = parseControlFlowStatement();

            statement = std::move(result.statement);
        } else {
            throw SyntaxError("Expected statement", peek().getLine(), peek().getColumn());
        }

        if (!(endsWithNewlineOrEOF || peek().getType() == TokenType::RIGHT_BRACE)) {
            throw SyntaxError("Expected newline after statement", peek().getLine(), peek().getColumn());
        }

        return StatementParsingResult{
            .statement = std::move(statement),
        };
    }

    ExpressionStatementParsingResult Parser::parseExpressionStatement() {
        Token firstToken = peek();

        auto result = parseExpression();

        Token lastToken = peek();

        return ExpressionStatementParsingResult{
            .statement = std::make_unique<ExpressionStatementNode>(std::move(result.expression), firstToken, lastToken),
        };
    }

    VariableDeclarationStatementParsingResult Parser::parseVariableDeclarationStatement() {
        Token firstToken = peek();

        VariableDeclarationParsingContext context{
            .position = VariableDeclarationParsingContext::Position::STATEMENT,
            .hasMetadata = false,
            .metadataTerms = {},
        };

        auto result = parseVariableDeclaration(std::move(context));

        Token lastToken = peek();

        return VariableDeclarationStatementParsingResult{
            .statement = std::make_unique<VariableDeclarationStatementNode>(std::move(result.declaration), firstToken, lastToken),
        };
    }

    BlockStatementParsingResult Parser::parseBlockStatement() {
        Token firstToken = peek();

        if (!match(TokenType::LEFT_BRACE)) {
            throw SyntaxError("Expected '{' to start block statement", peek().getLine(), peek().getColumn());
        }

        std::vector<std::unique_ptr<StatementNode>> statements;

        while (!check(TokenType::RIGHT_BRACE)) {
            auto statementResult = parseStatement();
            statements.push_back(std::move(statementResult.statement));
        }

        if (!match(TokenType::RIGHT_BRACE)) {
            throw SyntaxError("Expected '}' to end block statement", peek().getLine(), peek().getColumn());
        }

        Token lastToken = peek();

        return BlockStatementParsingResult{
            .statement = std::make_unique<BlockStatementNode>(std::move(statements), firstToken, lastToken),
        };
    }

    ControlFlowStatementParsingResult Parser::parseControlFlowStatement() {
        if (check(TokenType::IF)) {
            auto result = parseIfStatement();

            return ControlFlowStatementParsingResult{
                .statement = std::move(result.statement),
            };
        } else if (check(TokenType::SWITCH)) {
            auto result = parseSwitchStatement();

            return ControlFlowStatementParsingResult{
                .statement = std::move(result.statement),
            };
        } else if (match(TokenType::LABEL)) {
            std::optional<std::unique_ptr<IdentifierNode>> label;

            if (!check(TokenType::IDENTIFIER)) {
                throw SyntaxError("Expected identifier after 'label'", peek().getLine(), peek().getColumn());
            } else {
                label = std::make_optional(constructCurrentIdentifierNode());
            }

            if (check(TokenType::WHILE)) {
                WhileStatementParsingContext context{
                    .label = std::move(label),
                };

                auto result = parseWhileStatement(std::move(context));

                return ControlFlowStatementParsingResult{
                    .statement = std::move(result.statement),
                };
            } else if (check(TokenType::FOR)) {
                ForStatementParsingContext context{
                    .label = std::move(label),
                };

                auto result = parseForStatement(std::move(context));

                return ControlFlowStatementParsingResult{
                    .statement = std::move(result.statement),
                };
            } else {
                throw SyntaxError("Expected 'while' or 'for' after label declaration", peek().getLine(), peek().getColumn());
            }
        } else if (check(TokenType::WHILE)) {
            auto result = parseWhileStatement({ std::nullopt });

            return ControlFlowStatementParsingResult{
                .statement = std::move(result.statement),
            };
        } else if (check(TokenType::FOR)) {
            auto result = parseForStatement({ std::nullopt });

            return ControlFlowStatementParsingResult{
                .statement = std::move(result.statement),
            };
        } else if (check(TokenType::RETURN)) {
            auto result = parseReturnStatement();

            return ControlFlowStatementParsingResult{
                .statement = std::move(result.statement),
            };
        } else if (check(TokenType::BREAK)) {
            auto result = parseBreakStatement();

            return ControlFlowStatementParsingResult{
                .statement = std::move(result.statement),
            };
        } else if (check(TokenType::CONTINUE)) {
            auto result = parseContinueStatement();

            return ControlFlowStatementParsingResult{
                .statement = std::move(result.statement),
            };
        } else if (check(TokenType::RELOAD)) {
            auto result = parseReloadStatement();

            return ControlFlowStatementParsingResult{
                .statement = std::move(result.statement),
            };
        } else {
            throw SyntaxError("Expected control flow statement", peek().getLine(), peek().getColumn());
        }
    }

    IfStatementParsingResult Parser::parseIfStatement() {
        Token firstToken = peek();

        if (!match(TokenType::IF)) {
            throw SyntaxError("Expected 'if' to start if statement", peek().getLine(), peek().getColumn());
        }

        if (!match(TokenType::LEFT_PARENTHESIS)) {
            throw SyntaxError("Expected '(' after 'if'", peek().getLine(), peek().getColumn());
        }

        auto conditionResult = parseExpression();

        if (!match(TokenType::RIGHT_PARENTHESIS)) {
            throw SyntaxError("Expected ')' after if condition", peek().getLine(), peek().getColumn());
        }

        auto thenBranchResult = parseStatement();

        std::optional<std::unique_ptr<StatementNode>> elseBranch = std::nullopt;

        if (match(TokenType::ELSE)) {
            auto elseBranchResult = parseStatement();
            elseBranch = std::move(elseBranchResult.statement);
        }

        Token lastToken = peek();

        return IfStatementParsingResult{
            .statement = std::make_unique<IfStatementNode>(std::move(conditionResult.expression), std::move(thenBranchResult.statement), std::move(elseBranch), firstToken, lastToken),
        };
    }

    SwitchStatementParsingResult Parser::parseSwitchStatement() {
        Token firstToken = peek();

        bool hasSwitchCases = false;
        SwitchStatementType switchType = SwitchStatementType::LITERAL_MATCH;
        std::vector<SwitchStatementItem::LiteralMatchItem> literalMatchItems;
        std::vector<SwitchStatementItem::TypeMatchItem> typeMatchItems;
        std::optional<std::unique_ptr<StatementNode>> defaultCase = std::nullopt;

        if (!match(TokenType::SWITCH)) {
            throw SyntaxError("Expected 'switch' to start switch statement", peek().getLine(), peek().getColumn());
        }

        if (!match(TokenType::LEFT_PARENTHESIS)) {
            throw SyntaxError("Expected '(' after 'switch'", peek().getLine(), peek().getColumn());
        }

        auto expressionResult = parseExpression();

        if (!match(TokenType::RIGHT_PARENTHESIS)) {
            throw SyntaxError("Expected ')' after switch expression", peek().getLine(), peek().getColumn());
        }

        if (!match(TokenType::LEFT_BRACE)) {
            throw SyntaxError("Expected '{' to start switch body", peek().getLine(), peek().getColumn());
        }

        while (check(TokenType::CASE)) {
            unsigned int caseBeginLine = peek().getLine();
            unsigned int caseBeginColumn = peek().getColumn();
            auto caseResult = parseSwitchCase();

            if (hasSwitchCases && caseResult.kind != switchType) {
                throw SyntaxError("Cannot mix different types of switch cases in the same switch statement", caseBeginLine, caseBeginColumn);
            }

            if (!hasSwitchCases) {
                switchType = caseResult.kind;
                hasSwitchCases = true;
            }

            if (!match(TokenType::ARROW)) {
                throw SyntaxError("Expected '->' after case label", peek().getLine(), peek().getColumn());
            }

            auto caseBodyResult = parseStatement();

            if (caseResult.kind == SwitchStatementType::LITERAL_MATCH) {
                literalMatchItems.push_back({ std::move(caseResult.literal.value()), std::move(caseBodyResult.statement) });
            } else if (caseResult.kind == SwitchStatementType::TYPE_MATCH) {
                typeMatchItems.push_back({ std::move(caseResult.type.value()), std::move(caseBodyResult.statement) });
            }
        }

        if (match(TokenType::DEFAULT)) {
            if (!match(TokenType::ARROW)) {
                throw SyntaxError("Expected '->' after 'default'", peek().getLine(), peek().getColumn());
            }

            auto defaultBodyResult = parseStatement();

            defaultCase = std::make_optional<std::unique_ptr<StatementNode>>(std::move(defaultBodyResult.statement));
        }

        if (!match(TokenType::RIGHT_BRACE)) {
            throw SyntaxError("Expected '}' to end switch body", peek().getLine(), peek().getColumn());
        }

        Token lastToken = peek();

        if (switchType == SwitchStatementType::LITERAL_MATCH) {
            return SwitchStatementParsingResult{
                .statement = std::make_unique<SwitchStatementNode>(std::move(expressionResult.expression), std::move(literalMatchItems), std::move(defaultCase), firstToken, lastToken),
            };
        } else {
            return SwitchStatementParsingResult{
                .statement = std::make_unique<SwitchStatementNode>(std::move(expressionResult.expression), std::move(typeMatchItems), std::move(defaultCase), firstToken, lastToken),
            };
        }
    }

    WhileStatementParsingResult Parser::parseWhileStatement(WhileStatementParsingContext context) {
        Token firstToken = peek();

        if (!match(TokenType::WHILE)) {
            throw SyntaxError("Expected 'while' to start while statement", peek().getLine(), peek().getColumn());
        }

        if (!match(TokenType::LEFT_PARENTHESIS)) {
            throw SyntaxError("Expected '(' after 'while'", peek().getLine(), peek().getColumn());
        }

        auto conditionResult = parseExpression();

        if (!match(TokenType::RIGHT_PARENTHESIS)) {
            throw SyntaxError("Expected ')' after while condition", peek().getLine(), peek().getColumn());
        }

        auto bodyResult = parseStatement();

        Token lastToken = peek();

        if (context.label.has_value()) {
            return WhileStatementParsingResult{
                .statement =
                    std::make_unique<WhileStatementNode>(std::move(context.label.value()), std::move(conditionResult.expression), std::move(bodyResult.statement), firstToken, lastToken),
            };
        } else {
            return WhileStatementParsingResult{
                .statement = std::make_unique<WhileStatementNode>(std::move(conditionResult.expression), std::move(bodyResult.statement), firstToken, lastToken),
            };
        }
    }

    ForStatementParsingResult Parser::parseForStatement(ForStatementParsingContext context) {
        Token firstToken = peek();

        std::unique_ptr<ValueDeclarationNode> loopVariable;
        std::unique_ptr<ExpressionNode> iterableExpression;
        std::unique_ptr<StatementNode> body;

        if (!match(TokenType::FOR)) {
            throw SyntaxError("Expected 'for' to start for statement", peek().getLine(), peek().getColumn());
        }

        if (!match(TokenType::LEFT_PARENTHESIS)) {
            throw SyntaxError("Expected '(' after 'for'", peek().getLine(), peek().getColumn());
        }

        Token variableFirstToken = peek();
        VariableDeclarationPrimaryParsingContext variableContext{
            .kind = ValueDeclarationType::Kind::LOOP_VARIABLE,
        };
        auto variableDeclarationPrimaryResult = parseVariableDeclarationPrimary(std::move(variableContext));
        Token variableLastToken = peek();
        loopVariable = std::make_unique<ValueDeclarationNode>(
            variableDeclarationPrimaryResult.kind,
            ValueDeclarationType::Context::BLOCK,
            ValueDeclarationType::AccessModifier::PUBLIC,
            std::move(variableDeclarationPrimaryResult.name),
            std::move(variableDeclarationPrimaryResult.type),
            std::nullopt,
            variableFirstToken,
            variableLastToken
        );

        if (!match(TokenType::IN)) {
            throw SyntaxError("Expected 'in' after loop variable declaration", peek().getLine(), peek().getColumn());
        }

        auto iterableExpressionResult = parseExpression();
        iterableExpression = std::move(iterableExpressionResult.expression);

        if (!match(TokenType::RIGHT_PARENTHESIS)) {
            throw SyntaxError("Expected ')' after for statement", peek().getLine(), peek().getColumn());
        }

        auto bodyResult = parseStatement();
        body = std::move(bodyResult.statement);

        Token lastToken = peek();

        if (context.label.has_value()) {
            return ForStatementParsingResult{
                .statement =
                    std::make_unique<ForStatementNode>(std::move(context.label.value()), std::move(loopVariable), std::move(iterableExpression), std::move(body), firstToken, lastToken),
            };
        } else {
            return ForStatementParsingResult{
                .statement = std::make_unique<ForStatementNode>(std::move(loopVariable), std::move(iterableExpression), std::move(body), firstToken, lastToken),
            };
        }
    }

    ReturnStatementParsingResult Parser::parseReturnStatement() {
        Token firstToken = peek();

        if (!match(TokenType::RETURN)) {
            throw SyntaxError("Expected 'return' to start return statement", peek().getLine(), peek().getColumn());
        }

        std::optional<std::unique_ptr<ExpressionNode>> returnValue = std::nullopt;

        if (!(endsWithNewlineOrEOF || peek().getType() == TokenType::RIGHT_BRACE)) {
            auto returnValueResult = parseExpression();
            returnValue = std::move(returnValueResult.expression);
        }

        Token lastToken = peek();

        if (returnValue.has_value()) {
            return ReturnStatementParsingResult{
                .statement = std::make_unique<ReturnStatementNode>(std::move(returnValue.value()), firstToken, lastToken),
            };
        } else {
            return ReturnStatementParsingResult{
                .statement = std::make_unique<ReturnStatementNode>(firstToken, lastToken),
            };
        }
    }

    BreakStatementParsingResult Parser::parseBreakStatement() {
        Token firstToken = peek();

        if (!match(TokenType::BREAK)) {
            throw SyntaxError("Expected 'break' to start break statement", peek().getLine(), peek().getColumn());
        }

        std::optional<std::unique_ptr<IdentifierNode>> label = std::nullopt;

        if (!(endsWithNewlineOrEOF || peek().getType() == TokenType::RIGHT_BRACE)) {
            if (!check(TokenType::IDENTIFIER)) {
                throw SyntaxError("Expected identifier after 'break'", peek().getLine(), peek().getColumn());
            } else {
                label = std::make_optional(constructCurrentIdentifierNode());
            }
        }

        Token lastToken = peek();

        if (label.has_value()) {
            return BreakStatementParsingResult{
                .statement = std::make_unique<BreakStatementNode>(std::move(label.value()), firstToken, lastToken),
            };
        } else {
            return BreakStatementParsingResult{
                .statement = std::make_unique<BreakStatementNode>(firstToken, lastToken),
            };
        }
    }

    ContinueStatementParsingResult Parser::parseContinueStatement() {
        Token firstToken = peek();

        if (!match(TokenType::CONTINUE)) {
            throw SyntaxError("Expected 'continue' to start continue statement", peek().getLine(), peek().getColumn());
        }

        std::optional<std::unique_ptr<IdentifierNode>> label = std::nullopt;

        if (!(endsWithNewlineOrEOF || peek().getType() == TokenType::RIGHT_BRACE)) {
            if (!check(TokenType::IDENTIFIER)) {
                throw SyntaxError("Expected identifier after 'continue'", peek().getLine(), peek().getColumn());
            } else {
                label = std::make_optional(constructCurrentIdentifierNode());
            }
        }

        Token lastToken = peek();

        if (label.has_value()) {
            return ContinueStatementParsingResult{
                .statement = std::make_unique<ContinueStatementNode>(std::move(label.value()), firstToken, lastToken),
            };
        } else {
            return ContinueStatementParsingResult{
                .statement = std::make_unique<ContinueStatementNode>(firstToken, lastToken),
            };
        }
    }

    ReloadStatementParsingResult Parser::parseReloadStatement() {
        Token firstToken = peek();

        if (!match(TokenType::RELOAD)) {
            throw SyntaxError("Expected 'reload' to start reload statement", peek().getLine(), peek().getColumn());
        }

        Token lastToken = peek();

        return ReloadStatementParsingResult{
            .statement = std::make_unique<ReloadStatementNode>(firstToken, lastToken),
        };
    }

    SwitchCaseParsingResult Parser::parseSwitchCase() {
        if (!match(TokenType::CASE)) {
            throw SyntaxError("Expected 'case' to start switch case", peek().getLine(), peek().getColumn());
        }

        static const std::unordered_set<TokenType> literalStarters = {
            TokenType::PLUS, TokenType::MINUS, TokenType::TILDE, TokenType::EXCLAMATION, TokenType::NUMBER, TokenType::STRING, TokenType::CHAR, TokenType::TRUE, TokenType::FALSE,
        };

        if (checkAny(literalStarters)) {
            auto literalExpressionResult = parseExpression();

            return SwitchCaseParsingResult{
                .kind = SwitchStatementType::LITERAL_MATCH,
                .literal = std::move(literalExpressionResult.expression),
            };
        } else if (check(TokenType::IDENTIFIER)) {
            auto typeResult = parseType();

            std::optional<std::unique_ptr<ExpressionNode>> guardExpression = std::nullopt;
            if (match(TokenType::WHEN)) {
                auto guardResult = parseExpression();
                guardExpression = std::move(guardResult.expression);
            }

            return SwitchCaseParsingResult{
                .kind = SwitchStatementType::TYPE_MATCH,
                .type = std::move(typeResult.type),
                .guardExpression = std::move(guardExpression),
            };
        } else {
            throw SyntaxError("Expected literal or type identifier after 'case'", peek().getLine(), peek().getColumn());
        }
    }
} // namespace vnlc
