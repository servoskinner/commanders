#include "Logger.hpp"

Logger& Logger::get()
{
    static Logger instance;
    return instance;
}

Logger::~Logger()
{
    if (logfile.has_value()) {
        logfile->close();
    }
}

void Logger::enable(std::string filename)
{
    logfile.emplace();
    logfile->open(filename);
}

void Logger::disable()
{
    logfile.reset();
}

void Logger::write(std::string msg = "")
{
    if (logfile.has_value()) {
        logfile.value() << msg << std::endl;
    }
}