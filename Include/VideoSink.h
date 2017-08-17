#pragma once


#include "ErrorSource.h"
#include "VideoFrame.h"
#include "ReferenceCounted.h"


namespace Vreo
{


struct IVideoSink : virtual public Vreo::IReferenceCounted, virtual public Vreo::IErrorSource
{
    virtual bool                       writeFrame(const Vreo::VideoFrame& frame) = 0;

protected:
    virtual ~IVideoSink() throw() { }
};



typedef Vreo::ReferenceCountedPtr<Vreo::IVideoSink> VideoSinkPtr;


}

Q_DECLARE_INTERFACE(Vreo::IVideoSink, "com.vreo.IVideoSink")
