#include "VideoBuffer.h"

#include <cstring>
#include <malloc.h>


namespace Vreo
{


void VideoBuffer::addRef() throw()
{
    m_refs++;
}


void VideoBuffer::release() throw()
{
    if (!--m_refs)
    {
        delete this;
    }
}


VideoBuffer::~VideoBuffer()
{
    if (m_data)
    {
        ::free(m_data);
    }
}


VideoBuffer::VideoBuffer(size_t size)
: m_refs(1)
{
    m_data = ::malloc(size);
    if (m_data)
    {
        m_size = size;
    }
    else
    {
        m_size = 0;
    }
}


VideoBuffer::VideoBuffer(const void* ptr, size_t size)
: m_refs(1)
{
    m_data = ::malloc(size);
    if (m_data)
    {
        if (ptr)
        {
            ::memcpy(m_data, ptr, size);
        }

        m_size = size;
    }
    else
    {
        m_size = 0;
    }
}



}
