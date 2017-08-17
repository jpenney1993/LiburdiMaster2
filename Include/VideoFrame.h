#pragma once

#include "VideoBuffer.h"
#include "VideoFormat.h"

#include <cstdint>


namespace Vreo
{


class VideoFrame
{
public:
    ~VideoFrame();
    VideoFrame();
    explicit VideoFrame(const Vreo::VideoFormat& vf);
    VideoFrame(const Vreo::VideoFormat& vf, uint64_t timestamp, const void* data, size_t size);

    bool                         isValid() const;
    uint64_t                     timestamp() const;
    void*                        data() const;
    size_t                       size() const;
    const Vreo::VideoFormat&     format() const;

    void                         setTimestamp(uint64_t timestamp);

private:
    Vreo::VideoFormat            m_vf;
    Vreo::VideoBufferPtr         m_buffer;
    uint64_t                     m_timestamp;
};


inline bool VideoFrame::isValid() const
{
    return m_buffer ? m_buffer->isValid() : false;
}


inline uint64_t VideoFrame::timestamp() const
{
    return m_timestamp;
}


inline void* VideoFrame::data() const
{
    return m_buffer->data();
}


inline size_t VideoFrame::size() const
{
    return m_buffer->size();
}


inline const Vreo::VideoFormat& VideoFrame::format() const
{
    return m_vf;
}


inline void VideoFrame::setTimestamp(uint64_t timestamp)
{
    m_timestamp = timestamp;
}


}
