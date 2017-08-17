#include "Logger.h"
#include "Yuyv2Rgb.h"


namespace Vreo
{


void Yuyv2Rgb::addRef() throw()
{
    m_refs++;
}


void Yuyv2Rgb::release() throw()
{
    if (!--m_refs)
    {
        delete this;
    }
}


Yuyv2Rgb::~Yuyv2Rgb()
{

}


Yuyv2Rgb::Yuyv2Rgb(IVideoSource* source)
: m_refs(1)
, m_source(source, true)
{
    Q_ASSERT(source);
}


bool Yuyv2Rgb::startCapture()
{
    return m_source->startCapture();
}


void Yuyv2Rgb::stopCapture()
{
    return m_source->stopCapture();
}


#define CLIP(value) (uint8_t)(((value) > 0xff) ? 0xff : (((value) < 0) ? 0 : (value)))


bool Yuyv2Rgb::readFrame(VideoFrame& frm)
{
    VideoFrame frmSrc;
    if (!m_source->readFrame(frmSrc))
    {
        frm = VideoFrame();
        return false;
    }

    if (!frmSrc.isValid())
    {
        ONEVIEW_LOG_WARNING("Invalid frame");
        frm = VideoFrame();
        return false;
    }

    if (frmSrc.format().pf != pfYUYV)
    {
        ONEVIEW_LOG_ERROR("Unsupported input frame format");
        frm = VideoFrame();
        return false;
    }

    VideoFormat vf(pfRGB24, frmSrc.format().width, frmSrc.format().height);
    VideoFrame frmDst(vf);
    if (!frmDst.isValid())
    {
        ONEVIEW_LOG_WARNING("Failed to allocate a video frame");
        frm = VideoFrame();
        return false;
    }

    size_t sizeYUV = frmSrc.format().height * frmSrc.format().width * 2; // 2 bytes per pixel
    if (frmSrc.size() < sizeYUV)
    {
        ONEVIEW_LOG_WARNING("Invalid frame size %d", frmSrc.size());
        frm = VideoFrame();
        return false;
    }

    uint8_t* prgb = (uint8_t*)frmDst.data();
    const uint8_t* pyuv = (const uint8_t*)frmSrc.data();

    for(size_t l = 0; l < sizeYUV; l = l + 4)
    {	// iterate every 4 bytes
        // standart: r = y0 + 1.402 (v-128)
        // logitech: r = y0 + 1.370705 (v-128)
        *prgb++ = CLIP(pyuv[l] + 1.402 * (pyuv[l + 3] - 128));
        // standart: g = y0 - 0.34414 (u-128) - 0.71414 (v-128)
        // logitech: g = y0 - 0.337633 (u-128)- 0.698001 (v-128)
        *prgb++ = CLIP(pyuv[l] - 0.34414 * (pyuv[l + 1] - 128) - 0.71414 * (pyuv[l + 3] - 128));
        // standart: b = y0 + 1.772 (u-128)
        // logitech: b = y0 + 1.732446 (u-128)
        *prgb++ = CLIP(pyuv[l] + 1.772 * (pyuv[l + 1] - 128));
        // standart: r1 =y1 + 1.402 (v-128)
        // logitech: r1 = y1 + 1.370705 (v-128)
        *prgb++ = CLIP(pyuv[l + 2] + 1.402 * (pyuv[l + 3] - 128));
        // standart: g1 = y1 - 0.34414 (u-128) - 0.71414 (v-128)
        // logitech: g1 = y1 - 0.337633 (u-128)- 0.698001 (v-128)
        *prgb++ = CLIP(pyuv[l + 2] - 0.34414 * (pyuv[l + 1] - 128) - 0.71414 * (pyuv[l + 3] - 128));
        // standart: b1 = y1 + 1.772 (u-128)
        // logitech: b1 = y1 + 1.732446 (u-128)
        *prgb++ = CLIP(pyuv[l + 2] + 1.772 * (pyuv[l + 1] - 128));
    }

    frm = frmDst;
    frm.setTimestamp(frmSrc.timestamp());

    return true;
}


}
