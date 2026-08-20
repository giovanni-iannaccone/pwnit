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
    bool json;
};

class CheckCommand
{
    CheckOptions opt;
    
public:
    explicit CheckCommand(CLI::App &app);
};

enum ContainerType
{
    DOCKER,
    PODMAN
};

struct ContainerOptions
{
    ContainerType type;
    uint16_t port;
    std::string container_id;
};

class ContainerCommand
{
    ContainerOptions opt;
    
public:
    explicit ContainerCommand(CLI::App &app);
};

struct RopOptions {
    bool json;
    int depth;
    std::string search;
    std::string elf;
};

class RopCommand
{
    RopOptions opt;
    
public:
    explicit RopCommand(CLI::App &app);
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
