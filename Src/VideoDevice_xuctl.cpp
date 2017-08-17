#include "guid.h"
#include "Logger.h"
#include "Util.h"
#include "VideoDevice.h"

#include <errno.h>
#include <linux/usb/video.h>
#include <linux/uvcvideo.h>


namespace Vreo
{


bool VideoDevice::_xuGetControlLength(uint8_t idControl, uint16_t& length)
{
    uvc_xu_control_query xu;

    xu.unit = m_xuid;
    xu.selector = idControl;
    xu.query = UVC_GET_LEN;
    xu.size = sizeof(length);
    xu.data = (unsigned char*)&length;

    if (_ioctl(UVCIOC_CTRL_QUERY, &xu) < 0)
    {
        setLastError(Error(errno), "Failed to get XU control %x (%s) size: %s", idControl, propertyName(idControl).toLocal8Bit().constData());
        return false;
    }

    ONEVIEW_LOG_DEBUG("Detected XU control %x (%s) length %d", idControl, propertyName(idControl).toLocal8Bit().constData(), length);

    return true;
}


bool VideoDevice::_xuGetControlMin(uint8_t idControl, uint16_t length, void* min)
{
    uvc_xu_control_query xu;

    xu.unit = m_xuid;
    xu.selector = idControl;
    xu.query = UVC_GET_MIN;
    xu.size = length;
    xu.data = (unsigned char*)min;
    if (_ioctl(UVCIOC_CTRL_QUERY, &xu) < 0)
    {
        setLastError(Error(errno), "Failed to get XU control %x (%s) min value", idControl, propertyName(idControl).toLocal8Bit().constData());
        return false;
    }

    ONEVIEW_LOG_DEBUG("Detected XU control %x (%s) min value %s", idControl, propertyName(idControl).toLocal8Bit().constData(), byteArrayToStringDebug(min, length).toLocal8Bit().constData());

    return true;
}


bool VideoDevice::_xuGetControlMax(uint8_t idControl, uint16_t length, void* max)
{
    uvc_xu_control_query xu;

    xu.unit = m_xuid;
    xu.selector = idControl;
    xu.query = UVC_GET_MAX;
    xu.size = length;
    xu.data = (unsigned char*)max;
    if (_ioctl(UVCIOC_CTRL_QUERY, &xu) < 0)
    {
        setLastError(Error(errno), "Failed to get XU control %x (%s) max value", idControl, propertyName(idControl).toLocal8Bit().constData());
        return false;
    }

    ONEVIEW_LOG_DEBUG("Detected XU control %x (%s) max value %s", idControl, propertyName(idControl).toLocal8Bit().constData(), byteArrayToStringDebug(max, length).toLocal8Bit().constData());

    return true;
}


bool VideoDevice::_xuGetControlDefault(uint8_t idControl, uint16_t length, void* def)
{
    uvc_xu_control_query xu;

    xu.unit = m_xuid;
    xu.selector = idControl;
    xu.query = UVC_GET_MIN;
    xu.size = length;
    xu.data = (unsigned char*)def;
    if (_ioctl(UVCIOC_CTRL_QUERY, &xu) < 0)
    {
        setLastError(Error(errno), "Failed to get XU control %x (%s) default value", idControl, propertyName(idControl).toLocal8Bit().constData());
        return false;
    }

    ONEVIEW_LOG_DEBUG("Detected XU control %x (%s) def value %s", idControl, propertyName(idControl).toLocal8Bit().constData(), byteArrayToStringDebug(def, length).toLocal8Bit().constData());

    return true;
}


bool VideoDevice::_xuGetControlStep(uint8_t idControl, uint16_t length, void* step)
{
    uvc_xu_control_query xu;

    xu.unit = m_xuid;
    xu.selector = idControl;
    xu.query = UVC_GET_RES;
    xu.size = length;
    xu.data = (unsigned char*)step;
    if (_ioctl(UVCIOC_CTRL_QUERY, &xu) < 0)
    {
        setLastError(Error(errno), "Failed to get XU control %x (%s) step value", idControl, propertyName(idControl).toLocal8Bit().constData());
        return false;
    }

    ONEVIEW_LOG_DEBUG("Detected XU control %x (%s) step value %s", idControl, propertyName(idControl).toLocal8Bit().constData(), byteArrayToStringDebug(step, length).toLocal8Bit().constData());

    return true;
}


bool VideoDevice::_xuGetControlValue(uint8_t idControl, uint16_t length, void* val)
{
    uvc_xu_control_query xu;

    xu.unit = m_xuid;
    xu.selector = idControl;
    xu.query = UVC_GET_CUR;
    xu.size = length;
    xu.data = (unsigned char*)val;
    if (_ioctl(UVCIOC_CTRL_QUERY, &xu) < 0)
    {
        setLastError(Error(errno), "Failed to get XU control %x (%s) current value", idControl, propertyName(idControl).toLocal8Bit().constData());
        return false;
    }

    ONEVIEW_LOG_DEBUG("Read XU control %x (%s) value %s", idControl, propertyName(idControl).toLocal8Bit().constData(), byteArrayToStringDebug(val, length).toLocal8Bit().constData());

    return true;
}


bool VideoDevice::_xuSetControlValue(uint8_t idControl, uint16_t length, const void* val)
{
    uvc_xu_control_query xu;

    xu.unit = m_xuid;
    xu.selector = idControl;
    xu.query = UVC_SET_CUR;
    xu.size = length;
    xu.data = (unsigned char*)val;
    if (_ioctl(UVCIOC_CTRL_QUERY, &xu) < 0)
    {
        setLastError(Error(errno), "Failed to write XU control %x (%s) value %s", idControl, propertyName(idControl).toLocal8Bit().constData(), byteArrayToStringDebug(val, length).toLocal8Bit().constData());
        return false;
    }

    ONEVIEW_LOG_DEBUG("Wrote XU control %x (%s) value %s", idControl, propertyName(idControl).toLocal8Bit().constData(), byteArrayToStringDebug(val, length).toLocal8Bit().constData());

    return true;
}


}
