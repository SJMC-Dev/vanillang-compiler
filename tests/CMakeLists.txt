FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(
    vnlctest_lexer
    ${PROJECT_SOURCE_DIR}/src/lexer/Lexer.cpp
    ${PROJECT_SOURCE_DIR}/src/token/Token.cpp
    LexerTest.cpp
)

file(GLOB_RECURSE AST_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/ast/*.cpp
)

file(GLOB_RECURSE SEMANTIC_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/semantic/*.cpp
)

file(GLOB_RECURSE TYPE_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/type/*.cpp
)

file(GLOB_RECURSE SCOPE_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/scope/*.cpp
)

file(GLOB_RECURSE SYMBOL_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/symbol/*.cpp
)

file(GLOB_RECURSE METADATA_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/metadata/*.cpp
)

file(GLOB_RECURSE VNI_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/vni/*.cpp
)

file(GLOB_RECURSE VNI_IMPORT_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/vni/import/*.cpp
)

file(GLOB_RECURSE OUTLINE_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/outline/*.cpp
)

file(GLOB_RECURSE COLLECTOR_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/collector/*.cpp
)

add_executable(
    vnlctest_parser
    ${PROJECT_SOURCE_DIR}/src/diagnostic/Diagnostic.cpp
    ${PROJECT_SOURCE_DIR}/src/lexer/Lexer.cpp
    ${PROJECT_SOURCE_DIR}/src/token/Token.cpp
    ${PROJECT_SOURCE_DIR}/src/parser/ParseResult.cpp
    ${PROJECT_SOURCE_DIR}/src/parser/Parser.cpp
    ${PROJECT_SOURCE_DIR}/src/util/TokenKindUtil.cpp
    ${AST_FILES}
    ${VNI_IMPORT_FILES}
    ${OUTLINE_FILES}
    ${COLLECTOR_FILES}
    ParserTest.cpp
)

add_executable(
    vnlctest_collector
    ${PROJECT_SOURCE_DIR}/src/diagnostic/Diagnostic.cpp
    ${PROJECT_SOURCE_DIR}/src/lexer/Lexer.cpp
    ${PROJECT_SOURCE_DIR}/src/token/Token.cpp
    ${VNI_IMPORT_FILES}
    ${OUTLINE_FILES}
    ${COLLECTOR_FILES}
    CollectorTest.cpp
)

add_executable(
    vnlctest_semantic
    ${PROJECT_SOURCE_DIR}/src/diagnostic/Diagnostic.cpp
    ${PROJECT_SOURCE_DIR}/src/lexer/Lexer.cpp
    ${PROJECT_SOURCE_DIR}/src/token/Token.cpp
    ${PROJECT_SOURCE_DIR}/src/parser/ParseResult.cpp
    ${PROJECT_SOURCE_DIR}/src/parser/Parser.cpp
    ${PROJECT_SOURCE_DIR}/src/util/TokenKindUtil.cpp
    ${AST_FILES}
    ${SEMANTIC_FILES}
    ${SCOPE_FILES}
    ${SYMBOL_FILES}
    ${METADATA_FILES}
    ${TYPE_FILES}
    ${VNI_IMPORT_FILES}
    ${OUTLINE_FILES}
    ${COLLECTOR_FILES}
    SemanticAnalyzerTest.cpp
)

add_executable(
    vnlctest_vni
    ${PROJECT_SOURCE_DIR}/src/token/Token.cpp
    ${PROJECT_SOURCE_DIR}/src/semantic/SemanticResult.cpp
    ${AST_FILES}
    ${TYPE_FILES}
    ${VNI_FILES}
    ModuleInterfaceTest.cpp
)

target_include_directories(
    vnlctest_lexer
    PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

target_include_directories(
    vnlctest_parser
    PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

target_include_directories(
    vnlctest_collector
    PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

target_include_directories(
    vnlctest_semantic
    PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

target_include_directories(
    vnlctest_vni
    PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

target_link_libraries(
    vnlctest_lexer
    PRIVATE
    GTest::gtest_main
    fmt::fmt
    nlohmann_json::nlohmann_json
    CLI11::CLI11
)

target_link_libraries(
    vnlctest_parser
    PRIVATE
    GTest::gtest_main
    fmt::fmt
    nlohmann_json::nlohmann_json
    CLI11::CLI11
)

target_link_libraries(
    vnlctest_collector
    PRIVATE
    GTest::gtest_main
    fmt::fmt
    nlohmann_json::nlohmann_json
    CLI11::CLI11
)

target_link_libraries(
    vnlctest_semantic
    PRIVATE
    GTest::gtest_main
    fmt::fmt
    nlohmann_json::nlohmann_json
    CLI11::CLI11
)

target_link_libraries(
    vnlctest_vni
    PRIVATE
    GTest::gtest_main
    fmt::fmt
    nlohmann_json::nlohmann_json
    CLI11::CLI11
)

target_compile_definitions(
    vnlctest_parser
    PRIVATE
    VNLC_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

include(GoogleTest)
gtest_discover_tests(vnlctest_lexer)
gtest_discover_tests(vnlctest_parser)
gtest_discover_tests(vnlctest_collector)
gtest_discover_tests(vnlctest_semantic)
gtest_discover_tests(vnlctest_vni)
