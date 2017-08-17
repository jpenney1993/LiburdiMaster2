#pragma once

#include "ErrorSource.h"
#include "ReferenceCounted.h"


namespace Vreo
{

enum class PowerLineFreq
{
    fDisabled	= 0,
    f50hz	= 1,
    f60hz	= 2,
    fAuto	= 3,
};


struct IUvcControls : virtual public Vreo::IReferenceCounted, virtual public Vreo::IErrorSource
{
    virtual bool                       brightnessRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) = 0;
    virtual bool                       brightness(int32_t& val) = 0;
    virtual bool                       setBrightness(int32_t val) = 0;

    virtual bool                       contrastRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) = 0;
    virtual bool                       contrast(int32_t& val) = 0;
    virtual bool                       setContrast(int32_t val) = 0;

    virtual bool                       hueRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) = 0;
    virtual bool                       hue(int32_t& val) = 0;
    virtual bool                       setHue(int32_t val) = 0;

    virtual bool                       saturationRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) = 0;
    virtual bool                       saturation(int32_t& val) = 0;
    virtual bool                       setSaturation(int32_t val) = 0;

    virtual bool                       sharpnessRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) = 0;
    virtual bool                       sharpness(int32_t& val) = 0;
    virtual bool                       setSharpness(int32_t val) = 0;

    virtual bool                       blcRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) = 0;
    virtual bool                       blc(int32_t& val) = 0;
    virtual bool                       setBlc(int32_t val) = 0;

    virtual bool                       powerLineFreq(Vreo::PowerLineFreq& f) = 0;
    virtual bool                       setPowerLineFreq(Vreo::PowerLineFreq f) = 0;

    virtual bool                       zoomRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) = 0;
    virtual bool                       zoom(int32_t& val) = 0;
    virtual bool                       setZoom(int32_t val) = 0;

protected:
    virtual ~IUvcControls() throw() { }
};


typedef Vreo::ReferenceCountedPtr<Vreo::IUvcControls> UvcControlsPtr;


}


Q_DECLARE_INTERFACE(Vreo::IUvcControls, "com.vreo.IUvcControls")
