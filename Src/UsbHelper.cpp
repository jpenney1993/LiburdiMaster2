#include "guid.h"
#include "Logger.h"
#include "UsbHelper.h"


#include <cstdio>

#include <libusb-1.0/libusb.h>


#define USB_VIDEO_CONTROL		1


struct __attribute__ ((__packed__)) xu_descriptor
{
    int8_t bLength;
    int8_t bDescriptorType;
    int8_t bDescriptorSubType;
    int8_t bUnitID;
    Vreo::GUID guidExtensionCode;
};


namespace Vreo
{



bool findXuId(const QString& vid, const QString& pid, const Vreo::GUID& guid, int8_t& xuid)
{
    int _vid = 0;
    int _pid = 0;
    ::sscanf(vid.toLocal8Bit().constData(), "%x", &_vid);
    ::sscanf(pid.toLocal8Bit().constData(), "%x", &_pid);

    libusb_context* context = nullptr;

    int rc = ::libusb_init(&context);
    Q_ASSERT(rc == 0);
    if (rc != 0)
    {
        return false;
    }

    libusb_device** list = nullptr;
    ssize_t count = ::libusb_get_device_list(context, &list);
    Q_ASSERT(count > 0);

    xuid = -1;

    for (ssize_t idx = 0; idx < count; ++idx)
    {
        libusb_device* device = list[idx];
        libusb_device_descriptor desc;

        rc = ::libusb_get_device_descriptor(device, &desc);
        Q_ASSERT(rc == 0);

        if ((_vid == desc.idVendor) && (_pid == desc.idProduct))
        {
            for (uint8_t i = 0; i < desc.bNumConfigurations; ++i)
            {
                libusb_config_descriptor* config = nullptr;
                if (::libusb_get_config_descriptor(device, i, &config) == 0)
                {
                    for (uint8_t j = 0; j < config->bNumInterfaces; j++)
                    {
                        for (uint8_t k = 0; k < config->interface[j].num_altsetting; k++)
                        {
                            const libusb_interface_descriptor*interface = &config->interface[j].altsetting[k];
                            if (interface->bInterfaceClass != LIBUSB_CLASS_VIDEO || interface->bInterfaceSubClass != USB_VIDEO_CONTROL)
                            {
                                continue;
                            }

                            const uint8_t* ptr = interface->extra;
                            while (int(ptr - interface->extra + sizeof(xu_descriptor)) < interface->extra_length)
                            {
                                xu_descriptor* desc = (xu_descriptor*)ptr;

                                Vreo::GUID _guid = desc->guidExtensionCode;
                                if (_guid == guid)
                                {

                                    xuid = desc->bUnitID;
                                    ::libusb_free_config_descriptor(config);
                                    ::libusb_free_device_list(list, 1);
                                    ::libusb_exit(context);
                                    return (xuid != -1);
                                }

                                ptr += desc->bLength;
                            }
                        }
                    }
                }
            }
        }
    }

    ::libusb_free_device_list(list, 1);
    ::libusb_exit(context);

    return false;

}


}
