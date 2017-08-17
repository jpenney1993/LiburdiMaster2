#pragma once

#include <QMetaType>
#include <QString>


namespace Vreo
{


class Error
{
public:
    ~Error() { }
    Error() : m_code(0) { }
    Error(int code) : m_code(code) { }
    Error(int code, const char* message) : m_code(code) { m_message = QString::fromLocal8Bit(message); }
    Error(int code, const QString& message) : m_code(code), m_message(message) { }
    Error(const char* message) : m_code(-1) { m_message = QString::fromLocal8Bit(message); }
    Error(const QString& message) : m_code(-1), m_message(message) { }

    bool                                isError() const { return (m_code != 0); }
    int                                 code() const { return m_code; }
    const QString&                      message() const { return m_message; }
    QString                             toString() const;

private:
    int                                 m_code;
    QString                             m_message;
};



}


Q_DECLARE_METATYPE(Vreo::Error)
