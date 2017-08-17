#pragma once

#include "ErrorSource.h"
#include "ReferenceCounted.h"


namespace Vreo
{


enum ExposureMode
{
    EmAuto = 0,
    EmHold = 1,
    EmManual = 2,
    EmShutterPriority = 3,
    EmGainPriority = 4
};


enum ExposureWeighting
{
    EwCentreWeighted = 0,
    EwSpot = 1,
    EwAverageWeighting = 2
};


enum ExposureSpeed
{
    EsNormal = 0,
    EsHigh = 1,
    EsLow = 2
};


enum WhiteBalance
{
    WbAuto = 0,
    WbHold = 1,
    WbATW = 2,
    WbLightBulb = 3,
    WbNeutralFluorescent = 4,
    WbClearSky = 5,
    WbCloudySky = 6,
    WbDaylightFluorescent = 9,
    WbLightbulbFluorescent = 10
};


enum CcSpeed
{
    CcNormal = 0,
    CcHigh = 1,
    CcLow = 2
};


enum
{
    MaxFaces = 8
};


struct FaceInfo
{
    unsigned int             posX;
    unsigned int             posY;
    unsigned int             size;
    unsigned int             angle;

    FaceInfo() : posX(0), posY(0), size(0), angle(0) { }
    FaceInfo(unsigned int _posX, unsigned int _posY, unsigned int _size, unsigned int _angle)
        : posX(_posX), posY(_posY), size(_size), angle(_angle) { }
};


struct FaceInfoSet
{
    FaceInfo                 info[MaxFaces];
};


enum ManualFocus
{
    MfOneStepMoveToMacro = 0xc1,
    MfTwoStepMoveToMacro = 0xc2,
    MfThreeStepMoveToMacro = 0xc3,
    MfOneStepMoveToInf = 0x41,
    MfTwoStepMoveToInf = 0x42,
    MfThreeStepMoveToInf = 0x43,
    MfGoToMacro = 0x80,
    MfGoToInf = 0x00
};


enum
{
    PageSize = 256
};


enum
{
    SectorCount = 8
};


struct Page
{
    char                  data[PageSize];
};


enum
{
    MaxPages = 65536
};


enum SpiStatus
{
    SpiReady = 0,
    SpiBusy = 1
};



struct IXuControls : virtual public Vreo::IReferenceCounted, virtual public Vreo::IErrorSource
{
public:
    virtual QString                       propertyName(uint32_t index) const = 0;

    virtual bool                          exposureMode(Vreo::ExposureMode& mode) = 0;
    virtual bool                          setExposureMode(Vreo::ExposureMode mode) = 0;
    virtual bool                          evCorrection(int& correction) = 0;
    virtual bool                          setEvCorrection(int correction) = 0;
    virtual bool                          exposureWeighting(Vreo::ExposureWeighting& ew) = 0;
    virtual bool                          setExposureWeighting(Vreo::ExposureWeighting ew) = 0;
    virtual bool                          exposureSpeed(Vreo::ExposureSpeed& es) = 0;
    virtual bool                          setExposureSpeed(Vreo::ExposureSpeed es) = 0;
    virtual bool                          adaptiveGradation(bool& bOn) = 0;
    virtual bool                          setAdaptiveGradation(bool bOn) = 0;
    virtual bool                          atrSetup(unsigned int& gain, bool& bWide, unsigned int& contrast, unsigned int& chroma) = 0;
    virtual bool                          setAtrSetup(unsigned int gain, bool bWide, unsigned int contrast, unsigned int chroma) = 0;
    virtual bool                          horizFlip(bool& bOn) = 0;
    virtual bool                          setHorizFlip(bool bOn) = 0;
    virtual bool                          vertFlip(bool& bOn) = 0;
    virtual bool                          setVertFlip(bool bOn) = 0;
    virtual bool                          imageStabilization(bool& bOn) = 0;
    virtual bool                          setImageStabilization(bool bOn) = 0;
    virtual bool                          shutterSpeed(unsigned int& speed) = 0;
    virtual bool                          setShutterSpeed(unsigned int speed) = 0;
    virtual bool                          gain(unsigned int& gain) = 0;
    virtual bool                          setGain(unsigned int gain) = 0;
    virtual bool                          setAutoFocus() = 0;
    virtual bool                          setManualFocus(unsigned int focus) = 0;
    virtual bool                          faceDetection(bool& bOn) = 0;
    virtual bool                          setFaceDetection(bool bOn) = 0;
    virtual bool                          numberOfFaces(unsigned int& faces) = 0;
    virtual bool                          setNumberOfFaces(unsigned int faces) = 0;
    virtual bool                          faceInfo(Vreo::FaceInfoSet& info) = 0;
    virtual bool                          whiteBalance(Vreo::WhiteBalance& wb) = 0;
    virtual bool                          setWhiteBalance(Vreo::WhiteBalance wb) = 0;
    virtual bool                          ccOffset(bool& bOn) = 0;
    virtual bool                          setCcOffset(bool bOn) = 0;
    virtual bool                          ccOffsetSetup(int& r, int& b) = 0;
    virtual bool                          setCcOffsetSetup(int r, int b) = 0;
    virtual bool                          ccSpeed(Vreo::CcSpeed& speed) = 0;
    virtual bool                          setCcSpeed(Vreo::CcSpeed speed) = 0;
    virtual bool                          firmwareRevision(QString& rev) = 0;
    virtual bool                          readSensor(unsigned int address, unsigned int size, void* buffer) = 0;
    virtual bool                          writeSensor(unsigned int address, unsigned int size, const void* buffer) = 0;
    virtual bool                          coldReboot() = 0;
    virtual bool                          eraseSector(unsigned int sector) = 0;
    virtual bool                          setCurrentPage(unsigned int page) = 0;
    virtual bool                          getSpiStatus(Vreo::SpiStatus& status) = 0;
    virtual bool                          readPage(Vreo::Page& page) = 0;
    virtual bool                          writePage(const Vreo::Page& page) = 0;

protected:
    virtual ~IXuControls() throw() { }
};



typedef Vreo::ReferenceCountedPtr<Vreo::IXuControls> XuControlsPtr;


}


Q_DECLARE_INTERFACE(Vreo::IXuControls, "com.vreo.IXuControls")
