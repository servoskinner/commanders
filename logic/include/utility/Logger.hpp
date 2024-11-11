#pragma once

#include <optional>
#include <fstream>
#include <string>

class Logger
{
    public:
    static Logger& get();

    void write(std::string msg);
    void enable(std::string filename);
    void disable();

    private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    ~Logger();

    std::optional<std::ofstream> logfile;
};