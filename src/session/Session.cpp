#include "Session.hpp"
#include "collector/Collector.hpp"
#include "config/Config.hpp"
#include "lexer/Lexer.hpp"
#include "log/Logger.hpp"
#include "parser/Parser.hpp"
#include "semantic/SemanticAnalyzer.hpp"
#include "semantic/SemanticResult.hpp"
#include <fstream>

namespace vnlc {
    Session::Session(Config&& config) : config(config) {}

    void Session::run() {
        VNLC_LOG_INFO("Session started.");

        std::ifstream input(config.inputFilePath);

        Lexer collectorLexer(input);
        Collector collector(std::move(collectorLexer));
        collectionResult = collector.collect(config);

        input.clear();
        input.seekg(0);

        Lexer lexer(input);
        Parser parser(std::move(lexer));
        ast = parser.parse(config);

        SemanticAnalyzer semanticAnalyzer(*ast, collectionResult->getImports());
        SemanticResult semantic = semanticAnalyzer.analyze(config);
    }
} // namespace vnlc
