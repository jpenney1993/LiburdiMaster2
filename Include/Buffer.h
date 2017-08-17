#pragma once

#include <cstdint>

namespace Vreo
{


class Buffer
{
public:
    ~Buffer();
    Buffer();

    bool                            map(int fd, uint32_t length, uint32_t offset);
    void                            unmap();

    void*                           data() const { return m_data; }
    uint32_t                        offset() const { return m_offset; }
    uint32_t                        length() const { return m_length; }

private:
    void*                           m_data;
    uint32_t                        m_offset;
    uint32_t                        m_length;
};


}
