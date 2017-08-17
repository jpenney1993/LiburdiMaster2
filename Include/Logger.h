#pragma once

#include <fstream>
#include <mutex>

#include <QPlainTextEdit>
#include <QString>


namespace Vreo
{


class Logger
{
public:
    enum
    {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Disable = 4
    };

public:
    static Logger&                instance();

    void                          write(int level, const char* text, va_list va);
    void                          write(int level, const QString& text);

    void                          writeDebug(const char* text, ...);
    void                          writeInfo(const char* text, ...);
    void                          writeWarning(const char* text, ...);
    void                          writeError(const char* text, ...);

    int                           level() const;
    void                          setLevel(int level);

    void                          indent();
    void                          unindent();

    void                          setView(QPlainTextEdit* p);

    void                          clearView();

protected:
    ~Logger();
    Logger();

    void                          _write(int level, const QString& text);
    bool                          _open();

private:
    Logger(const Logger&);
    Logger& operator=(const Logger&);

private:
    std::mutex                    m_lock;
    std::ofstream                 m_file;
    int                           m_level;
    QPlainTextEdit*               m_pView;
    int                           m_indent;

    const QLatin1String           HtmlDebug;
    const QLatin1String           HtmlInfo;
    const QLatin1String           HtmlWarning;
    const QLatin1String           HtmlError;
    const QLatin1String           HtmlEnd;
};


inline int Logger::level() const
{
    return m_level;
}


inline void Logger::writeDebug(const char* text, ...)
{
    va_list args;
    va_start(args, text);
    write(Debug, text, args);
    va_end(args);
}


inline void Logger::writeInfo(const char* text, ...)
{
    va_list args;
    va_start(args, text);
    write(Info, text, args);
    va_end(args);
}


inline void Logger::writeWarning(const char* text, ...)
{
    va_list args;
    va_start(args, text);
    write(Warning, text, args);
    va_end(args);
}


inline void Logger::writeError(const char* text, ...)
{
    va_list args;
    va_start(args, text);
    write(Error, text, args);
    va_end(args);
}



class LoggerIndentScope
{
public:
    LoggerIndentScope(const char* text, ...)
    {
        va_list args;
        va_start(args, text);
        m_text.vsprintf(text, args);
        va_end(args);

        Logger::instance().writeDebug("%s", m_text.toLocal8Bit().constData());

        Logger::instance().indent();
    }

    ~LoggerIndentScope()
    {
        Logger::instance().unindent();
    }

private:
    LoggerIndentScope(const LoggerIndentScope&);
    LoggerIndentScope& operator=(const LoggerIndentScope&);

private:
    QString                       m_text;
};



}


#ifdef ONEVIEW_ENABLE_LOGGING


#define ONEVIEW_SET_LOGGING_LEVEL(level)        Vreo::Logger::instance().setLevel(level)
#define ONEVIEW_SET_LOGGER_WINDOW(window)       Vreo::Logger::instance().setView(window)
#define ONEVIEW_CLEAR_LOGGER_WINDOW()           Vreo::Logger::instance().clearView()

#define ONEVIEW_LOG_DEBUG                       Vreo::Logger::instance().writeDebug
#define ONEVIEW_LOG_INFO                        Vreo::Logger::instance().writeInfo
#define ONEVIEW_LOG_WARNING                     Vreo::Logger::instance().writeWarning
#define ONEVIEW_LOG_ERROR                       Vreo::Logger::instance().writeError

#define ONEVIEW_LOG_DEBUG_(s)                   Vreo::Logger::instance().write(Vreo::Logger::Debug, s)
#define ONEVIEW_LOG_INFO_(s)                    Vreo::Logger::instance().write(Vreo::Logger::Info, s)
#define ONEVIEW_LOG_WARNING_(s)                 Vreo::Logger::instance().write(Vreo::Logger::Warning, s)
#define ONEVIEW_LOG_ERROR_(s)                   Vreo::Logger::instance().write(Vreo::Logger::Error, s)

#define ONEVIEW_LOG_INDENT()                    Vreo::Logger::instance().indent()
#define ONEVIEW_LOG_UNINDENT()                  Vreo::Logger::instance().unindent()

#define ONEVIEW_BLOCK_SCOPE(name)               Vreo::LoggerIndentScope _lis(name)
#define ONEVIEW_FUNCTION_SCOPE()                Vreo::LoggerIndentScope _lis(__PRETTY_FUNCTION__)


#else // ONEVIEW_ENABLE_LOGGING


#define ONEVIEW_SET_LOGGING_LEVEL(level)        ((void)0)
#define ONEVIEW_SET_LOGGER_WINDOW(window)       ((void)0)
#define ONEVIEW_CLEAR_LOGGER_WINDOW()           ((void)0)

inline void ONEVIEW_LOG_DEBUG(const char*, ...) { }
inline void ONEVIEW_LOG_INFO(const char*, ...) { }
inline void ONEVIEW_LOG_WARNING(const char*, ...) { }
inline void ONEVIEW_LOG_ERROR(const char*, ...) { }

#define ONEVIEW_LOG_DEBUG_(s)                   ((void)0)
#define ONEVIEW_LOG_INFO_(s)                    ((void)0)
#define ONEVIEW_LOG_WARNING_(s)                 ((void)0)
#define ONEVIEW_LOG_ERROR_(s)                   ((void)0)

#define ONEVIEW_LOG_INDENT()                    ((void)0)
#define ONEVIEW_LOG_UNINDENT()                  ((void)0)

#define ONEVIEW_BLOCK_SCOPE(name)               ((void)0)
#define ONEVIEW_FUNCTION_SCOPE()                ((void)0)

#endif // ONEVIEW_ENABLE_LOGGING
