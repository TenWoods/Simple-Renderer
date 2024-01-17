#include <string>
#include <iostream>
#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>

//TODO: output to file

class logger
{
private:
    bool isOut;
    bool isFileOut;
    //std::ofstream fs;
    std::string m_name;
    std::string out_path;
    std::shared_ptr<spdlog::logger> m_logger;
    std::shared_ptr<spdlog::logger> m_file_logger;
public:
    logger() : isOut(true), isFileOut(false)
    {
        m_logger = spdlog::stdout_color_mt("");
    }
    logger(const std::string name) : isOut(true), isFileOut(false), m_name(name)
    {
        m_logger = spdlog::stdout_color_mt(m_name);
    }

    explicit logger(const logger& obj)
    {
        isOut = obj.isOut;
        out_path = obj.out_path;
        m_logger = obj.m_logger;
        m_file_logger = obj.m_file_logger;
    }

    ~logger()
    {
    }

    template<typename... Args>
    void std_output(fmt::format_string<Args...> fmt, Args &&...args)
    {
        if (!isOut)
            return;
        m_logger->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void err_output(fmt::format_string<Args...> fmt, Args &&...args)
    {
        if (!isOut)
            return;
        m_logger->error(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void file_output(fmt::format_string<Args...> fmt, Args &&...args)
    {
        //std::cout << "?" << std::endl;
        if (!isFileOut)
            return;
        m_file_logger->info(fmt, std::forward<Args>(args)...);
    }

    void setOut(const bool value)
    {
        isOut = value;
    }

    void setFileOut(const bool value)
    {
        isFileOut = value;
    }

    void setPath(const std::string& path)
    {
        out_path = path;
        m_file_logger = spdlog::basic_logger_mt<spdlog::synchronous_factory>(m_name+"-file", path);
        m_file_logger->set_pattern("%v");
    }
};
