#include "Util.h"


namespace Vreo
{



QString byteArrayToString(const void* p, size_t size)
{
    const uint8_t* pByte = (const uint8_t*)p;
    QString s;
    for (size_t i = 0; i < size; i++)
    {
        char t[4];
        if (i == size - 1)
        {
            ::sprintf(t, "%02x", (int)(*pByte));
        }
        else
        {
            ::sprintf(t, "%02x ", (int)(*pByte));
        }

        s.append(t);

        pByte++;
    }

    return s;
}


QString byteArrayToStringDebug(const void* p, size_t size)
{
    const uint8_t* pByte = (const uint8_t*)p;
    QString s;
    for (size_t i = 0; i < size; i++)
    {
        char t[4];
        if (i == size - 1)
        {
            ::sprintf(t, "%02x", (int)(*pByte));
        }
        else
        {
            ::sprintf(t, "%02x ", (int)(*pByte));
        }

        s.append(t);

        pByte++;
    }

    if (size == 1)
    {
        // try to treat as BYTE
        char t[32];
        uint8_t x = *(uint8_t*)p;
        int8_t y = *(int8_t*)p;
        ::sprintf(t, " (%hhu / %hhd)", x, y);
        s.append(t);
    }
    if (size == 2)
    {
        // try to treat as WORD
        char t[32];
        uint16_t x = *(uint16_t*)p;
        int16_t y = *(int16_t*)p;
        ::sprintf(t, " (%hu / %hd)", x, y);
        s.append(t);
    }
    else if (size == 4)
    {
        // try to treat as DWORD
        char t[32];
        uint32_t x = *(uint32_t*)p;
        int32_t y = *(int32_t*)p;
        ::sprintf(t, " (%u / %d)", x, y);
        s.append(t);
    }

    return s;
}


}
