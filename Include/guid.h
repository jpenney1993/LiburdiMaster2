#pragma once


#include <cstdint>
#include <string.h>

#include <QString>


namespace Vreo
{


struct __attribute__ ((__packed__)) GUID
{
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];

    bool operator==(const GUID& other) const;
    bool operator!=(const GUID& other) const;
    bool operator<(const GUID& other) const;

    QString toString() const;
};


inline bool GUID::operator==(const GUID& other) const
{
    return (::memcmp(this, &other, sizeof(GUID)) == 0);
}


inline bool GUID::operator!=(const GUID& other) const
{
    return (::memcmp(this, &other, sizeof(GUID)) != 0);
}


inline bool GUID::operator<(const GUID& other) const
{
    return (::memcmp(this, &other, sizeof(GUID)) < 0);
}


inline QString GUID::toString() const
{
    QString s;
    s.sprintf("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", Data1, Data2, Data3, Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]);
    return s;
}


}
