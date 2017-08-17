#pragma once

#include <QString>


namespace Vreo
{


struct DeviceDesc
{
    DeviceDesc()
    {
    }

    DeviceDesc(const QString& _devicePath, const QString& _deviceName, const QString& _vid, const QString& _pid)
    : path(_devicePath)
    , name(_deviceName)
    , vid(_vid)
    , pid(_pid)
    {
    }

    bool operator==(const DeviceDesc& other) const
    {
        return path == other.path;
    }

    bool operator!=(const DeviceDesc& other) const
    {
        return path == other.path;
    }

    bool operator<(const DeviceDesc& other) const
    {
        return path < other.path;
    }

    QString                      path;
    QString                      name;
    QString                      vid;
    QString                      pid;
};


}
