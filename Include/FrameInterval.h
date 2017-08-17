#pragma once


namespace Vreo
{



struct FrameInterval
{
    uint32_t                   numerator;
    uint32_t                   denominator;

    FrameInterval() : numerator(0), denominator(1) { }
    FrameInterval(uint32_t n, uint32_t d) : numerator(n), denominator(d) { }

};


}
