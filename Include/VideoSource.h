#pragma once

#include "ErrorSource.h"
#include "VideoFrame.h"
#include "ReferenceCounted.h"


namespace Vreo
{


struct IVideoSource : virtual public Vreo::IReferenceCounted, virtual public Vreo::IErrorSource
{
    virtual bool                       startCapture() = 0;
    virtual void                       stopCapture() = 0;
    virtual bool                       readFrame(Vreo::VideoFrame& frame) = 0;

protected:
    virtual ~IVideoSource() throw() { }
};


typedef Vreo::ReferenceCountedPtr<Vreo::IVideoSource> VideoSourcePtr;


}


Q_DECLARE_INTERFACE(Vreo::IVideoSource, "com.vreo.IVideoSource")
