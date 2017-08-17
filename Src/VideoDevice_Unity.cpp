#include "Logger.h"
#include "VideoDevice.h"

#include <QThread>

#include <cstdint>


#define HIBYTE(w) uint8_t(((w) & 0xff00) >> 8)
#define LOBYTE(w) uint8_t((w) & 0x00ff)


namespace Vreo
{


QString VideoDevice::propertyName(uint32_t propIndex) const
{
    switch (propIndex)
    {
    case 0x01: return QLatin1String("EX_EXPOSURE_MODE");
    case 0x02: return QLatin1String("EX_EV_CORRECTION");
    case 0x03: return QLatin1String("EX_EXPOSURE_WEIGHTING");
    case 0x04: return QLatin1String("EX_EXPOSURE_SPEED");
    case 0x05: return QLatin1String("EX_ADAPTIVE_GRADATION");
    case 0x06: return QLatin1String("EX_ATR_SETUP");
    case 0x07: return QLatin1String("EX_HORIZ_FLIP");
    case 0x08: return QLatin1String("EX_VERT_FLIP");
    case 0x09: return QLatin1String("EX_IMAGE_STABILIZATION");
    case 0x0a: return QLatin1String("EX_SHUTTER_SPEED");
    case 0x0b: return QLatin1String("EX_GAIN");
    case 0x0c: return QLatin1String("EX_AUTO_FOCUS");
    case 0x0d: return QLatin1String("EX_MANUAL_FOCUS");
    case 0x0e: return QLatin1String("EX_FACE_DETECTION");
    case 0x0f: return QLatin1String("EX_NUM_FACES");
    case 0x10: return QLatin1String("EX_FACE_INFO");
    case 0x11: return QLatin1String("EX_WHITE_BALANCE");
    case 0x12: return QLatin1String("EX_CC_OFFSET");
    case 0x13: return QLatin1String("EX_CC_OFFSET_SETUP");
    case 0x14: return QLatin1String("EX_CC_SPEED");
    case 0x15: return QLatin1String("EX_FW_REV");
    case 0x16: return QLatin1String("EX_SPI_SET_AND_STATUS");
    case 0x17: return QLatin1String("EX_SPI_RW_PAGE");
    case 0x18: return QLatin1String("EX_READ_SENSOR");
    case 0x19: return QLatin1String("EX_WRITE_SENSOR");
    }

    return QLatin1String("<unknown>");
}


bool VideoDevice::exposureMode(ExposureMode& mode)
{
    uint8_t _mode = 0;
    if (_xuGetControlValue(0x01, sizeof(_mode), &_mode))
    {
        mode = ExposureMode(_mode);
        return true;
    }

    return false;
}


bool VideoDevice::setExposureMode(ExposureMode mode)
{
    uint8_t _mode = uint8_t(mode);

    return _xuSetControlValue(0x01, sizeof(_mode), &mode);
}


bool VideoDevice::evCorrection(int& correction)
{
    int8_t _correction = 0;
    if (_xuGetControlValue(0x02, sizeof(_correction), &_correction))
    {
        correction = _correction;
        return true;
    }

    return false;
}


bool VideoDevice::setEvCorrection(int correction)
{
    int8_t _correction = int8_t(correction);

    return _xuSetControlValue(0x02, sizeof(_correction), &_correction);
}


bool VideoDevice::exposureWeighting(ExposureWeighting& ew)
{
    uint8_t _weighting = 0;
    if (_xuGetControlValue(0x03, sizeof(_weighting), &_weighting))
    {
        ew = ExposureWeighting(_weighting);
        return true;
    }

    return false;
}


bool VideoDevice::setExposureWeighting(ExposureWeighting ew)
{
    uint8_t _weighting = int8_t(ew);

    return _xuSetControlValue(0x03, sizeof(_weighting), &_weighting);
}


bool VideoDevice::exposureSpeed(ExposureSpeed& es)
{
    uint8_t _speed = 0;
    if (_xuGetControlValue(0x04, sizeof(_speed), &_speed))
    {
        es = ExposureSpeed(_speed);
        return true;
    }

    return false;
}


bool VideoDevice::setExposureSpeed(ExposureSpeed es)
{
    uint8_t _speed = int8_t(es);

    return _xuSetControlValue(0x04, sizeof(_speed), &_speed);
}


bool VideoDevice::adaptiveGradation(bool& bOn)
{
    uint8_t _ag = 0;
    if (_xuGetControlValue(0x05, sizeof(_ag), &_ag))
    {
        bOn = !!_ag;
        return true;
    }

    return false;
}


bool VideoDevice::setAdaptiveGradation(bool bOn)
{
    uint8_t _ag = bOn ? 1 : 0;

    return _xuSetControlValue(0x05, sizeof(_ag), &_ag);
}


bool VideoDevice::atrSetup(unsigned int& gain, bool& bWide, unsigned int& contrast, unsigned int& chroma)
{
    uint32_t _atr = 0;
    if (_xuGetControlValue(0x06, sizeof(_atr), &_atr))
    {
        gain = _atr & 0x000000ff;
        bWide = ((_atr & 0x0000ff00) != 0);
        contrast = ((_atr & 0x00ff0000) >> 16);
        chroma = ((_atr & 0xff000000) >> 24);

        return true;
    }

    return false;
}


bool VideoDevice::setAtrSetup(unsigned int gain, bool bWide, unsigned int contrast, unsigned int chroma)
{
    uint32_t _atr = gain | (bWide ? 0x00000100 : 0) | (contrast << 16) | (chroma << 24);

    return _xuSetControlValue(0x06, sizeof(_atr), &_atr);
}


bool VideoDevice::horizFlip(bool& bOn)
{
    uint8_t _on = 0;
    if (_xuGetControlValue(0x07, sizeof(_on), &_on))
    {
        bOn = !!_on;
        return true;
    }

    return false;
}


bool VideoDevice::setHorizFlip(bool bOn)
{
    uint8_t _on = bOn ? 1 : 0;

    return _xuSetControlValue(0x07, sizeof(_on), &_on);
}


bool VideoDevice::vertFlip(bool& bOn)
{
    uint8_t _on = 0;
    if (_xuGetControlValue(0x08, sizeof(_on), &_on))
    {
        bOn = !!_on;
        return true;
    }

    return false;
}


bool VideoDevice::setVertFlip(bool bOn)
{
    uint8_t _on = bOn ? 1 : 0;

    return _xuSetControlValue(0x08, sizeof(_on), &_on);
}



bool VideoDevice::imageStabilization(bool& bOn)
{
    uint8_t _on = 0;
    if (_xuGetControlValue(0x09, sizeof(_on), &_on))
    {
        bOn = !!_on;
        return true;
    }

    return false;
}


bool VideoDevice::setImageStabilization(bool bOn)
{
    uint8_t _on = bOn ? 1 : 0;

    return _xuSetControlValue(0x09, sizeof(_on), &_on);
}


bool VideoDevice::shutterSpeed(unsigned int& speed)
{
    uint8_t _speed = 0;
    if (_xuGetControlValue(0x0a, sizeof(_speed), &_speed))
    {
        speed = _speed;
        return true;
    }

    return false;
}


bool VideoDevice::setShutterSpeed(unsigned int speed)
{
    uint8_t _speed = uint8_t(speed);

    return _xuSetControlValue(0x0a, sizeof(_speed), &_speed);
}


bool VideoDevice::gain(unsigned int& gain)
{
    uint16_t _gain = 0;

    if (_xuGetControlValue(0x0b, sizeof(_gain), &_gain))
    {
        gain = (unsigned int)_gain;
        return true;
    }

    return false;
}


bool VideoDevice::setGain(unsigned int gain)
{
    uint16_t _gain = uint16_t(gain);

    return _xuSetControlValue(0x0b, sizeof(_gain), &_gain);
}


bool VideoDevice::setAutoFocus()
{
#if 0
    uint8_t _focus = 1;

    return _xuSetControlValue(0x0c, sizeof(_focus), &_focus);
#else
    uint8_t data[2];
    data[0] = 0x04;
    data[1] = 0x31;
    if (writeSensor(0x0001, sizeof(data), data))
    {
        QThread::msleep(2000);
        data[0] = 0x00;
        data[1] = 0x33;
        return writeSensor(0x0001, sizeof(data), data);
    }
    else
    {
        return false;
    }
#endif
}


bool VideoDevice::setManualFocus(unsigned int focus)
{
    uint8_t _focus = uint8_t(focus);

    return _xuSetControlValue(0x0d, sizeof(_focus), &_focus);
}


bool VideoDevice::faceDetection(bool& bOn)
{
    uint8_t _on = 0;

    if (_xuGetControlValue(0x0e, sizeof(_on), &_on))
    {
        bOn = !!_on;
        return true;
    }

    return false;
}


bool VideoDevice::setFaceDetection(bool bOn)
{
    uint8_t _on = bOn ? 1 : 0;

    return _xuSetControlValue(0x0e, sizeof(_on), &_on);
}


bool VideoDevice::numberOfFaces(unsigned int& faces)
{
    uint8_t _faces = 0;

    if (_xuGetControlValue(0x0f, sizeof(_faces), &_faces))
    {
        faces = (unsigned int)_faces;
        return true;
    }

    return false;
}


bool VideoDevice::setNumberOfFaces(unsigned int faces)
{
    uint8_t _faces = uint8_t(faces);

    return _xuSetControlValue(0x0f, sizeof(_faces), &_faces);
}


bool VideoDevice::faceInfo(FaceInfoSet& info)
{
    uint8_t buffer[56];

    if (!_xuGetControlValue(0x10, sizeof(buffer), buffer))
    {
        return false;
    }

    for (int iFace = 0; iFace < MaxFaces; iFace++)
    {
        uint8_t* p = buffer + iFace * (2 + 2 + 2 + 1);
        info.info[iFace].posX = *(uint16_t*)p;
        info.info[iFace].posY = *(uint16_t*)(p + 2);
        info.info[iFace].size = *(uint16_t*)(p + 4);
        info.info[iFace].angle = *(p + 6);
    }

    return true;
}


bool VideoDevice::whiteBalance(WhiteBalance& wb)
{
    uint8_t _wb = 0;
    if (_xuGetControlValue(0x11, sizeof(_wb), &_wb))
    {
        wb = WhiteBalance(_wb);
        return true;
    }

    return false;
}


bool VideoDevice::setWhiteBalance(WhiteBalance wb)
{
    uint8_t _wb = uint8_t(wb);

    return _xuSetControlValue(0x11, sizeof(_wb), &_wb);
}


bool VideoDevice::ccOffset(bool& bOn)
{
    uint8_t _cco = 0;
    if (_xuGetControlValue(0x12, sizeof(_cco), &_cco))
    {
        bOn = !!_cco;
        return true;
    }

    return false;
}


bool VideoDevice::setCcOffset(bool bOn)
{
    uint8_t _cco = bOn ? 1 : 0;

    return _xuSetControlValue(0x12, sizeof(_cco), &_cco);
}


bool VideoDevice::ccOffsetSetup(int& r, int& b)
{
    uint16_t w = 0;

    if (_xuGetControlValue(0x13, sizeof(w), &w))
    {
        r = (int)(w & 0x00ff);
        b = (int)((w & 0xff00) >> 8);

        return true;
    }

    return false;
}


bool VideoDevice::setCcOffsetSetup(int r, int b)
{
    uint16_t w = uint16_t(r) | (uint16_t(b) << 8);

    return _xuSetControlValue(0x13, sizeof(w), &w);
}


bool VideoDevice::ccSpeed(CcSpeed& speed)
{
    uint8_t _speed = 0;

    if (_xuGetControlValue(0x14, sizeof(_speed), &_speed))
    {
        speed = CcSpeed(_speed);
        return true;
    }

    return false;

}


bool VideoDevice::setCcSpeed(CcSpeed speed)
{
    uint8_t _speed = uint8_t(speed);

    return _xuSetControlValue(0x14, sizeof(_speed), &_speed);
}


bool VideoDevice::firmwareRevision(QString& rev)
{
    char temp[9];
    ::memset(temp, 0, sizeof(temp));

    if (_xuGetControlValue(0x15, 8, temp))
    {
        rev = QString::fromLatin1(temp);
        return true;
    }

    return false;
}


bool VideoDevice::readSensor(unsigned int address, unsigned int size, void* buffer)
{
    if (size > 61) return false;

    uint8_t temp[64];
    ::memset(temp, 0, sizeof(temp));
    temp[0] = HIBYTE(address);
    temp[1] = LOBYTE(address);
    temp[2] = uint8_t(size);

    if (!_xuSetControlValue(0x18, 64, temp))
    {
        return false;
    }

    if (!_xuGetControlValue(0x18, 64, temp))
    {
        return false;
    }

    ::memcpy(buffer, temp, size);

    return true;
}


bool VideoDevice::writeSensor(unsigned int address, unsigned int size, const void* buffer)
{
    if (size > 61) return false;

    uint8_t temp[64];
    ::memset(temp, 0, sizeof(temp));
    temp[0] = HIBYTE(address);
    temp[1] = LOBYTE(address);
    temp[2] = uint8_t(size);
    ::memcpy(&temp[3], buffer, size);

    return _xuSetControlValue(0x19, 64, temp);
}


bool VideoDevice::coldReboot()
{
    uint8_t data[5];
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0x02;

    return _xuSetControlValue(0x16, sizeof(data), &data);
}


bool VideoDevice::eraseSector(unsigned int sector)
{
    uint8_t data[5];
    data[0] = 0;
    data[1] = uint8_t(sector);
    data[2] = 0;
    data[3] = 0;
    data[4] = 0x01;

    return _xuSetControlValue(0x16, sizeof(data), &data);
}


bool VideoDevice::setCurrentPage(unsigned int page)
{
    uint8_t data[5];
    data[0] = 0;
    data[1] = 0;
    data[2] = LOBYTE(page);
    data[3] = HIBYTE(page);
    data[4] = 0x00;

    return _xuSetControlValue(0x16, sizeof(data), &data);
}


bool VideoDevice::getSpiStatus(SpiStatus& status)
{
    uint8_t data[5];

    if (!_xuGetControlValue(0x16, sizeof(data), data))
    {
        return false;
    }

    status = SpiStatus(data[0]);

    return true;
}


bool VideoDevice::readPage(Page& page)
{
    return _xuGetControlValue(0x17, PageSize, page.data);
}


bool VideoDevice::writePage(const Page& page)
{
    return _xuSetControlValue(0x17, PageSize, page.data);
}


}
