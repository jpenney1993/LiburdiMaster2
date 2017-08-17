#pragma once


#include "PixelFormat.h"

#include <cstdint>

#include <QString>


namespace Vreo
{


struct VideoFormat
{
    Vreo::pixelformat_t                 pf;
    uint32_t                            width;
    uint32_t                            height;

    VideoFormat() : pf(0), width(0), height(0) { }
    VideoFormat(Vreo::pixelformat_t _pf, uint32_t w, uint32_t h) : pf(_pf), width(w), height(h) { }

    bool operator==(const VideoFormat& other) const
    {
        return (pf == other.pf) && (width == other.width) && (height == other.height);
    }

    bool operator!=(const VideoFormat& other) const
    {
        return (pf != other.pf) || (width != other.width) || (height != other.height);
    }

    bool operator<(const VideoFormat& other) const
    {
        if (pf < other.pf) return true;
        else if ((pf == other.pf) && (width < other.width)) return true;
        else if ((pf == other.pf) && (width == other.width) && (height < other.height)) return true;

        return false;
    }

    QString toString() const
    {
        return QString("%1 %2x%3").arg(pixelFormatToString(pf)).arg(width).arg(height);
    }
};


}
