#pragma once

#include <CLI/CLI.hpp>

namespace pwnit::commands {

struct AnalysisOptions
{
    std::string file;
};

class AnalysisCommand
{
    AnalysisOptions opt;
    
public:
    explicit AnalysisCommand(CLI::App &app);
};

struct CheckOptions
{
    std::string file;
    bool symbols;
    bool sections;
};

class CheckCommand
{
    CheckOptions opt;
    
public:
    explicit CheckCommand(CLI::App &app);
};

struct StartOptions {
    std::string elf;
    std::string libc;
    std::string ld;
};

class StartCommand
{
    StartOptions opt;
    
public:
    explicit StartCommand(CLI::App &app);
};

}
