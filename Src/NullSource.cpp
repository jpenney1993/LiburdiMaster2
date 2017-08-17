#include "CoreTime.h"
#include "Logger.h"
#include "NullSource.h"


namespace Vreo
{


void NullSource::addRef() throw()
{
    m_refs++;
}


void NullSource::release() throw()
{
    if (!--m_refs)
    {
        delete this;
    }
}


NullSource::~NullSource()
{

}


NullSource::NullSource(const VideoFormat& vf)
: m_refs(1)
, m_vf(vf)
{
    Q_ASSERT(vf.pf != 0);
    Q_ASSERT(vf.width > 0);
    Q_ASSERT(vf.height > 0);
}


bool NullSource::startCapture()
{
    return true;
}


void NullSource::stopCapture()
{

}


bool NullSource::readFrame(VideoFrame& frm)
{
    VideoFrame frmDst(m_vf);
    if (!frmDst.isValid())
    {
        ONEVIEW_LOG_ERROR("Failed to allocate a video frame");
        return false;
    }

    uint8_t* p = (uint8_t*)frmDst.data();

    size_t cb = frmDst.size();
    while (cb)
    {
        *p++ = ::rand();
        cb--;
    }

    frm = frmDst;
    frm.setTimestamp(timeMonotonic());

    return true;
}


}
