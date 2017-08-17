#include "Error.h"

#include <string.h>


namespace Vreo
{



QString Error::toString() const
{
    QString e = (m_code != -1) ? QString::fromLocal8Bit(::strerror(m_code)) : QString();
    if (m_message.isEmpty())
    {
        return e;
    }

    return QString("%1: %2").arg(m_message).arg(e);
}


}
