#include "Logger.h"
#include "VideoDevice.h"

#include <errno.h>
#include <libv4l2.h>
#include <linux/v4l2-controls.h>

#include <QThread>


namespace Vreo
{


bool VideoDevice::_uvcControlRange(uint32_t id, int32_t& min, int32_t& max, int32_t& def, int32_t& step)
{
    Q_ASSERT(m_handle >= 0);

    v4l2_queryctrl ctrl;
    ::memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = id;

    if (_ioctl(VIDIOC_QUERYCTRL, &ctrl) < 0)
    {
        return false;
    }

    min = ctrl.minimum;
    max = ctrl.maximum;
    def = ctrl.default_value;
    step = ctrl.step;

    return true;
}


bool VideoDevice::_uvcControlValue(uint32_t id, int32_t& val)
{
    v4l2_control ctrl;
    ::memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = id;

    if (_ioctl(VIDIOC_G_CTRL, &ctrl) < 0)
    {
        return false;
    }

    val = ctrl.value;
    return true;
}


bool VideoDevice::_uvcSetControlValue(uint32_t id, int32_t val)
{
    v4l2_control ctrl;
    ::memset(&ctrl, 0, sizeof(ctrl));
    ctrl.id = id;
    ctrl.value = val;

    if (_ioctl(VIDIOC_S_CTRL, &ctrl) < 0)
    {
        return false;
    }

    return true;
}


bool VideoDevice::brightnessRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step)
{
    if (!_uvcControlRange(V4L2_CID_BRIGHTNESS, min, max, def, step))
    {
        setLastError(Error(errno), "Failed to read brightness control range");
        return false;
    }

    return true;
}


bool VideoDevice::brightness(int32_t& val)
{
    if (!_uvcControlValue(V4L2_CID_BRIGHTNESS, val))
    {
        setLastError(Error(errno), "Failed to read brightness control value");
        return false;
    }

    return true;
}


bool VideoDevice::setBrightness(int32_t val)
{
    if (!_uvcSetControlValue(V4L2_CID_BRIGHTNESS, val))
    {
        setLastError(Error(errno), "Failed to write brightness control value");
        return false;
    }

    return true;
}


bool VideoDevice::contrastRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step)
{
    if (!_uvcControlRange(V4L2_CID_CONTRAST, min, max, def, step))
    {
        setLastError(Error(errno), "Failed to read contrast control range");
        return false;
    }

    return true;
}


bool VideoDevice::contrast(int32_t& val)
{
    if (!_uvcControlValue(V4L2_CID_CONTRAST, val))
    {
        setLastError(Error(errno), "Failed to read contrast control value");
        return false;
    }

    return true;
}


bool VideoDevice::setContrast(int32_t val)
{
    if (!_uvcSetControlValue(V4L2_CID_CONTRAST, val))
    {
        setLastError(Error(errno), "Failed to write contrast control value");
        return false;
    }

    return true;
}


bool VideoDevice::hueRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step)
{
    if (!_uvcControlRange(V4L2_CID_HUE, min, max, def, step))
    {
        setLastError(Error(errno), "Failed to read hue control range");
        return false;
    }

    return true;
}


bool VideoDevice::hue(int32_t& val)
{
    if (!_uvcControlValue(V4L2_CID_HUE, val))
    {
        setLastError(Error(errno), "Failed to read hue control value");
        return false;
    }

    return true;
}


bool VideoDevice::setHue(int32_t val)
{
    if (!_uvcSetControlValue(V4L2_CID_HUE, val))
    {
        setLastError(Error(errno), "Failed to write hue control value");
        return false;
    }

    return true;
}


bool VideoDevice::saturationRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step)
{
    if (!_uvcControlRange(V4L2_CID_SATURATION, min, max, def, step))
    {
        setLastError(Error(errno), "Failed to read saturation control range");
        return false;
    }

    return true;
}


bool VideoDevice::saturation(int32_t& val)
{
    if (!_uvcControlValue(V4L2_CID_SATURATION, val))
    {
        setLastError(Error(errno), "Failed to read saturaion control value");
        return false;
    }

    return true;
}


bool VideoDevice::setSaturation(int32_t val)
{
    if (!_uvcSetControlValue(V4L2_CID_SATURATION, val))
    {
        setLastError(Error(errno), "Failed to write saturation control value");
        return false;
    }

    return true;
}


bool VideoDevice::sharpnessRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step)
{
    if (!_uvcControlRange(V4L2_CID_SHARPNESS, min, max, def, step))
    {
        setLastError(Error(errno), "Failed to read sharpness control range");
        return false;
    }

    return true;
}


bool VideoDevice::sharpness(int32_t& val)
{
    if (!_uvcControlValue(V4L2_CID_SHARPNESS, val))
    {
        setLastError(Error(errno), "Failed to read sharpness control value");
        return false;
    }

    return true;
}


bool VideoDevice::setSharpness(int32_t val)
{
    if (!_uvcSetControlValue(V4L2_CID_SHARPNESS, val))
    {
        setLastError(Error(errno), "Failed to write sharpness control value");
        return false;
    }

    return true;
}


bool VideoDevice::blcRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step)
{
    if (!_uvcControlRange(V4L2_CID_BACKLIGHT_COMPENSATION, min, max, def, step))
    {
        setLastError(Error(errno), "Failed to read BLC control range");
        return false;
    }

    return true;
}


bool VideoDevice::blc(int32_t& val)
{
    if (!_uvcControlValue(V4L2_CID_BACKLIGHT_COMPENSATION, val))
    {
        setLastError(Error(errno), "Failed to read BLC control value");
        return false;
    }

    return true;
}


bool VideoDevice::setBlc(int32_t val)
{
    if (!_uvcSetControlValue(V4L2_CID_BACKLIGHT_COMPENSATION, val))
    {
        setLastError(Error(errno), "Failed to write BLC control value");
        return false;
    }

    return true;
}


bool VideoDevice::powerLineFreq(PowerLineFreq& f)
{
    int32_t val = 0;
    if (!_uvcControlValue(V4L2_CID_POWER_LINE_FREQUENCY, val))
    {
        setLastError(Error(errno), "Failed to read PW line control value");
        return false;
    }

    f = PowerLineFreq(val);

    return true;
}


bool VideoDevice::setPowerLineFreq(PowerLineFreq f)
{
    int32_t val = int32_t(f);
    if (!_uvcSetControlValue(V4L2_CID_POWER_LINE_FREQUENCY, val))
    {
        setLastError(Error(errno), "Failed to write PW line control value");
        return false;
    }

    return true;
}


bool VideoDevice::zoomRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step)
{
    if (!_uvcControlRange(V4L2_CID_ZOOM_ABSOLUTE, min, max, def, step))
    {
        setLastError(Error(errno), "Failed to read zoom control range");
        return false;
    }

    min = _convertZoomFromUnity(min);
    max = _convertZoomFromUnity(max);
    def = _convertZoomFromUnity(def);

    std::swap(min, max);

    return true;
}


bool VideoDevice::zoom(int32_t& val)
{
    if (!_uvcControlValue(V4L2_CID_ZOOM_ABSOLUTE, val))
    {
        setLastError(Error(errno), "Failed to read zoom control value");
        return false;
    }

    val = _convertZoomFromUnity(val);

    return true;
}


bool VideoDevice::setZoom(int32_t val)
{
    if (!_uvcSetControlValue(V4L2_CID_ZOOM_ABSOLUTE, _convertZoomToUnity(val)))
    {
        setLastError(Error(errno), "Failed to write zoom control value");
        return false;
    }

    QThread::msleep(200);

    return true;
}


}
