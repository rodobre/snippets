#ifndef _LOGGING_H
#define _LOGGING_H

#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <cstdio>
#include <utility>
#include <ctime>
#include <iomanip>

#define LOG_INFO            0
#define LOG_ERROR           1
#define LOG_CRITICAL_ERROR  2

class Logger
{
private:
    std::mutex      access_mutex;
    std::mutex      time_mutex;
    std::string     html_header;
    std::fstream    output_file;

    size_t          info_count;
    size_t          error_count;
    size_t          critical_error_count;

public:
    Logger() :
        html_header(
                "<head>\n"
                "<style>\n"
                "body\n"
                "{\n"
                "\tbackground-color: #101010;\n"
                "}\n\n"
                "i\n"
                "{\n"
                "\tfont-family: Consolas, monaco, monospace;\n"
                "\tfont-style: normal;\n"
                "\ttext-align: justify;\n"
                "\ttext-indent: 50px;\n"
                "\tcolor: #10dd10;\n"
                "}\n\n"
                "e\n"
                "{\n"
                "\tfont-family: Consolas, monaco, monospace;\n"
                "\tfont-style: normal;\n"
                "\ttext-align: justify;\n"
                "\ttext-indent: 60px;\n"
                "\tcolor: #ffa510;\n"
                "}\n\n"
                "ce\n"
                "{\n"
                "\tfont-family: Consolas, monaco, monospace;\n"
                "\tfont-style: normal;\n"
                "\ttext-align: justify;\n"
                "\ttext-indent: 60px;\n"
                "\tcolor: #ff1010;\n"
                "}\n"
                "</style>\n"
                "</head>\n\n"
                "<body>\n"
                    ),

        info_count(0),
        error_count(0),
        critical_error_count(0) {}

    void OpenLogFile(const std::string& path)
    {
        output_file.open(path, std::ios::in | std::ios::out | std::ios::ate);

        if(!output_file || !output_file.is_open())
        {
            output_file.open(path, std::ios::out);

            if(!output_file || !output_file.is_open())
                return;

            output_file.write(html_header.c_str(), html_header.size());
        }
        else
            output_file.seekp(-8, std::ios_base::end);

        LogInfo("Beginning logging session");
    }

    std::string HelperGetTime()
    {
        std::lock_guard lock(this->time_mutex);

        std::time_t t   = std::time(nullptr);
        std::tm tm      = *std::localtime(&t);

        char buf[128] = {};
        strftime(buf, 127, "[%H:%M:%S %d.%m.%Y] ", &tm);
        return std::string(buf);
    }

    template <typename... Args>
    void LogInfo(const char* fmt, Args... arguments)
    {
        char format_buffer[2049] = {};

        auto result = _snprintf_s(format_buffer, sizeof(format_buffer) - 1, _TRUNCATE, fmt, std::forward<Args>(arguments)...);

        if(result == -1)
            return;

        std::string construct_html_entry = "\t\t<i>";
        construct_html_entry.append(HelperGetTime());
        construct_html_entry.append(format_buffer);
        construct_html_entry += "</i><br>\n";

        std::lock_guard lock(this->access_mutex);

        ++ info_count;
        output_file.write(construct_html_entry.c_str(), construct_html_entry.size());
        output_file.flush();
    };

    template <typename... Args>
    void LogError(const char* fmt, Args... arguments)
    {
        char format_buffer[2049] = {};

        auto result = _snprintf_s(format_buffer, sizeof(format_buffer) - 1, _TRUNCATE, fmt, std::forward<Args>(arguments)...);

        if(result == -1)
            return;

        std::string construct_html_entry = "\t\t<e>";
        construct_html_entry.append(HelperGetTime());
        construct_html_entry.append(format_buffer);
        construct_html_entry += "</i><br>\n";

        std::lock_guard lock(this->access_mutex);

        ++ error_count;
        output_file.write(construct_html_entry.c_str(), construct_html_entry.size());
        output_file.flush();
    };

    template <typename... Args>
    void LogCriticalError(const char* fmt, Args... arguments)
    {
        char format_buffer[2049] = {};

        auto result = _snprintf_s(format_buffer, sizeof(format_buffer) - 1, _TRUNCATE, fmt, std::forward<Args>(arguments)...);

        if(result == -1)
            return;

        std::string construct_html_entry = "\t\t<ce>";
        construct_html_entry.append(HelperGetTime());
        construct_html_entry.append(format_buffer);
        construct_html_entry += "</i><br>\n";

        std::lock_guard lock(this->access_mutex);

        ++ critical_error_count;
        output_file.write(construct_html_entry.c_str(), construct_html_entry.size());
        output_file.flush();
    };

    void Close()
    {
        LogInfo("Logging session ended");
        LogInfo("<br><br>[%u] Messages [%u] Errors [%u] Critical errors", info_count, error_count, critical_error_count);

        std::lock_guard lock(this->access_mutex);

        output_file.flush();
        output_file.close();
    }

    ~Logger() = default;
};

class FnLog
{
private:
    Logger&  logger;
    uint8_t     log_type;

public:
    const char* m_pScope;
    const char* m_pFile;
    int32_t m_dLine;
    bool b_WroteMessage;

    FnLog(Logger& _logger, uint8_t tmp_log_type, const char* scope, const char* file, int32_t line, const char* message = nullptr)
    : log_type(tmp_log_type), m_pScope(scope), m_pFile(file), m_dLine(line), logger(_logger)
    {
        if(message == nullptr)
        {
            #ifdef LOG_EXTREME_VERBOSITY
            logger.LogInfo("%s() ----> ( %d : %s )", m_pScope, m_dLine, m_pFile);
            #else
            logger.LogInfo("%s() ---->", m_pScope);
            #endif

            b_WroteMessage = false;
        }
        else
        {
            switch(log_type)
            {
                case LOG_INFO: default:
                {
                    #ifdef LOG_EXTREME_VERBOSITY
                    logger.LogInfo("%s() %s ( %d : %s )", m_pScope, message, m_dLine, m_pFile);
                    #else
                    logger.LogInfo("%s() %s", m_pScope, message);
                    #endif
                }
                break;

                case LOG_ERROR:
                {
                    #ifdef LOG_EXTREME_VERBOSITY
                    logger.LogError("%s() %s ( %d : %s )", m_pScope, message, m_dLine, m_pFile);
                    #else
                    logger.LogError("%s() %s", m_pScope, message);
                    #endif
                }
                break;

                case LOG_CRITICAL_ERROR:
                {
                    #ifdef LOG_EXTREME_VERBOSITY
                    logger.LogCriticalError("%s() %s ( %d : %s )", m_pScope, message, m_dLine, m_pFile);
                    #else
                    logger.LogCriticalError("%s() %s", m_pScope, message);
                    #endif
                }
                break;
            }

            b_WroteMessage = true;
        }
    }


    ~FnLog()
    {
        if(!b_WroteMessage)
        {
            #ifdef LOG_EXTREME_VERBOSITY
            logger.LogInfo("%s() <---- ( %d : %s )", m_pScope, m_dLine, m_pFile);
            #else
            logger.LogInfo("%s() <----", m_pScope);
            #endif
        }
    }
};

#define CONCAT_IMPL( x, y )     x##y
#define MACRO_CONCAT( x, y )    CONCAT_IMPL( x, y )

#define LOG(l, ...) FnLog MACRO_CONCAT( __m__, __COUNTER__ )( l, LOG_INFO, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__)
#define ERROR(l, ...) FnLog MACRO_CONCAT( __m__, __COUNTER__ )( l, LOG_ERROR, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__)
#define CRITICAL_ERROR(l, ...) FnLog MACRO_CONCAT( __m__, __COUNTER__ )( l, LOG_CRITICAL_ERROR, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__)
#endif //_LOGGING_H
