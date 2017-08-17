#pragma once

#include "Buffer.h"
#include "DeviceDesc.h"
#include "ErrorSourceBase.h"
#include "FrameInterval.h"
#include "ReferenceCounted.h"
#include "UvcControls.h"
#include "VideoFormat.h"
#include "VideoSource.h"
#include "Xu.h"

#include <atomic>
#include <cstdint>
#include <linux/videodev2.h>
#include <memory>
#include <mutex>
#include <vector>

#include <QObject>
#include <QString>


namespace Vreo
{




class VideoDevice : public QObject, public Vreo::ErrorSourceBase, public Vreo::IVideoSource, public Vreo::IUvcControls, public Vreo::IXuControls
{
    Q_OBJECT
    Q_INTERFACES(Vreo::IVideoSource Vreo::IErrorSource Vreo::IUvcControls Vreo::IXuControls)

public:
    explicit VideoDevice(const QString& path, const QString& vid, const QString& pid);

    // IVideoSource members
    void                                 addRef() throw() override;
    void                                 release() throw() override;

    bool                                 startCapture() override;
    void                                 stopCapture() override;
    bool                                 readFrame(Vreo::VideoFrame& frame) override;

    // IUvcControls members
    bool                                 brightnessRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) override;
    bool                                 brightness(int32_t& val) override;
    bool                                 setBrightness(int32_t val) override;

    bool                                 contrastRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) override;
    bool                                 contrast(int32_t& val) override;
    bool                                 setContrast(int32_t val) override;

    bool                                 hueRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) override;
    bool                                 hue(int32_t& val) override;
    bool                                 setHue(int32_t val) override;

    bool                                 saturationRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) override;
    bool                                 saturation(int32_t& val) override;
    bool                                 setSaturation(int32_t val) override;

    bool                                 sharpnessRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) override;
    bool                                 sharpness(int32_t& val) override;
    bool                                 setSharpness(int32_t val) override;

    bool                                 blcRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) override;
    bool                                 blc(int32_t& val) override;
    bool                                 setBlc(int32_t val) override;

    bool                                 powerLineFreq(Vreo::PowerLineFreq& f) override;
    bool                                 setPowerLineFreq(Vreo::PowerLineFreq f) override;

    bool                                 zoomRange(int32_t& min, int32_t& max, int32_t& def, int32_t& step) override;
    bool                                 zoom(int32_t& val) override;
    bool                                 setZoom(int32_t val) override;

    // IXuControls members
    QString                              propertyName(uint32_t index) const override;

    bool                                 exposureMode(Vreo::ExposureMode& mode) override;
    bool                                 setExposureMode(Vreo::ExposureMode mode) override;

    bool                                 evCorrection(int& correction) override;
    bool                                 setEvCorrection(int correction) override;

    bool                                 exposureWeighting(Vreo::ExposureWeighting& ew) override;
    bool                                 setExposureWeighting(Vreo::ExposureWeighting ew) override;

    bool                                 exposureSpeed(Vreo::ExposureSpeed& es) override;
    bool                                 setExposureSpeed(Vreo::ExposureSpeed es) override;

    bool                                 adaptiveGradation(bool& bOn) override;
    bool                                 setAdaptiveGradation(bool bOn) override;

    bool                                 atrSetup(unsigned int& gain, bool& bWide, unsigned int& contrast, unsigned int& chroma) override;
    bool                                 setAtrSetup(unsigned int gain, bool bWide, unsigned int contrast, unsigned int chroma) override;

    bool                                 horizFlip(bool& bOn) override;
    bool                                 setHorizFlip(bool bOn) override;

    bool                                 vertFlip(bool& bOn) override;
    bool                                 setVertFlip(bool bOn) override;

    bool                                 imageStabilization(bool& bOn) override;
    bool                                 setImageStabilization(bool bOn) override;

    bool                                 shutterSpeed(unsigned int& speed) override;
    bool                                 setShutterSpeed(unsigned int speed) override;

    bool                                 gain(unsigned int& gain) override;
    bool                                 setGain(unsigned int gain) override;

    bool                                 setAutoFocus() override;
    bool                                 setManualFocus(unsigned int focus) override;

    bool                                 faceDetection(bool& bOn) override;
    bool                                 setFaceDetection(bool bOn) override;

    bool                                 numberOfFaces(unsigned int& faces) override;
    bool                                 setNumberOfFaces(unsigned int faces) override;

    bool                                 faceInfo(Vreo::FaceInfoSet& info) override;

    bool                                 whiteBalance(Vreo::WhiteBalance& wb) override;
    bool                                 setWhiteBalance(Vreo::WhiteBalance wb) override;

    bool                                 ccOffset(bool& bOn) override;
    bool                                 setCcOffset(bool bOn) override;

    bool                                 ccOffsetSetup(int& r, int& b) override;
    bool                                 setCcOffsetSetup(int r, int b) override;

    bool                                 ccSpeed(Vreo::CcSpeed& speed) override;
    bool                                 setCcSpeed(Vreo::CcSpeed speed) override;

    bool                                 firmwareRevision(QString& rev) override;

    bool                                 readSensor(unsigned int address, unsigned int size, void* buffer) override;
    bool                                 writeSensor(unsigned int address, unsigned int size, const void* buffer) override;

    bool                                 coldReboot() override;
    bool                                 eraseSector(unsigned int sector) override;
    bool                                 setCurrentPage(unsigned int page) override;
    bool                                 getSpiStatus(Vreo::SpiStatus& status) override;
    bool                                 readPage(Vreo::Page& page) override;
    bool                                 writePage(const Vreo::Page& page) override;

    QString                              path() const;
    QString                              name() const;
    QString                              vid() const;
    QString                              pid() const;

    static void                          enumerateDevices(std::vector<Vreo::DeviceDesc>& devices);

    bool                                 enumeratePixelFormats(std::vector<Vreo::pixelformat_t>& formats);
    bool                                 enumerateResolutions(Vreo::pixelformat_t pf, std::vector<Vreo::VideoFormat>& resolutions);
    bool                                 enumerateFrameIntervals(const Vreo::VideoFormat& vf, std::vector<Vreo::FrameInterval>& intervals);

    bool                                 currentFormat(Vreo::VideoFormat& vf);
    bool                                 setCurrentFormat(const Vreo::VideoFormat& vf);

    void                                 requestStopStream();

private:
    ~VideoDevice();

    int                                  _ioctl(int ctl, void* arg);
    bool                                 _createBuffers();
    void                                 _releaseBuffers();
    bool                                 _getFrame();
    bool                                 _checkFrameAvailable();
    bool                                 _uvcControlRange(uint32_t id, int32_t& min, int32_t& max, int32_t& def, int32_t& step);
    bool                                 _uvcControlValue(uint32_t id, int32_t& val);
    bool                                 _uvcSetControlValue(uint32_t id, int32_t val);
    bool                                 _xuGetControlLength(uint8_t idControl, uint16_t& length);
    bool                                 _xuGetControlMin(uint8_t idControl, uint16_t length, void* min);
    bool                                 _xuGetControlMax(uint8_t idControl, uint16_t length, void* max);
    bool                                 _xuGetControlDefault(uint8_t idControl, uint16_t length, void* def);
    bool                                 _xuGetControlStep(uint8_t idControl, uint16_t length, void* step);
    bool                                 _xuGetControlValue(uint8_t idControl, uint16_t length, void* val);
    bool                                 _xuSetControlValue(uint8_t idControl, uint16_t length, const void* val);
    int32_t                              _convertZoomToUnity(int32_t zoom) const;
    int32_t                              _convertZoomFromUnity(int32_t zoom) const;


private:
    VideoDevice(const VideoDevice&);
    VideoDevice& operator=(const VideoDevice&);

private:
    enum
    {
        BufferCount = 8
    };

    enum class StreamingState
    {
        Ok,
        Stopped,
        StopRequested
    };

    std::mutex                           m_lock;
    std::atomic<long>                    m_refs;
    int                                  m_handle;
    QString                              m_path;
    QString                              m_name;
    QString                              m_vid;
    QString                              m_pid;
    int                                  m_errorCode;
    int                                  m_bufferCount;
    std::unique_ptr<Vreo::Buffer>        m_buffers[BufferCount];
    StreamingState                       m_streamingState;
    Vreo::VideoFormat                    m_vf;
    int8_t                               m_xuid;
    int32_t                              m_minZoom;
    int32_t                              m_maxZoom;
};


typedef ReferenceCountedPtr<Vreo::VideoDevice> VideoDevicePtr;



inline QString VideoDevice::path() const
{
    return m_path;
}


inline QString VideoDevice::name() const
{
    return m_name;
}


inline QString VideoDevice::vid() const
{
    return m_vid;
}


inline QString VideoDevice::pid() const
{
    return m_pid;
}


inline int32_t VideoDevice::_convertZoomToUnity(int32_t zoom) const
{
    // 0 -> 64
    // 60 -> 4
    return m_maxZoom - zoom;
}


inline int32_t VideoDevice::_convertZoomFromUnity(int32_t zoom) const
{
    // 64 -> 0
    // 4 -> 60
    return m_maxZoom - zoom;
}


}
