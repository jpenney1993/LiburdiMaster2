#pragma once

#include "ErrorSource.h"
#include "Logger.h"


namespace Vreo
{



class ErrorSourceBase : virtual public Vreo::IErrorSource
{
public:
    ~ErrorSourceBase() { }
    ErrorSourceBase() { }

    void                                 setLastError(const Vreo::Error& e) { m_error = e; }
    void                                 setLastError(const Vreo::Error& e, const char* msg, ...);

    const char*                          errorText() const { return m_error.toString().toLocal8Bit().constData(); }

    virtual Vreo::Error                  lastError() const override { return m_error; }

private:
    Vreo::Error                          m_error;
};


inline void ErrorSourceBase::setLastError(const Vreo::Error& e, const char* msg, ...)
{
    setLastError(e);

    va_list args;
    va_start(args, msg);

    QString s;
    s.vsprintf(msg, args);
    s.append(QLatin1String(": "));
    s.append(e.toString());

    ONEVIEW_LOG_ERROR_(s);

    va_end(args);
}


}
