FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(
    vnlctest_lexer
    ${PROJECT_SOURCE_DIR}/src/lexer/VnlcLexer.cpp
    ${PROJECT_SOURCE_DIR}/src/token/VnlcToken.cpp
    VnlcLexerTest.cpp
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

file(GLOB_RECURSE VNI_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/vni/*.cpp
)

add_executable(
    vnlctest_parser
    ${PROJECT_SOURCE_DIR}/src/lexer/VnlcLexer.cpp
    ${PROJECT_SOURCE_DIR}/src/token/VnlcToken.cpp
    ${PROJECT_SOURCE_DIR}/src/parser/VnlcParser.cpp
    ${PROJECT_SOURCE_DIR}/src/util/VnlcTokenTypeUtil.cpp
    ${AST_FILES}
    VnlcParserTest.cpp
)

add_executable(
    vnlctest_semantic
    ${PROJECT_SOURCE_DIR}/src/diagnostic/VnlcDiagnostic.cpp
    ${PROJECT_SOURCE_DIR}/src/lexer/VnlcLexer.cpp
    ${PROJECT_SOURCE_DIR}/src/token/VnlcToken.cpp
    ${PROJECT_SOURCE_DIR}/src/parser/VnlcParser.cpp
    ${PROJECT_SOURCE_DIR}/src/util/VnlcTokenTypeUtil.cpp
    ${AST_FILES}
    ${SEMANTIC_FILES}
    ${TYPE_FILES}
    VnlcSemanticAnalyzerTest.cpp
)

add_executable(
    vnlctest_vni
    ${PROJECT_SOURCE_DIR}/src/token/VnlcToken.cpp
    ${PROJECT_SOURCE_DIR}/src/semantic/VnlcSemanticAnalysisResult.cpp
    ${AST_FILES}
    ${TYPE_FILES}
    ${VNI_FILES}
    VnlcModuleInterfaceTest.cpp
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
gtest_discover_tests(vnlctest_semantic)
gtest_discover_tests(vnlctest_vni)
