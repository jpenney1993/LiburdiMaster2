#include "Logger.h"

#include <QDebug>
#include <QDir>
#include <QDateTime>


namespace Vreo
{




Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}


Logger::~Logger()
{

}


Logger::Logger()
#ifdef ONEVIEW_ENABLE_LOGGING
#ifndef QT_NO_DEBUG
: m_level(Debug)
#else
: m_level(Disable)
#endif
#else
: m_level(Disable)
#endif
, m_pView(nullptr)
, m_indent(0)
, HtmlDebug("<font color=\"#87CEFF\">")
, HtmlInfo("<font color=\"#000000\">")
, HtmlWarning("<font color=\"#0000FF\">")
, HtmlError("<font color=\"#FF0000\">")
, HtmlEnd("</font>")
{

}


bool Logger::_open()
{
    if (m_file.is_open()) return true;

    QString dir = QDir::homePath() + QLatin1String("/OneView.log");
    m_file.open(dir.toLocal8Bit().constData(), std::ofstream::trunc);

    Q_ASSERT(m_file.is_open());

    return m_file.is_open();
}


void Logger::setLevel(int level)
{
    std::lock_guard<std::mutex> lk(m_lock);

    m_level = level;
    if (m_level != Disable)
    {
        _open();
    }
}


void Logger::_write(int level, const QString& text)
{
    std::lock_guard<std::mutex> lk(m_lock);

    QString ind;
    for (int i = 0; i < m_indent; i++)
    {
        ind += QLatin1String("    ");
    }

    const char* flag = "";
    switch (level)
    {
    case Debug:
        flag = "D";
        break;

    case Info:
        flag = "I";
        break;

    case Warning:
        flag = "W";
        break;

    case Error:
        flag = "E";
        break;
    }

    QString time = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss.zzz");
    QString msg = QString("[%1 %2] %3%4").arg(time).arg(flag).arg(ind).arg(text);

    const char* charData = msg.toLocal8Bit().constData();
    if (m_file.is_open())
    {
        m_file << charData << std::endl;
    }

    switch (level)
    {
    case Debug:
    case Info:
        qDebug() << msg;
        break;

    case Warning:
        qWarning() << msg;
        break;

    case Error:
        qCritical() << msg;
        break;
    }

    if (m_pView)
    {
        QString strColor;
        switch (level) {
        case Debug:
            strColor = HtmlDebug;
            break;

        case Info:
            strColor = HtmlInfo;
            break;

        case Warning:
            strColor = HtmlWarning;
            break;

        case Error:
            strColor = HtmlError;
            break;
        }//switch()

        QString strHtml = strColor + msg.toHtmlEscaped().replace("  ", "&nbsp;&nbsp;") + HtmlEnd;

        QMetaObject::invokeMethod(m_pView, "appendHtml", Qt::QueuedConnection, Q_ARG(QString, strHtml));
    }
}


void Logger::clearView()
{
    std::lock_guard<std::mutex> lk(m_lock);

    if (m_pView)
    {
        QMetaObject::invokeMethod(m_pView, "clear", Qt::QueuedConnection);
    }
}


void Logger::write(int level, const char* text, va_list va)
{
    if (level >= Disable) return;

    if (level < m_level) return;

    QString s;
    s.vsprintf(text, va);
    _write(level, s);
}


void Logger::write(int level, const QString& text)
{
    if (level >= Disable) return;

    if (level < m_level) return;

    _write(level, text);
}


void Logger::setView(QPlainTextEdit* p)
{
    std::lock_guard<std::mutex> lk(m_lock);

    m_pView = p;
}


void Logger::indent()
{
    std::lock_guard<std::mutex> lk(m_lock);

    if (m_indent < 20)
    {
        m_indent++;
    }
}


void Logger::unindent()
{
    std::lock_guard<std::mutex> lk(m_lock);

    if (m_indent > 0)
    {
        m_indent--;
    }
}



}
