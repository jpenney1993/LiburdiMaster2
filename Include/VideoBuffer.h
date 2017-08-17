#pragma once

#include "ReferenceCounted.h"

#include <atomic>
#include <cstddef>


namespace Vreo
{


class VideoBuffer : public Vreo::IReferenceCounted
{
public:
    ~VideoBuffer();
    explicit VideoBuffer(size_t size);
    VideoBuffer(const void* ptr, size_t size);

    void                            addRef() throw() override;
    void                            release() throw() override;

    bool                            isValid() const;
    void*                           data() const;
    size_t                          size() const;

private:
    VideoBuffer(const VideoBuffer&);
    VideoBuffer& operator=(const VideoBuffer&);

private:
    std::atomic<long>               m_refs;
    void*                           m_data;
    size_t                          m_size;
};


typedef ReferenceCountedPtr<Vreo::VideoBuffer> VideoBufferPtr;


inline bool VideoBuffer::isValid() const
{
    return ((m_data != nullptr) && (m_size > 0));
}


inline void* VideoBuffer::data() const
{
    return m_data;
}


inline size_t VideoBuffer::size() const
{
    return m_size;
}


}
