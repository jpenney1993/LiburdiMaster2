#pragma once

#include <cstdint>

#include <QString>


namespace Vreo
{


QString byteArrayToString(const void* data, size_t size);

QString byteArrayToStringDebug(const void* data, size_t size);


}
