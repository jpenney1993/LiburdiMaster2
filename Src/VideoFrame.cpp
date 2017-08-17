#include "VideoFrame.h"


namespace Vreo
{


VideoFrame::~VideoFrame()
{

}


VideoFrame::VideoFrame()
: m_timestamp(0)
{

}


VideoFrame::VideoFrame(const VideoFormat& vf, uint64_t timestamp, const void* data, size_t size)
: m_vf(vf)
, m_buffer(new VideoBuffer(data, size), false)
, m_timestamp(timestamp)
{

}


VideoFrame::VideoFrame(const VideoFormat& vf)
: m_vf(vf)
, m_timestamp(0)
{
    // how much space we need?
    size_t size = 0;
    switch (vf.pf)
    {
    case pfYUYV:
        size = vf.width * vf.height * 2;
        break;

    case pfRGB24:
        size = vf.width * vf.height * 3;
        break;

    default:
        Q_ASSERT(false);
        break;
    }

    if (size)
    {
        m_buffer.attach(new VideoBuffer(size));
    }
}


}
