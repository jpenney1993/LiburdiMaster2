#pragma once


#include "Error.h"

#include <QObject>

namespace Vreo
{


class IErrorSource
{
public:
    virtual Vreo::Error                        lastError() const = 0;

protected:
    virtual ~IErrorSource() { }
};



}


Q_DECLARE_INTERFACE(Vreo::IErrorSource, "com.vreo.IErrorSource")
