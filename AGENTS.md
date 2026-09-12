# Vanillang Compiler

## Overview
This is a DSL compiler, which allows developers to use Vanillang DSL to develop Minecraft datapack. It compiles Vanillang source code to mcfunction.
It is cross-platform, and should be available on Windows, macOS and Linux.

- Refer to https://github.com/SJMC-Dev/vnldocs to get information about Vanillang language.
- Refer to https://minecraft.wiki/ to get information about datapack and mcfunction.

## Commands
Be sure that your working directory is the root directory of this project before running these commands.

- Use `cmake --preset default-ninja` to generate build directory.
- Use `cmake --build --preset debug` to get a debug build.
- Use `cmake --build --preset release` to get a release build.

## Coding Standards
- Use PascalCase for customized type names and all type names defined in this project should start with `Vnlc`.
- Use camelCase for customized variable and function names.
- Follow `.clang-format` to format source code.
- Format code after editing.
- Do not add redundant comments if unnecessary.
- Refer to existing code for other coding standards.

## Commits
- Follow conventional commits when you need to write commit messages.

## Explanation
- Always explain the reason why you make such changes according to `git diff`.