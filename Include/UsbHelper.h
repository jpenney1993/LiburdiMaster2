#pragma once

#include "guid.h"

#include <cstdint>

#include <QString>


namespace Vreo
{

bool findXuId(const QString& vid, const QString& pid, const GUID& guid, int8_t& xuid);

}
