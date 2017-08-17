#pragma once


#include <cstdint>
#include <QString>


namespace Vreo
{


typedef uint32_t pixelformat_t;


#define pixelFormatFromFourcc(a, b, c, d) \
    ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))



enum
{
    pfRGB24 = pixelFormatFromFourcc('R', 'G', 'B', '3'),
    pfYUYV = pixelFormatFromFourcc('Y', 'U', 'Y', 'V')
};



inline QString pixelFormatToString(Vreo::pixelformat_t pf)
{
    const char* p = (const char*)&pf;
    for (int i = 0; i < 4; i++)
    {
        if (!::isalnum(p[i]))
        {
            QString s;
            s.sprintf("%u", pf);
            return s;
        }
    }

    QString s;
    s.sprintf("%c%c%c%c", p[0], p[1], p[2], p[3]);
    return s;
}



}
