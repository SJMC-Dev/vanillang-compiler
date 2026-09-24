#include "Session.hpp"
#include "collector/Collector.hpp"
#include "config/Config.hpp"
#include "lexer/Lexer.hpp"
#include "log/Logger.hpp"
#include "parser/Parser.hpp"
#include <fstream>

namespace vnlc {
    Session::Session(Config&& config) : config(config), imports(), collectionErrors(), ast(nullptr) {}

    void Session::run() {
        VNLC_LOG_INFO("Session started.");

        std::ifstream input(config.inputFilePath);

        Lexer collectorLexer(input);
        Collector collector(std::move(collectorLexer));
        auto collection = collector.collect(config);
        imports = collection.takeImports();
        collectionErrors = collection.takeErrors();

        input.clear();
        input.seekg(0);

        Lexer lexer(input);
        Parser parser(std::move(lexer));
        ast = parser.parse(config);
    }
} // namespace vnlc
