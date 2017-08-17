#include "CoreTime.h"
#include "guid.h"
#include "Logger.h"
#include "UsbHelper.h"
#include "Util.h"
#include "VideoDevice.h"

#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <libv4l2.h>

#include <QByteArray>


//{1E05FCAF-8FC7-4d2e-A952-6085F2F602A2}
const Vreo::GUID guidExtensionCode = { 0x1E05FCAF, 0x8FC7, 0x4d2e, { 0xA9, 0x52, 0x60, 0x85, 0xF2, 0xF6, 0x02, 0xA2 } };
const uint8_t iExtensionNode = 3;



namespace Vreo
{


void VideoDevice::addRef() throw()
{
    m_refs++;
}


void VideoDevice::release() throw()
{
    if (!--m_refs)
    {
        delete this;
    }
}


VideoDevice::~VideoDevice()
{
    stopCapture();

    if (m_handle >= 0)
    {
        ::v4l2_close(m_handle);
    }
}


VideoDevice::VideoDevice(const QString& path, const QString& vid, const QString& pid)
: m_refs(1)
, m_path(path)
, m_vid(vid)
, m_pid(pid)
, m_bufferCount(0)
, m_streamingState(StreamingState::Stopped)
{
#if 1
    m_xuid = iExtensionNode;
#else
    if (findXuId(vid, pid, guidExtensionCode, m_xuid))
    {
        ONEVIEW_LOG_DEBUG("Found XU node %d {%s}", m_xuid, guidExtensionCode.toString().toLocal8Bit().constData());
    }
    else
    {
        ONEVIEW_LOG_ERROR("Could not find XU node {%s}", guidExtensionCode.toString().toLocal8Bit().constData());
    }
#endif

    QByteArray path8Bit = path.toLocal8Bit();
    m_handle = ::v4l2_open(path8Bit.constData(), O_RDWR | O_NONBLOCK, 0);
    if (m_handle < 0)
    {
        setLastError(Error(errno), "Failed to open %1", path8Bit.constData());
        return;
    }

    v4l2_capability v4l2_cap;
    if (_ioctl(VIDIOC_QUERYCAP, &v4l2_cap) < 0)
    {
        setLastError(Error(errno), "IOCTL VIDIOC_QUERYCAP failed on %s", path8Bit.constData());
        ::v4l2_close(m_handle);
        m_handle = -1;
        return;
    }

    if ((v4l2_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0)
    {
        setLastError(Error(EPERM), "Video capture is not supported for %s", path8Bit.constData());
        ::v4l2_close(m_handle);
        m_handle = -1;
        return;
    }

    if ((v4l2_cap.capabilities & V4L2_CAP_STREAMING) == 0)
    {
        setLastError(Error(EPERM), "Video streaming is not supported for %s", path8Bit.constData());
        ::v4l2_close(m_handle);
        m_handle = -1;
        return;
    }

    m_name = (const char*)v4l2_cap.card;

    currentFormat(m_vf);

    {
        // necessary to convert UNITY's inverted zoom
        int32_t def;
        int32_t step;
        if (!_uvcControlRange(V4L2_CID_ZOOM_ABSOLUTE, m_minZoom, m_maxZoom, def, step))
        {
            setLastError(Error(errno), "Failed to read zoom control range");
        }
    }
}


bool VideoDevice::enumeratePixelFormats(std::vector<pixelformat_t>& formats)
{
    std::lock_guard<std::mutex> _lk(m_lock);

    Q_ASSERT(m_handle >= 0);

    v4l2_fmtdesc fmt;

    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while (_ioctl(VIDIOC_ENUM_FMT, &fmt) >= 0)
    {
        ONEVIEW_LOG_INFO("Available pixel format: %s", (const char*)fmt.description);

        formats.push_back(fmt.pixelformat);
        fmt.index++;
    }

    return (!formats.empty());
}


bool VideoDevice::enumerateResolutions(uint32_t pf, std::vector<VideoFormat>& resolutions)
{
    std::lock_guard<std::mutex> _lk(m_lock);

    Q_ASSERT(m_handle >= 0);

    v4l2_frmsizeenum frmsize;

    frmsize.pixel_format = pf;
    frmsize.index = 0;
    while (_ioctl(VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0)
    {
        if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
        {
            ONEVIEW_LOG_INFO("Available video resolution: %dx%d", frmsize.discrete.width, frmsize.discrete.height);

            resolutions.push_back(VideoFormat(pf, frmsize.discrete.width, frmsize.discrete.height));
        }

        frmsize.index++;
    }

    return (!resolutions.empty());
}


bool VideoDevice::currentFormat(VideoFormat& vf)
{
    std::lock_guard<std::mutex> _lk(m_lock);

    Q_ASSERT(m_handle >= 0);

    v4l2_format fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    int result = _ioctl(VIDIOC_G_FMT, &fmt);
    if (result < 0)
    {
        setLastError(Error(errno), "IOCTL VIDIOC_G_FMT failed");
        return false;
    }

    vf = VideoFormat(fmt.fmt.pix.pixelformat, fmt.fmt.pix.width, fmt.fmt.pix.height);

    return true;
}


bool VideoDevice::setCurrentFormat(const VideoFormat& vf)
{
    std::lock_guard<std::mutex> _lk(m_lock);

    Q_ASSERT(m_handle >= 0);

    v4l2_format fmt;
    ::memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = vf.pf;
    fmt.fmt.pix.width = vf.width;
    fmt.fmt.pix.height = vf.height;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;

    int result = _ioctl(VIDIOC_S_FMT, &fmt);
    if (result < 0)
    {
        setLastError(Error(errno), "IOCTL VIDIOC_S_FMT failed");
        return false;
    }

    m_vf = vf;

    return true;
}


void VideoDevice::enumerateDevices(std::vector<DeviceDesc>& devs)
{
    udev* _udev = ::udev_new();
    udev_enumerate* enumerate = ::udev_enumerate_new(_udev);

    ::udev_enumerate_add_match_subsystem(enumerate, "video4linux");
    ::udev_enumerate_scan_devices(enumerate);

    udev_list_entry* devices = ::udev_enumerate_get_list_entry(enumerate);

    udev_list_entry* dev_list_entry;
    udev_list_entry_foreach(dev_list_entry, devices)
    {
        const char* path = ::udev_list_entry_get_name(dev_list_entry);
        udev_device* dev = ::udev_device_new_from_syspath(_udev, path);
        const char* v4l2_device = ::udev_device_get_devnode(dev);

        dev = ::udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
        if (!dev)
        {
            continue;
        }

        QString vid = QString::fromLocal8Bit(::udev_device_get_sysattr_value(dev, "idVendor"));
        QString pid = QString::fromLocal8Bit(::udev_device_get_sysattr_value(dev, "idProduct"));

        VideoDevicePtr vd(new VideoDevice(QString::fromLocal8Bit(v4l2_device), vid, pid), false);
        if (vd->lastError().isError())
        {
            continue;
        }

        devs.push_back(DeviceDesc(vd->path(), vd->name(), vid, pid));
        ONEVIEW_LOG_INFO("Found device %s [%s] %s:%s", vd->path().toLocal8Bit().constData(), vd->name().toLocal8Bit().constData(), vid.toLocal8Bit().constData(), pid.toLocal8Bit().constData());

        ::udev_device_unref(dev);
    }

    ::udev_enumerate_unref(enumerate);
    ::udev_unref(_udev);
}


bool VideoDevice::enumerateFrameIntervals(const VideoFormat& vf, std::vector<FrameInterval>& intervals)
{
    std::lock_guard<std::mutex> _lk(m_lock);

    Q_ASSERT(m_handle >= 0);

    v4l2_frmivalenum fival;
    ::memset(&fival, 0, sizeof(fival));

    fival.index = 0;
    fival.pixel_format = vf.pf;
    fival.width = vf.width;
    fival.height = vf.height;

    while (_ioctl(VIDIOC_ENUM_FRAMEINTERVALS, &fival) >= 0)
    {
        fival.index++;
        if (fival.type == V4L2_FRMIVAL_TYPE_DISCRETE)
        {
            intervals.push_back(FrameInterval(fival.discrete.numerator, fival.discrete.denominator));
            ONEVIEW_LOG_INFO("Available frame rate %d/%d", fival.discrete.numerator, fival.discrete.denominator);
        }
    }

    return (!intervals.empty());
}


bool VideoDevice::startCapture()
{
    ONEVIEW_FUNCTION_SCOPE();

    std::lock_guard<std::mutex> _lk(m_lock);

    Q_ASSERT(m_handle >= 0);

    if (m_streamingState == StreamingState::Ok)
    {
        ONEVIEW_LOG_DEBUG("Streaming already started");
        return true;
    }

    if (!_createBuffers()) return false;

    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (_ioctl(VIDIOC_STREAMON, &type) < 0)
    {
        setLastError(Error(errno), "IOCTL VIDIOC_STREAMON failed");
        return false;
    }

    m_streamingState = StreamingState::Ok;

    return true;
}


void VideoDevice::requestStopStream()
{
    ONEVIEW_FUNCTION_SCOPE();

    std::lock_guard<std::mutex> _lk(m_lock);

    if (m_streamingState != StreamingState::Ok)
    {
        return;
    }

    m_streamingState = StreamingState::StopRequested;
}


void VideoDevice::stopCapture()
{
    ONEVIEW_FUNCTION_SCOPE();

    std::lock_guard<std::mutex> _lk(m_lock);

    if (m_handle < 0)
    {
        return;
    }

    if (m_streamingState == StreamingState::Stopped)
    {
        return;
    }

    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (_ioctl(VIDIOC_STREAMOFF, &type) < 0)
    {
        if(errno == 9)
        {
            ONEVIEW_LOG_DEBUG("Streaming already stopped");
        }
        else
        {
            setLastError(Error(errno), "IOCTL VIDIOC_STREAMOFF failed");
            return;
        }
    }

    _releaseBuffers();

    m_streamingState = StreamingState::Stopped;
}


bool VideoDevice::readFrame(VideoFrame& frame)
{
    std::lock_guard<std::mutex> _lk(m_lock);

    Q_ASSERT(m_handle >= 0);

    if (!_checkFrameAvailable())
    {
        return false;
    }

    if (m_streamingState != StreamingState::Ok)
    {
        return false;
    }

    bool result = false;
    // VIDIOC_DQBUF
    v4l2_buffer buffer;
    {
        ::memset(&buffer, 0, sizeof(buffer));
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        if(!_ioctl(VIDIOC_DQBUF, &buffer))
        {
            frame = VideoFrame(m_vf, timeMonotonic(), m_buffers[buffer.index]->data(), buffer.bytesused);
            result = frame.isValid();
        }
        else
        {
            setLastError(Error(errno), "IOCTL VIDIOC_DQBUF failed");
            frame = VideoFrame();
        }
    }

    // VIDIOC_QBUF
    {
        if (_ioctl(VIDIOC_QBUF, &buffer) != 0)
        {
            setLastError(Error(errno), "IOCTL VIDIOC_QBUF failed");
        }
    }

    return result;
}



}
