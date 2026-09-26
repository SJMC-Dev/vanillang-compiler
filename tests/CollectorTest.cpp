#include "collector/Collector.hpp"
#include "config/Config.hpp"
#include "error/IllegalModuleOrPackageNameError.hpp"
#include "error/SyntaxError.hpp"
#include "lexer/Lexer.hpp"
#include "vni/import/ImportedAlias.hpp"
#include "vni/import/ImportedLet.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace vnlc {

    class CollectorTest : public testing::Test {
    protected:
        std::filesystem::path testDirectory;
        Config config;

        void SetUp() override {
            const auto* testInfo = testing::UnitTest::GetInstance()->current_test_info();
            testDirectory = std::filesystem::temp_directory_path() / ("vnlctest-collector-" + std::string(testInfo->name()));
            std::filesystem::remove_all(testDirectory);
            const auto packageRootPath = testDirectory / "collector_test_package";
            config = Config{
                .mode = RunningMode::COMPILE,
                .vanillangVersion = "1.0",
                .minecraftVersion = "26.1.2",
                .packageRootPath = packageRootPath,
                .inputFilePath = packageRootPath / "models" / "imports.vnl",
                .outputDirectory = std::nullopt,
                .moduleInterfaceOutputDirectory = std::nullopt,
                .dependencyPackageRootPaths = {
                    { "pkg", testDirectory / "dependency_source" },
                    { "extra", testDirectory / "another_source" },
                },
                .optimizationLevel = std::nullopt,
            };
            writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"int"},"count":{"category":"let","type":"int"}})");
            writeFile("dependency_source/sub/other.vni", R"({"flag":{"category":"let","type":"bool"}})");
            writeFile("dependency_source/sub/second.vni", R"({"caption":{"category":"let","type":"string"}})");
            writeFile("another_source/tools.vni", R"({"enabled":{"category":"let","type":"bool"}})");
        }

        void TearDown() override {
            std::error_code error;
            std::filesystem::remove_all(testDirectory, error);
        }

        void writeFile(const std::filesystem::path& relativePath, std::string_view contents) const {
            const auto path = testDirectory / relativePath;
            std::filesystem::create_directories(path.parent_path());
            std::ofstream output(path);
            output << contents;
        }

        CollectionResult collect(std::string_view source, std::size_t bufferSize = 3) const {
            std::stringstream input{ std::string(source) };
            Lexer lexer(input);
            Collector collector(std::move(lexer), bufferSize);
            return collector.collect(config);
        }

        static const ImportedPackage* importedPackage(const CollectionResult& result, std::string_view name) {
            const auto package = result.getImports().find(std::string(name));
            return package == result.getImports().end() ? nullptr : package->second.get();
        }
    };

    TEST_F(CollectorTest, LoadsCompleteModulesForIdentifierModuleAndWildcardImports) {
        for (const auto source : {
                 "import pkg.api.value\n",
                 "import pkg.api\n",
                 "import pkg.api as selected\n",
                 "import pkg.api.*\n",
                 "import pkg.api.{*, self}\n",
                 "import pkg.api.{self as selected, value as renamed}\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = collect(source);

            ASSERT_TRUE(result.getErrors().empty());
            ASSERT_EQ(result.getImports().size(), 1);
            const auto* package = importedPackage(result, "pkg");
            ASSERT_NE(package, nullptr);
            EXPECT_EQ(package->getName(), "pkg");
            EXPECT_TRUE(package->getSubPackages().empty());
            ASSERT_EQ(package->getModules().size(), 1);
            const auto* api = package->getModuleByName("api");
            ASSERT_NE(api, nullptr);
            EXPECT_EQ(api->getName(), "api");
            EXPECT_EQ(api->getIdentifiers().size(), 2);
            for (const auto name : { "value", "count" }) {
                const auto* variable = dynamic_cast<const ImportedLet*>(api->getIdentifierByName(name));
                ASSERT_NE(variable, nullptr);
                EXPECT_EQ(variable->getName(), name);
                EXPECT_EQ(variable->getType(), "int");
            }
        }
    }

    TEST_F(CollectorTest, LoadsCompletePackagesForPackageAndSelfImports) {
        writeFile("dependency_source/ignored.txt", "invalid json");
        for (const auto source : { "import pkg\n", "import pkg as library\n", "import pkg.{self as library}\n", "import pkg.*\n" }) {
            SCOPED_TRACE(source);
            const auto result = collect(source);

            ASSERT_TRUE(result.getErrors().empty());
            ASSERT_EQ(result.getImports().size(), 1);
            const auto* package = importedPackage(result, "pkg");
            ASSERT_NE(package, nullptr);
            EXPECT_EQ(package->getName(), "pkg");
            EXPECT_EQ(package->getModules().size(), 1);
            EXPECT_NE(package->getModuleByName("api"), nullptr);
            ASSERT_EQ(package->getSubPackages().size(), 1);
            const auto* sub = package->getSubPackageByName("sub");
            ASSERT_NE(sub, nullptr);
            EXPECT_EQ(sub->getName(), "sub");
            EXPECT_EQ(sub->getModules().size(), 2);
            EXPECT_NE(sub->getModuleByName("other"), nullptr);
            EXPECT_NE(sub->getModuleByName("second"), nullptr);
        }
    }

    TEST_F(CollectorTest, LoadsNestedImportPathsUsingTheirOriginalNames) {
        const auto result = collect("import pkg.{api.{self as moduleAlias, value as renamed}, sub.{other.*, second as alternate}}\n");

        ASSERT_TRUE(result.getErrors().empty());
        ASSERT_EQ(result.getImports().size(), 1);
        const auto* package = importedPackage(result, "pkg");
        ASSERT_NE(package, nullptr);
        ASSERT_EQ(package->getModules().size(), 1);
        const auto* api = package->getModuleByName("api");
        ASSERT_NE(api, nullptr);
        EXPECT_EQ(api->getIdentifiers().size(), 2);
        EXPECT_NE(api->getIdentifierByName("value"), nullptr);
        ASSERT_EQ(package->getSubPackages().size(), 1);
        const auto* sub = package->getSubPackageByName("sub");
        ASSERT_NE(sub, nullptr);
        ASSERT_EQ(sub->getModules().size(), 2);
        const auto* other = sub->getModuleByName("other");
        const auto* second = sub->getModuleByName("second");
        ASSERT_NE(other, nullptr);
        ASSERT_NE(second, nullptr);
        EXPECT_NE(other->getIdentifierByName("flag"), nullptr);
        EXPECT_NE(second->getIdentifierByName("caption"), nullptr);
    }

    TEST_F(CollectorTest, MergesRepeatedModuleAndPackageImportsInEitherOrder) {
        for (const auto source : {
                 "import pkg.api.value\nimport pkg.sub.other.flag\nimport pkg\nimport pkg.api\nimport extra.tools.enabled\n",
                 "import extra.tools.enabled\nimport pkg\nimport pkg.sub.other.flag\nimport pkg.api.value\nimport pkg.api\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = collect(source);

            ASSERT_TRUE(result.getErrors().empty());
            ASSERT_EQ(result.getImports().size(), 2);
            const auto* package = importedPackage(result, "pkg");
            ASSERT_NE(package, nullptr);
            ASSERT_EQ(package->getModules().size(), 1);
            const auto* api = package->getModuleByName("api");
            ASSERT_NE(api, nullptr);
            EXPECT_EQ(api->getIdentifiers().size(), 2);
            ASSERT_EQ(package->getSubPackages().size(), 1);
            const auto* sub = package->getSubPackageByName("sub");
            ASSERT_NE(sub, nullptr);
            EXPECT_EQ(sub->getModules().size(), 2);
            EXPECT_NE(sub->getModuleByName("other"), nullptr);
            EXPECT_NE(sub->getModuleByName("second"), nullptr);
            const auto* extra = importedPackage(result, "extra");
            ASSERT_NE(extra, nullptr);
            EXPECT_EQ(extra->getName(), "extra");
            const auto* tools = extra->getModuleByName("tools");
            ASSERT_NE(tools, nullptr);
            EXPECT_NE(tools->getIdentifierByName("enabled"), nullptr);
        }
    }

    TEST_F(CollectorTest, LoadsDependenciesOfUnselectedImportedAliases) {
        writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"int"},"External":{"category":"imported","source":"extra.tools.enabled"}})");

        const auto result = collect("import pkg.api.value\n");

        ASSERT_TRUE(result.getErrors().empty());
        EXPECT_EQ(result.getImports().size(), 2);
        const auto* package = importedPackage(result, "pkg");
        ASSERT_NE(package, nullptr);
        const auto* api = package->getModuleByName("api");
        ASSERT_NE(api, nullptr);
        const auto* alias = dynamic_cast<const ImportedAlias*>(api->getIdentifierByName("External"));
        ASSERT_NE(alias, nullptr);
        EXPECT_EQ(alias->getSource(), "extra.tools.enabled");
        const auto* extra = importedPackage(result, "extra");
        ASSERT_NE(extra, nullptr);
        const auto* tools = extra->getModuleByName("tools");
        ASSERT_NE(tools, nullptr);
        const auto* enabled = dynamic_cast<const ImportedLet*>(tools->getIdentifierByName("enabled"));
        ASSERT_NE(enabled, nullptr);
        EXPECT_EQ(enabled->getType(), "bool");
    }

    TEST_F(CollectorTest, ScansTypeDependenciesInEveryNestedIdentifierCollection) {
        writeFile("another_source/types.vni", R"({"Remote":{"category":"typealias","genericParameters":[],"originalType":"pkg.sub.other.Target"}})");
        writeFile("dependency_source/sub/other.vni", R"({"Target":{"category":"typealias","genericParameters":[],"originalType":"int"}})");
        for (
            const auto contents : {
                R"({"category":"let","type":"extra.types.Remote"})",
                R"({"category":"func","returnType":"extra.types.Remote","native":false,"parameters":{}})",
                R"({"category":"func","returnType":"void","native":false,"parameters":{"input":{"category":"parameter","type":"extra.types.Remote"}}})",
                R"({"category":"method","returnType":"extra.types.Remote","native":false,"static":false,"accessModifier":"public","parameters":{}})",
                R"({"category":"method","returnType":"void","native":false,"static":false,"accessModifier":"public","parameters":{"input":{"category":"parameter","type":"extra.types.Remote"}}})",
                R"({"category":"class","genericParameters":[],"properties":{},"methods":{},"baseClass":"extra.types.Remote","implementedInterfaces":[],"final":false})",
                R"({"category":"class","genericParameters":[],"properties":{},"methods":{},"baseClass":null,"implementedInterfaces":["extra.types.Remote"],"final":false})",
                R"({"category":"class","genericParameters":[],"properties":{"property":{"category":"property","type":"extra.types.Remote","static":false,"accessModifier":"public"}},"methods":{},"baseClass":null,"implementedInterfaces":[],"final":false})",
                R"({"category":"class","genericParameters":[],"properties":{},"methods":{"method":{"category":"method","returnType":"extra.types.Remote","native":false,"static":false,"accessModifier":"public","parameters":{}}},"baseClass":null,"implementedInterfaces":[],"final":false})",
                R"({"category":"class","genericParameters":[],"properties":{},"methods":{"method":{"category":"method","returnType":"void","native":false,"static":false,"accessModifier":"public","parameters":{"input":{"category":"parameter","type":"extra.types.Remote"}}}},"baseClass":null,"implementedInterfaces":[],"final":false})",
                R"({"category":"interface","genericParameters":[],"methods":{"method":{"category":"method","returnType":"extra.types.Remote","native":false,"static":false,"accessModifier":"public","parameters":{}}}})",
                R"({"category":"interface","genericParameters":[],"methods":{"method":{"category":"method","returnType":"void","native":false,"static":false,"accessModifier":"public","parameters":{"input":{"category":"parameter","type":"extra.types.Remote"}}}}})",
                R"({"category":"enum","genericParameters":[],"members":{"member":{"category":"enummember","associatedValues":{"value":{"category":"enumvalue","type":"extra.types.Remote"}}}}})",
                R"({"category":"enummember","associatedValues":{"value":{"category":"enumvalue","type":"extra.types.Remote"}}})",
                R"({"category":"typealias","genericParameters":[],"originalType":"extra.types.Remote"})",
                R"({"category":"property","type":"extra.types.Remote","static":false,"accessModifier":"public"})",
                R"({"category":"parameter","type":"extra.types.Remote"})",
            }) {
            SCOPED_TRACE(contents);
            writeFile("dependency_source/api.vni", std::string(R"({"value":{"category":"let","type":"int"},"holder":)") + contents + "}");

            const auto result = collect("import pkg.api.value\n");

            ASSERT_TRUE(result.getErrors().empty());
            const auto* extraPackage = importedPackage(result, "extra");
            ASSERT_NE(extraPackage, nullptr);
            const auto* typesModule = extraPackage->getModuleByName("types");
            ASSERT_NE(typesModule, nullptr);
            EXPECT_NE(typesModule->getIdentifierByName("Remote"), nullptr);
            const auto* package = importedPackage(result, "pkg");
            ASSERT_NE(package, nullptr);
            const auto* subPackage = package->getSubPackageByName("sub");
            ASSERT_NE(subPackage, nullptr);
            const auto* otherModule = subPackage->getModuleByName("other");
            ASSERT_NE(otherModule, nullptr);
            EXPECT_NE(otherModule->getIdentifierByName("Target"), nullptr);
        }
    }

    TEST_F(CollectorTest, LoadsEveryQualifiedTypeInGenericArgumentsAndNullableTypes) {
        writeFile("dependency_source/api.vni", R"({"Alias":{"category":"typealias","genericParameters":["T"],"originalType":"pkg.sub.other.Pair<T, List<extra.types.Remote?>>?"}})");
        writeFile("dependency_source/sub/other.vni", R"({"Pair":{"category":"typealias","genericParameters":["A","B"],"originalType":"A"}})");
        writeFile("another_source/types.vni", R"({"Remote":{"category":"typealias","genericParameters":[],"originalType":"int"}})");

        const auto result = collect("import pkg.api.Alias\n");

        ASSERT_TRUE(result.getErrors().empty());
        EXPECT_EQ(result.getImports().size(), 2);
        const auto* package = importedPackage(result, "pkg");
        ASSERT_NE(package, nullptr);
        const auto* sub = package->getSubPackageByName("sub");
        ASSERT_NE(sub, nullptr);
        const auto* other = sub->getModuleByName("other");
        ASSERT_NE(other, nullptr);
        EXPECT_NE(other->getIdentifierByName("Pair"), nullptr);
        const auto* extra = importedPackage(result, "extra");
        ASSERT_NE(extra, nullptr);
        const auto* types = extra->getModuleByName("types");
        ASSERT_NE(types, nullptr);
        EXPECT_NE(types->getIdentifierByName("Remote"), nullptr);
    }

    TEST_F(CollectorTest, FollowsTransitiveRepeatedAndCyclicIndirectImports) {
        writeFile("dependency_source/api.vni", R"({"External":{"category":"imported","source":"pkg.sub.other.bridge"},"repeated":{"category":"imported","source":"pkg.sub.other.bridge"}})");
        writeFile("dependency_source/sub/other.vni", R"({"bridge":{"category":"imported","source":"extra.tools.enabled"},"back":{"category":"imported","source":"pkg.api.External"}})");

        const auto result = collect("import pkg.api.External\n");

        ASSERT_TRUE(result.getErrors().empty());
        ASSERT_EQ(result.getImports().size(), 2);
        const auto* package = importedPackage(result, "pkg");
        ASSERT_NE(package, nullptr);
        const auto* api = package->getModuleByName("api");
        ASSERT_NE(api, nullptr);
        EXPECT_EQ(api->getIdentifiers().size(), 2);
        const auto* alias = dynamic_cast<const ImportedAlias*>(api->getIdentifierByName("External"));
        ASSERT_NE(alias, nullptr);
        EXPECT_EQ(alias->getSource(), "pkg.sub.other.bridge");
        const auto* sub = package->getSubPackageByName("sub");
        ASSERT_NE(sub, nullptr);
        ASSERT_EQ(sub->getModules().size(), 1);
        const auto* other = sub->getModuleByName("other");
        ASSERT_NE(other, nullptr);
        EXPECT_EQ(other->getIdentifiers().size(), 2);
        EXPECT_NE(other->getIdentifierByName("bridge"), nullptr);
        EXPECT_NE(other->getIdentifierByName("back"), nullptr);
        const auto* extra = importedPackage(result, "extra");
        ASSERT_NE(extra, nullptr);
        const auto* tools = extra->getModuleByName("tools");
        ASSERT_NE(tools, nullptr);
        EXPECT_NE(tools->getIdentifierByName("enabled"), nullptr);
    }

    TEST_F(CollectorTest, LoadsPackagesAndModulesReferencedByIndirectImports) {
        for (const auto source : { "extra", "extra.tools", "pkg.sub" }) {
            SCOPED_TRACE(source);
            writeFile("dependency_source/api.vni", std::string(R"({"External":{"category":"imported","source":")") + source + R"("}})");

            const auto result = collect("import pkg.api.External\n");

            ASSERT_TRUE(result.getErrors().empty());
            const auto* package = importedPackage(result, "pkg");
            ASSERT_NE(package, nullptr);
            EXPECT_NE(package->getModuleByName("api"), nullptr);
            if (std::string_view(source) == "pkg.sub") {
                EXPECT_EQ(result.getImports().size(), 1);
                const auto* sub = package->getSubPackageByName("sub");
                ASSERT_NE(sub, nullptr);
                EXPECT_EQ(sub->getModules().size(), 2);
                EXPECT_NE(sub->getModuleByName("other"), nullptr);
                EXPECT_NE(sub->getModuleByName("second"), nullptr);
            } else {
                EXPECT_EQ(result.getImports().size(), 2);
                const auto* extra = importedPackage(result, "extra");
                ASSERT_NE(extra, nullptr);
                const auto* tools = extra->getModuleByName("tools");
                ASSERT_NE(tools, nullptr);
                EXPECT_NE(tools->getIdentifierByName("enabled"), nullptr);
            }
        }
    }

    TEST_F(CollectorTest, MergesOverlappingDirectAndIndirectImportsInEitherOrder) {
        writeFile("dependency_source/api.vni", R"({"External":{"category":"imported","source":"extra.tools.enabled"}})");
        writeFile("dependency_source/sub/other.vni", R"({"flag":{"category":"let","type":"bool"},"External":{"category":"imported","source":"extra.tools.enabled"}})");
        for (const auto source : {
                 "import pkg.api.External\nimport extra.tools.enabled\nimport pkg.sub.other.flag\nimport extra.tools as tools\n",
                 "import extra.tools.enabled\nimport pkg.api.External\nimport pkg.sub.other.flag\nimport extra.tools as tools\n",
             }) {
            SCOPED_TRACE(source);
            const auto result = collect(source);

            ASSERT_TRUE(result.getErrors().empty());
            ASSERT_EQ(result.getImports().size(), 2);
            const auto* package = importedPackage(result, "pkg");
            ASSERT_NE(package, nullptr);
            const auto* api = package->getModuleByName("api");
            ASSERT_NE(api, nullptr);
            EXPECT_NE(dynamic_cast<const ImportedAlias*>(api->getIdentifierByName("External")), nullptr);
            const auto* sub = package->getSubPackageByName("sub");
            ASSERT_NE(sub, nullptr);
            const auto* other = sub->getModuleByName("other");
            ASSERT_NE(other, nullptr);
            EXPECT_NE(other->getIdentifierByName("flag"), nullptr);
            EXPECT_NE(dynamic_cast<const ImportedAlias*>(other->getIdentifierByName("External")), nullptr);
            const auto* extra = importedPackage(result, "extra");
            ASSERT_NE(extra, nullptr);
            ASSERT_EQ(extra->getModules().size(), 1);
            const auto* tools = extra->getModuleByName("tools");
            ASSERT_NE(tools, nullptr);
            EXPECT_EQ(tools->getIdentifiers().size(), 1);
            EXPECT_NE(tools->getIdentifierByName("enabled"), nullptr);
        }
    }

    TEST_F(CollectorTest, DoesNotCommitImportsWithMissingOrMalformedIndirectDependencies) {
        writeFile("another_source/broken.vni", "invalid json");
        for (const auto source : { "absent.tools.enabled", "extra.missing.enabled", "extra.broken.enabled", "", ".extra.tools.enabled", "extra..tools.enabled", "extra.tools.enabled." }) {
            SCOPED_TRACE(source);
            writeFile(
                "dependency_source/api.vni",
                std::string(R"({"External":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":")") + source + R"("}})"
            );

            const auto result = collect("import pkg.api.External\n");

            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getPhase(), DiagnosticPhase::COLLECTOR);
            EXPECT_EQ(result.getErrors().front().getSeverity(), DiagnosticSeverity::ERROR);
            EXPECT_TRUE(result.getImports().empty());
        }
    }

    TEST_F(CollectorTest, FailedIndirectImportsPreserveEarlierModulesWithoutCommittingDependencies) {
        writeFile("dependency_source/broken.vni", R"({"External":{"category":"imported","source":"extra.tools.enabled"},"broken":{"category":"imported","source":"pkg.sub.missing.value"}})");

        const auto result = collect("import pkg.api as kept\nimport pkg.broken as staged\n");

        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors().front().getPhase(), DiagnosticPhase::COLLECTOR);
        ASSERT_EQ(result.getImports().size(), 1);
        const auto* package = importedPackage(result, "pkg");
        ASSERT_NE(package, nullptr);
        ASSERT_EQ(package->getModules().size(), 1);
        const auto* api = package->getModuleByName("api");
        ASSERT_NE(api, nullptr);
        EXPECT_NE(api->getIdentifierByName("value"), nullptr);
        EXPECT_NE(api->getIdentifierByName("count"), nullptr);
        EXPECT_TRUE(package->getSubPackages().empty());
        EXPECT_EQ(package->getModuleByName("broken"), nullptr);
        EXPECT_EQ(importedPackage(result, "extra"), nullptr);
    }

    TEST_F(CollectorTest, FailedImportPathListsDoNotCommitNewBranches) {
        for (const auto source : { "import pkg.sub.{other as staged, missing}\n", "import pkg.sub.{missing, other as staged}\n" }) {
            SCOPED_TRACE(source);
            const auto result = collect(std::string("import pkg.api as kept\n") + source);

            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getPhase(), DiagnosticPhase::COLLECTOR);
            ASSERT_EQ(result.getImports().size(), 1);
            const auto* package = importedPackage(result, "pkg");
            ASSERT_NE(package, nullptr);
            ASSERT_EQ(package->getModules().size(), 1);
            EXPECT_NE(package->getModuleByName("api"), nullptr);
            EXPECT_TRUE(package->getSubPackages().empty());
        }
    }

    TEST_F(CollectorTest, ReportsUnknownPackagesAndModulesAsCollectorErrors) {
        for (const auto source : { "import absent.api\n", "import pkg.absent\n", "import pkg.sub.absent\n", "import pkg.{api, absent}\n" }) {
            SCOPED_TRACE(source);
            const auto result = collect(source);

            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getPhase(), DiagnosticPhase::COLLECTOR);
            EXPECT_FALSE(result.getErrors().front().getMessage().empty());
            EXPECT_TRUE(result.getImports().empty());
        }
    }

    TEST_F(CollectorTest, ReportsMalformedModuleInterfacesAsCollectorErrors) {
        for (const auto contents : { "", "invalid json", "[]", R"({"value":{"category":"let","type":123}})", R"({"value":{"category":null}})" }) {
            SCOPED_TRACE(contents);
            writeFile("dependency_source/broken.vni", contents);

            const auto result = collect("import pkg.broken\n");

            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getPhase(), DiagnosticPhase::COLLECTOR);
            EXPECT_TRUE(result.getImports().empty());
        }
    }

    TEST_F(CollectorTest, RejectsMalformedUnselectedIdentifiersWhenImportingAModuleOrIdentifier) {
        writeFile("dependency_source/api.vni", R"({"value":{"category":"let","type":"int"},"unused":{"category":"let","type":123}})");
        for (const auto source : { "import pkg.api\n", "import pkg.api.value\n" }) {
            SCOPED_TRACE(source);
            const auto result = collect(source);

            ASSERT_EQ(result.getErrors().size(), 1);
            EXPECT_EQ(result.getErrors().front().getPhase(), DiagnosticPhase::COLLECTOR);
            EXPECT_TRUE(result.getImports().empty());
        }
    }

    TEST_F(CollectorTest, ReportsMissingDependencyDirectoriesAsCollectorErrors) {
        config.dependencyPackageRootPaths.at("pkg") = testDirectory / "missing_directory";

        const auto result = collect("import pkg.api\n");

        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors().front().getPhase(), DiagnosticPhase::COLLECTOR);
        EXPECT_TRUE(result.getImports().empty());
    }

    TEST_F(CollectorTest, PreservesModuleNamesWhenReadingSymbolicLinks) {
        writeFile("shared/original.vni", R"({"linked":{"category":"let","type":"int"}})");
        std::error_code error;
        std::filesystem::create_symlink(testDirectory / "shared/original.vni", testDirectory / "dependency_source/linkedApi.vni", error);
        if (error) {
            GTEST_SKIP() << error.message();
        }

        const auto result = collect("import pkg.linkedApi.{self, linked}\n");

        ASSERT_TRUE(result.getErrors().empty());
        const auto* package = importedPackage(result, "pkg");
        ASSERT_NE(package, nullptr);
        EXPECT_EQ(package->getModules().size(), 1);
        const auto* linkedApi = package->getModuleByName("linkedApi");
        ASSERT_NE(linkedApi, nullptr);
        EXPECT_EQ(linkedApi->getName(), "linkedApi");
        EXPECT_NE(linkedApi->getIdentifierByName("linked"), nullptr);
        EXPECT_EQ(package->getModuleByName("original"), nullptr);
    }

    TEST_F(CollectorTest, ReportsCyclicPackageDirectoriesAsCollectorErrors) {
        std::error_code error;
        std::filesystem::create_directory_symlink(testDirectory / "dependency_source", testDirectory / "dependency_source/loop", error);
        if (error) {
            GTEST_SKIP() << error.message();
        }

        const auto result = collect("import pkg\n");

        ASSERT_EQ(result.getErrors().size(), 1);
        EXPECT_EQ(result.getErrors().front().getPhase(), DiagnosticPhase::COLLECTOR);
        EXPECT_NE(result.getErrors().front().getMessage().find("Package directory cycle"), std::string_view::npos);
        EXPECT_TRUE(result.getImports().empty());
    }

    TEST_F(CollectorTest, CollectsEmptyAndCommentOnlyModules) {
        for (const auto source : { "", "\n\n", "# empty module\n#* class Hidden {} *#\n" }) {
            SCOPED_TRACE(source);
            const auto result = collect(source);

            EXPECT_TRUE(result.getErrors().empty());
            EXPECT_TRUE(result.getImports().empty());
            const auto& outline = result.getModuleOutline();
            EXPECT_EQ(outline.getName(), "imports");
            EXPECT_EQ(outline.getFullName(), "collector_test_package.models.imports");
            EXPECT_TRUE(outline.getTypeDeclarations().empty());
        }
    }

    TEST_F(CollectorTest, DerivesModuleNamesFromTheConfiguredPackagePath) {
        config.inputFilePath = config.packageRootPath / "nested" / "models" / "sample.vnl";
        const auto result = collect("class Item {}\n");

        EXPECT_EQ(result.getModuleOutline().getName(), "sample");
        EXPECT_EQ(result.getModuleOutline().getFullName(), "collector_test_package.nested.models.sample");
    }

    TEST_F(CollectorTest, RejectsIllegalModuleAndPackageNames) {
        for (const auto path : { "int.vnl", "bad-name.vnl", "bad-name/module.vnl", "class/module.vnl", "123/module.vnl" }) {
            SCOPED_TRACE(path);
            config.inputFilePath = config.packageRootPath / path;
            config.inputFilePath.make_preferred();
            EXPECT_THROW(static_cast<void>(collect("")), IllegalModuleOrPackageNameError);
        }
    }

    TEST_F(CollectorTest, CollectsEveryTopLevelTypeOutlineInSourceOrder) {
        const auto result = collect(R"(
class Base {}
final class Box<T> extends Base implements Readable<T> {
    func run() { if true { return } }
}
interface Readable<T> {
    func read(value: T) -> T
}
enum State<T> {
    Ready
    Some(value: T)
}
type Alias<T> = Box<T>
)");
        ASSERT_TRUE(result.getErrors().empty());
        const auto& declarations = result.getModuleOutline().getTypeDeclarations();
        ASSERT_EQ(declarations.size(), 5);
        const auto* baseClass = dynamic_cast<const ClassDeclarationOutline*>(declarations[0].get());
        const auto* finalClass = dynamic_cast<const ClassDeclarationOutline*>(declarations[1].get());
        const auto* interfaceDeclaration = dynamic_cast<const InterfaceDeclarationOutline*>(declarations[2].get());
        const auto* enumDeclaration = dynamic_cast<const EnumDeclarationOutline*>(declarations[3].get());
        const auto* aliasDeclaration = dynamic_cast<const TypeAliasDeclarationOutline*>(declarations[4].get());
        ASSERT_NE(baseClass, nullptr);
        ASSERT_NE(finalClass, nullptr);
        ASSERT_NE(interfaceDeclaration, nullptr);
        ASSERT_NE(enumDeclaration, nullptr);
        ASSERT_NE(aliasDeclaration, nullptr);
        EXPECT_EQ(baseClass->getName(), "Base");
        EXPECT_EQ(finalClass->getName(), "Box");
        EXPECT_EQ(interfaceDeclaration->getName(), "Readable");
        EXPECT_EQ(enumDeclaration->getName(), "State");
        EXPECT_EQ(aliasDeclaration->getAliasName(), "Alias");

        const auto& members = enumDeclaration->getMemberDeclarations();
        ASSERT_EQ(members.size(), 2);
        EXPECT_EQ(members[0]->getName(), "Ready");
        EXPECT_EQ(members[1]->getName(), "Some");
        for (const auto& member : members) {
            EXPECT_EQ(member->getEnumDeclaration(), enumDeclaration);
        }
    }

    TEST_F(CollectorTest, CollectsEmptyEnumAndSkipsMemberMetadataAndNestedParentheses) {
        const auto result = collect(R"(
enum Empty {}
enum State<T> {
    metadata(since "1.0")
    Ready
    metadata(custom nested(value))
    Some(value: T, fallback: T = create(defaultValue()))
    Done
}
class After {}
)");
        ASSERT_TRUE(result.getErrors().empty());
        const auto& declarations = result.getModuleOutline().getTypeDeclarations();
        ASSERT_EQ(declarations.size(), 3);
        const auto* emptyEnum = dynamic_cast<const EnumDeclarationOutline*>(declarations[0].get());
        const auto* enumDeclaration = dynamic_cast<const EnumDeclarationOutline*>(declarations[1].get());
        const auto* after = dynamic_cast<const ClassDeclarationOutline*>(declarations[2].get());
        ASSERT_NE(emptyEnum, nullptr);
        ASSERT_NE(enumDeclaration, nullptr);
        ASSERT_NE(after, nullptr);
        EXPECT_TRUE(emptyEnum->getMemberDeclarations().empty());
        const auto& members = enumDeclaration->getMemberDeclarations();
        ASSERT_EQ(members.size(), 3);
        EXPECT_EQ(members[0]->getName(), "Ready");
        EXPECT_EQ(members[1]->getName(), "Some");
        EXPECT_EQ(members[2]->getName(), "Done");
        for (const auto& member : members) {
            EXPECT_EQ(member->getEnumDeclaration(), enumDeclaration);
        }
        EXPECT_EQ(after->getName(), "After");
    }

    TEST_F(CollectorTest, SkipsNestedDeclarationsAndNonTypeTopLevelDeclarations) {
        const auto result = collect(R"(
let value = 0
func run() {
    if true {
        class Local {}
        type LocalAlias = int
    }
    enum LocalState { Ready }
}
class Outer {
    class Nested {}
    interface NestedInterface {}
    func run() { if true {} }
}
interface Visible {}
type Alias = int
)");
        ASSERT_TRUE(result.getErrors().empty());
        const auto& declarations = result.getModuleOutline().getTypeDeclarations();
        ASSERT_EQ(declarations.size(), 3);
        const auto* outer = dynamic_cast<const ClassDeclarationOutline*>(declarations[0].get());
        const auto* visible = dynamic_cast<const InterfaceDeclarationOutline*>(declarations[1].get());
        const auto* alias = dynamic_cast<const TypeAliasDeclarationOutline*>(declarations[2].get());
        ASSERT_NE(outer, nullptr);
        ASSERT_NE(visible, nullptr);
        ASSERT_NE(alias, nullptr);
        EXPECT_EQ(outer->getName(), "Outer");
        EXPECT_EQ(visible->getName(), "Visible");
        EXPECT_EQ(alias->getAliasName(), "Alias");
    }

    TEST_F(CollectorTest, StopsCollectingAtTopLevelExports) {
        const std::string source = "class Before {}\nexport Before\nclass After {}\n";
        const auto result = collect(source);

        const auto& outline = result.getModuleOutline();
        ASSERT_EQ(outline.getTypeDeclarations().size(), 1);
        const auto* declaration = dynamic_cast<const ClassDeclarationOutline*>(outline.getTypeDeclarations()[0].get());
        ASSERT_NE(declaration, nullptr);
        EXPECT_EQ(declaration->getName(), "Before");
        EXPECT_EQ(outline.getOffset(), 0);
        EXPECT_EQ(outline.getLength(), source.find("export"));
    }

    TEST_F(CollectorTest, RecordsTypeAndEnumMemberSourceLocations) {
        const std::string source = "\n  final class Box {}\ninterface Readable {}\nenum State {\n    Ready\n    Some(value: int)\n}\nexport Box\n";
        const auto result = collect(source);

        const auto& outline = result.getModuleOutline();
        EXPECT_EQ(outline.locate(), (std::pair<std::size_t, std::size_t>{ 1, 1 }));
        EXPECT_EQ(outline.getOffset(), 0);
        EXPECT_EQ(outline.getLength(), source.find("export"));
        const auto& declarations = outline.getTypeDeclarations();
        ASSERT_EQ(declarations.size(), 3);
        EXPECT_EQ(declarations[0]->locate(), (std::pair<std::size_t, std::size_t>{ 2, 3 }));
        EXPECT_EQ(declarations[0]->getOffset(), source.find("final"));
        EXPECT_EQ(declarations[0]->getLength(), source.find("interface") - source.find("final"));
        EXPECT_EQ(declarations[1]->locate(), (std::pair<std::size_t, std::size_t>{ 3, 1 }));
        EXPECT_EQ(declarations[1]->getOffset(), source.find("interface"));
        EXPECT_EQ(declarations[1]->getLength(), source.find("enum") - source.find("interface"));
        const auto* enumDeclaration = dynamic_cast<const EnumDeclarationOutline*>(declarations[2].get());
        ASSERT_NE(enumDeclaration, nullptr);
        EXPECT_EQ(enumDeclaration->locate(), (std::pair<std::size_t, std::size_t>{ 4, 1 }));
        EXPECT_EQ(enumDeclaration->getOffset(), source.find("enum"));
        EXPECT_EQ(enumDeclaration->getLength(), source.find("export") - source.find("enum"));
        const auto& members = enumDeclaration->getMemberDeclarations();
        ASSERT_EQ(members.size(), 2);
        EXPECT_EQ(members[0]->locate(), (std::pair<std::size_t, std::size_t>{ 5, 5 }));
        EXPECT_EQ(members[0]->getOffset(), source.find("Ready"));
        EXPECT_EQ(members[0]->getLength(), source.find("Some") - source.find("Ready"));
        EXPECT_EQ(members[1]->locate(), (std::pair<std::size_t, std::size_t>{ 6, 5 }));
        EXPECT_EQ(members[1]->getOffset(), source.find("Some"));
        EXPECT_EQ(members[1]->getLength(), source.find("}\nexport") - source.find("Some"));
    }

    TEST_F(CollectorTest, CollectsImportsAndOutlinesAcrossTokenBufferBoundaries) {
        for (const std::size_t bufferSize : { 1, 2, 3, 64 }) {
            SCOPED_TRACE(bufferSize);
            const auto result = collect(
                R"(
# import and declarations span multiple buffers
import pkg.api.{
    self as api,
    value
}
#* class Hidden {} *#
final class Box<T> {
    func run() { if true {} }
}
enum State<T> {
    metadata(since "1.0")
    Some(value: T)
}
type Alias<T> = Box<T>
)",
                bufferSize
            );
            ASSERT_TRUE(result.getErrors().empty());
            ASSERT_EQ(result.getImports().size(), 1);
            const auto* package = importedPackage(result, "pkg");
            ASSERT_NE(package, nullptr);
            const auto* importedModule = package->getModuleByName("api");
            ASSERT_NE(importedModule, nullptr);
            EXPECT_NE(importedModule->getIdentifierByName("value"), nullptr);
            const auto& declarations = result.getModuleOutline().getTypeDeclarations();
            ASSERT_EQ(declarations.size(), 3);
            const auto* classDeclaration = dynamic_cast<const ClassDeclarationOutline*>(declarations[0].get());
            const auto* enumDeclaration = dynamic_cast<const EnumDeclarationOutline*>(declarations[1].get());
            const auto* aliasDeclaration = dynamic_cast<const TypeAliasDeclarationOutline*>(declarations[2].get());
            ASSERT_NE(classDeclaration, nullptr);
            ASSERT_NE(enumDeclaration, nullptr);
            ASSERT_NE(aliasDeclaration, nullptr);
            EXPECT_EQ(classDeclaration->getName(), "Box");
            EXPECT_EQ(enumDeclaration->getName(), "State");
            EXPECT_EQ(aliasDeclaration->getAliasName(), "Alias");
            ASSERT_EQ(enumDeclaration->getMemberDeclarations().size(), 1);
            EXPECT_EQ(enumDeclaration->getMemberDeclarations()[0]->getName(), "Some");
        }
    }

    TEST_F(CollectorTest, ReportsMalformedTypeHeadersAndUnbalancedBodiesAsSyntaxErrors) {
        for (const auto source : {
                 "class",
                 "final interface Bad {}",
                 "class Box<T",
                 "class Box {",
                 "interface",
                 "interface Readable<T",
                 "interface Readable",
                 "interface Readable {",
                 "enum",
                 "enum State<T",
                 "enum State",
                 "enum State { Ready Other }",
                 "enum State { metadata Ready }",
                 "enum State { metadata(",
                 "enum State { Some(",
                 "type",
                 "type Alias int",
                 "type Alias<T = int",
             }) {
            SCOPED_TRACE(source);
            EXPECT_THROW(static_cast<void>(collect(source)), SyntaxError);
        }
    }

    TEST_F(CollectorTest, ReportsMalformedImportPathsAsSyntaxErrors) {
        for (const auto source : {
                 "import",
                 "import pkg.",
                 "import pkg..api",
                 "import pkg.{}",
                 "import pkg.{api",
                 "import pkg.api value",
             }) {
            SCOPED_TRACE(source);
            EXPECT_THROW(static_cast<void>(collect(source)), SyntaxError);
        }
    }

    TEST_F(CollectorTest, AccumulatesImportDiagnosticsAndContinuesCollecting) {
        const std::string source = "\n  import absent.api\nimport pkg.missing\nimport pkg.api.value\nclass Kept {}\n";
        const auto result = collect(source);

        const auto& errors = result.getErrors();
        ASSERT_EQ(errors.size(), 2);
        const auto firstImportOffset = source.find("import");
        for (std::size_t index = 0; index < errors.size(); ++index) {
            EXPECT_EQ(errors[index].getPhase(), DiagnosticPhase::COLLECTOR);
            EXPECT_EQ(errors[index].getSeverity(), DiagnosticSeverity::ERROR);
            EXPECT_FALSE(errors[index].getMessage().empty());
            EXPECT_EQ(errors[index].getLine(), index + 2);
            EXPECT_EQ(errors[index].getColumn(), index == 0 ? 3 : 1);
            EXPECT_EQ(errors[index].getOffset(), index == 0 ? firstImportOffset : source.find("import", firstImportOffset + 1));
            EXPECT_EQ(errors[index].getLength(), 6);
        }
        ASSERT_EQ(result.getImports().size(), 1);
        const auto* package = importedPackage(result, "pkg");
        ASSERT_NE(package, nullptr);
        EXPECT_NE(package->getModuleByName("api"), nullptr);
        EXPECT_EQ(package->getModuleByName("missing"), nullptr);
        const auto& declarations = result.getModuleOutline().getTypeDeclarations();
        ASSERT_EQ(declarations.size(), 1);
        const auto* declaration = dynamic_cast<const ClassDeclarationOutline*>(declarations[0].get());
        ASSERT_NE(declaration, nullptr);
        EXPECT_EQ(declaration->getName(), "Kept");
    }
} // namespace vnlc
