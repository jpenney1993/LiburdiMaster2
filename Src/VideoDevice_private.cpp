#include "Logger.h"
#include "VideoDevice.h"

#include <errno.h>
#include <libv4l2.h>


namespace Vreo
{


int VideoDevice::_ioctl(int ctl, void* arg)
{
    Q_ASSERT(m_handle >= 0);

    int ret = 0;
    int tries = 4;
    do
    {
        if (ret)
        {
            ::usleep(50);
        }
        ret = ::v4l2_ioctl(m_handle, ctl, arg);
    }
    while (ret && tries-- && ((errno == EINTR) || (errno == EAGAIN) || (errno == ETIMEDOUT) || (errno == EIO) || (errno == EPIPE)));

    return ret;
}


void VideoDevice::_releaseBuffers()
{
    Q_ASSERT(m_handle >= 0);

    // unmap
    for (int i = 0; i < m_bufferCount; i++)
    {
        if (m_buffers[i])
        {
            m_buffers[i].reset();
        }
    }

    m_bufferCount = 0;

    // VIDIOC_REQBUFS
    {
        v4l2_requestbuffers rb;
        ::memset(&rb, 0, sizeof(rb));
        rb.count = 0;
        rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        rb.memory = V4L2_MEMORY_MMAP;

        if(_ioctl(VIDIOC_REQBUFS, &rb) < 0)
        {
            setLastError(Error(errno, "IOCTL VIDIOC_REQBUFS failed"));
        }
    }
}


bool VideoDevice::_createBuffers()
{
    // VIDIOC_REQBUFS
    {
        Q_ASSERT(!m_bufferCount); // release first

        v4l2_requestbuffers rb;
        ::memset(&rb, 0, sizeof(rb));
        rb.count = BufferCount;
        rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        rb.memory = V4L2_MEMORY_MMAP;

        if (_ioctl(VIDIOC_REQBUFS, &rb) < 0)
        {
            setLastError(Error(errno, "IOCTL VIDIOC_REQBUFS failed"));
            return false;
        }

        m_bufferCount = rb.count;

        ONEVIEW_LOG_DEBUG("Requested %d buffers", rb.count);
    }

    // VIDIOC_QUERYBUF and map
    for (int i = 0; i < m_bufferCount; i++)
    {
        v4l2_buffer buffer;
        ::memset(&buffer, 0, sizeof(buffer));
        buffer.index = i;
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;

        if (_ioctl(VIDIOC_QUERYBUF, &buffer) < 0)
        {
            setLastError(Error(errno, "IOCTL VIDIOC_QUERYBUF failed"));
            return false;
        }

        std::unique_ptr<Buffer> ptrBuffer(new Buffer);
        if (!ptrBuffer->map(m_handle, buffer.length, buffer.m.offset))
        {
            setLastError(Error(errno, "Unable to map buffer"));
            return false;
        }

        ONEVIEW_LOG_DEBUG("Mapped buffer %d at %p; length %d; offset %d", i, ptrBuffer->data(), ptrBuffer->length(), ptrBuffer->offset());

        m_buffers[i].swap(ptrBuffer);
    }

    // VIDIOC_QBUF
    for (int i = 0; i < m_bufferCount; ++i)
    {
        v4l2_buffer buffer;
        ::memset(&buffer, 0, sizeof(buffer));
        buffer.index = i;
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;

        if (_ioctl(VIDIOC_QBUF, &buffer) < 0)
        {
            setLastError(Error(errno, "IOCTL VIDIOC_QBUF failed"));
            return false;
        }
    }

    ONEVIEW_LOG_DEBUG("Queued %d buffers", m_bufferCount);

    return true;
}


bool VideoDevice::_checkFrameAvailable()
{
    Q_ASSERT(m_handle >= 0);

    if(m_streamingState != StreamingState::Ok)
    {
        if(m_streamingState == StreamingState::StopRequested)
        {
            stopCapture();
        }

        ONEVIEW_LOG_ERROR("Video stream is not running");
        return false;
    }

    fd_set rdset;
    timeval timeout;
    FD_ZERO(&rdset);
    FD_SET(m_handle, &rdset);
    timeout.tv_sec = 1; // 1 sec timeout
    timeout.tv_usec = 0;
    // wait for data
    if (::select(m_handle + 1, &rdset, nullptr, nullptr, &timeout) <= 0) // 0 for timeout
    {
        setLastError(Error(errno, "select() failed"));
        return false;
    }

    return (FD_ISSET(m_handle, &rdset));
}


}



