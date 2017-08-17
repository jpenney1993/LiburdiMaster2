#include "Buffer.h"
#include "Logger.h"

#include <errno.h>
#include <libv4l2.h>
#include <sys/mman.h>


namespace Vreo
{


Buffer::~Buffer()
{
    unmap();
}


Buffer::Buffer()
: m_data(MAP_FAILED)
, m_offset(0)
, m_length(0)
{

}


bool Buffer::map(int fd, uint32_t length, uint32_t offset)
{
    unmap();

    m_data = ::v4l2_mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    if (m_data == MAP_FAILED)
    {
        return false;
    }

    m_length = length;
    m_offset = offset;

    return true;
}


void Buffer::unmap()
{
    if ((m_data != MAP_FAILED) && (m_length > 0))
    {
        ::v4l2_munmap(m_data, m_length);
        m_data = MAP_FAILED;
        m_length = 0;
        m_offset = 0;
    }
}


}
