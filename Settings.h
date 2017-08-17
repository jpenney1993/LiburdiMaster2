#pragma once

#include "PixelFormat.h"
#include "Xu.h"

#include <cstdint>
#include <string>


class Settings
{
public:
    Settings();
    ~Settings();

    bool                                                 load(const char* file = nullptr);
    void                                                 loadDefaults();
    bool                                                 save(const char* file = nullptr);

    void                                                 mainWindowPos(int& x, int& y, int& w, int& h, bool& bMaximized) const;
    void                                                 setMainWindowPos(int x, int y, int w, int h, bool bMaximized);

    int                                                  loggingLevel() const;
    void                                                 setLoggingLevel(int level);

    void                                                 videoFormat(int& x, int& y, Vreo::pixelformat_t& pf) const;
    void                                                 setVideoFormat(int x, int y, Vreo::pixelformat_t pf);

    const char*                                          videoPath() const;
    void                                                 setVideoPath(const char* path);

    long                                                 brightness() const;
    void                                                 setBrightness(long val);
    long                                                 contrast() const;
    void                                                 setContrast(long val);
    long                                                 hue() const;
    void                                                 setHue(long val);
    long                                                 saturation() const;
    void                                                 setSaturation(long val);
    long                                                 sharpness() const;
    void                                                 setSharpness(long val);
    long                                                 blc() const;
    void                                                 setBlc(long val);
    long                                                 powerLineFreq() const;
    void                                                 setPowerLineFreq(long val);
    long                                                 zoom() const;
    void                                                 setZoom(long val);

    Vreo::ExposureMode                                   exposureMode() const;
    void                                                 setExposureMode(Vreo::ExposureMode mode);
    int                                                  evCorrection() const;
    void                                                 setEvCorection(int evc);
    Vreo::ExposureWeighting                              exposureWeighting() const;
    void                                                 setExposureWeighting(Vreo::ExposureWeighting ew);
    Vreo::ExposureSpeed                                  exposureSpeed() const;
    void                                                 setExposureSpeed(Vreo::ExposureSpeed es);
    bool                                                 adaptiveGradation() const;
    void                                                 setAdaptiveGradation(bool bOn);
    void                                                 atrSetup(unsigned int& gain, bool& bWide, unsigned int& contrast, unsigned int& chroma) const;
    void                                                 setAtrSetup(unsigned int gain, bool bWide, unsigned int contrast, unsigned int chroma);
    bool                                                 horizFlip() const;
    void                                                 setHorizFlip(bool bFlip);
    bool                                                 vertFlip() const;
    void                                                 setVertFlip(bool bFlip);
    bool                                                 imageStabilization() const;
    void                                                 setImageStabilization(bool bOn);
    unsigned int                                         shutterSpeed() const;
    void                                                 setShutterSpeed(unsigned int speed);
    unsigned int                                         gain() const;
    void                                                 setGain(unsigned int gain);
    bool                                                 faceDetection() const;
    void                                                 setFaceDetection(bool bOn);
    unsigned int                                         numberOfFaces() const;
    void                                                 setNumberOfFaces(unsigned int n);
    Vreo::WhiteBalance                                   whiteBalance() const;
    void                                                 setWhiteBalance(Vreo::WhiteBalance wb);
    bool                                                 ccOffset() const;
    void                                                 setCcOffset(bool bOn);
    void                                                 ccOffsetSetup(int& r, int& b) const;
    void                                                 setCcOffsetSetup(int r, int b);
    Vreo::CcSpeed                                        ccSpeed() const;
    void                                                 setCcSpeed(Vreo::CcSpeed speed);

    unsigned int                                         readRegister() const;
    void                                                 setReadRegister(unsigned int x);
    unsigned int                                         readBytes() const;
    void                                                 setReadBytes(unsigned int x);
    unsigned int                                         writeRegister() const;
    void                                                 setWriteRegister(unsigned int x);

private:
    Settings(const Settings& other);
    Settings& operator=(const Settings& other);

private:
    bool                                                 m_bDirty;
    bool                                                 m_bLoaded;
    std::string                                          m_fileName;

    int                                                  m_posX;
    int                                                  m_posY;
    int                                                  m_posW;
    int                                                  m_posH;
    bool                                                 m_posMaximized;

    int                                                  m_logLevel;

    int                                                  m_videoResX;
    int                                                  m_videoResY;
    Vreo::pixelformat_t                                  m_pixelFormat;

    std::string                                          m_videoPath;

    long                                                 m_brightness;
    long                                                 m_contrast;
    long                                                 m_hue;
    long                                                 m_saturation;
    long                                                 m_sharpness;
    long                                                 m_blc;
    long                                                 m_powerLineFreq;
    long                                                 m_zoom;

    int                                                  m_exposureMode;
    int                                                  m_evCorrection;
    int                                                  m_exposureWeighting;
    int                                                  m_exposureSpeed;
    bool                                                 m_adaptiveGradation;
    unsigned int                                         m_atrGain;
    bool                                                 m_atrWide;
    unsigned int                                         m_atrContrast;
    unsigned int                                         m_atrChroma;
    bool                                                 m_horizFlip;
    bool                                                 m_vertFlip;
    bool                                                 m_imageStabilization;
    unsigned int                                         m_shuterSpeed;
    unsigned int                                         m_gain;
    bool                                                 m_faceDetection;
    unsigned int                                         m_numberOfFaces;
    int                                                  m_whiteBalance;
    bool                                                 m_ccOffset;
    int                                                  m_ccR;
    int                                                  m_ccB;
    int                                                  m_ccSpeed;

    unsigned int                                         m_readRegister;
    unsigned int                                         m_readBytes;
    unsigned int                                         m_writeRegister;
};


inline void Settings::mainWindowPos(int& x, int& y, int& w, int& h, bool& bMaximized) const
{
    x = m_posX;
    y = m_posY;
    w = m_posW;
    h = m_posH;
    bMaximized = m_posMaximized;
}


inline void Settings::setMainWindowPos(int x, int y, int w, int h, bool bMaximized)
{
    m_bDirty = true;
    m_posX = x;
    m_posY = y;
    m_posW = w;
    m_posH = h;
    m_posMaximized = bMaximized;
}


inline int Settings::loggingLevel() const
{
    return m_logLevel;
}


inline void Settings::setLoggingLevel(int level)
{
    m_bDirty = true;
    m_logLevel = level;
}


inline void Settings::videoFormat(int& x, int& y, Vreo::pixelformat_t& pf) const
{
    x = m_videoResX;
    y = m_videoResY;
    pf = m_pixelFormat;
}


inline void Settings::setVideoFormat(int x, int y, Vreo::pixelformat_t pf)
{
    m_bDirty = true;
    m_videoResX = x;
    m_videoResY = y;
    m_pixelFormat = pf;
}


inline const char* Settings::videoPath() const
{
    return m_videoPath.c_str();
}


inline void Settings::setVideoPath(const char* path)
{
    m_bDirty = true;
    m_videoPath = path;
}


inline long Settings::blc() const
{
    return m_blc;
}


inline void Settings::setBlc(long val)
{
    m_blc = val;
    m_bDirty = true;
}


inline long Settings::sharpness() const
{
    return m_sharpness;
}


inline void Settings::setSharpness(long val)
{
    m_sharpness = val;
    m_bDirty = true;
}


inline long Settings::saturation() const
{
    return m_saturation;
}


inline void Settings::setSaturation(long val)
{
    m_saturation = val;
    m_bDirty = true;
}


inline long Settings::hue() const
{
    return m_hue;
}


inline void Settings::setHue(long val)
{
    m_hue = val;
    m_bDirty = true;
}


inline long Settings::contrast() const
{
    return m_contrast;
}


inline void Settings::setContrast(long val)
{
    m_contrast = val;
    m_bDirty = true;
}


inline long Settings::brightness() const
{
    return m_brightness;
}


inline void Settings::setBrightness(long val)
{
    m_brightness = val;
    m_bDirty = true;
}


inline long Settings::powerLineFreq() const
{
    return m_powerLineFreq;
}


inline void Settings::setPowerLineFreq(long val)
{
    m_powerLineFreq = val;
    m_bDirty = true;
}


inline long Settings::zoom() const
{
    return m_zoom;
}


inline void Settings::setZoom(long val)
{
    m_zoom = val;
    m_bDirty = true;
}


inline Vreo::ExposureMode Settings::exposureMode() const
{
    return Vreo::ExposureMode(m_exposureMode);
}


inline void Settings::setExposureMode(Vreo::ExposureMode mode)
{
    m_exposureMode = int(mode);
    m_bDirty = true;
}


inline int Settings::evCorrection() const
{
    return m_evCorrection;
}


inline void Settings::setEvCorection(int evc)
{
    m_evCorrection = evc;
    m_bDirty = true;
}


inline Vreo::ExposureWeighting Settings::exposureWeighting() const
{
    return Vreo::ExposureWeighting(m_exposureWeighting);
}


inline void Settings::setExposureWeighting(Vreo::ExposureWeighting ew)
{
    m_exposureWeighting = int(ew);
    m_bDirty = true;
}


inline Vreo::ExposureSpeed Settings::exposureSpeed() const
{
    return Vreo::ExposureSpeed(m_exposureSpeed);
}


inline void Settings::setExposureSpeed(Vreo::ExposureSpeed es)
{
    m_exposureSpeed = int(es);
    m_bDirty = true;
}


inline bool Settings::adaptiveGradation() const
{
    return m_adaptiveGradation;
}


inline void Settings::setAdaptiveGradation(bool bOn)
{
    m_adaptiveGradation = bOn;
    m_bDirty = true;
}


inline void Settings::atrSetup(unsigned int& gain, bool& bWide, unsigned int& contrast, unsigned int& chroma) const
{
    gain = m_atrGain;
    bWide = m_atrWide;
    contrast = m_atrContrast;
    chroma = m_atrChroma;
}


inline void Settings::setAtrSetup(unsigned int gain, bool bWide, unsigned int contrast, unsigned int chroma)
{
    m_atrGain = gain;
    m_atrWide = bWide;
    m_atrContrast = contrast;
    m_atrChroma = chroma;
    m_bDirty = true;
}


inline bool Settings::horizFlip() const
{
    return m_horizFlip;
}


inline void Settings::setHorizFlip(bool bFlip)
{
    m_bDirty = true;
    m_horizFlip = bFlip;
}


inline bool Settings::vertFlip() const
{
    return m_vertFlip;
}


inline void Settings::setVertFlip(bool bFlip)
{
    m_bDirty = true;
    m_vertFlip = bFlip;
}


inline bool Settings::imageStabilization() const
{
    return m_imageStabilization;
}


inline void Settings::setImageStabilization(bool bOn)
{
    m_imageStabilization = bOn;
    m_bDirty = true;
}


inline unsigned int Settings::shutterSpeed() const
{
    return m_shuterSpeed;
}


inline void Settings::setShutterSpeed(unsigned int speed)
{
    m_shuterSpeed = speed;
    m_bDirty = true;
}


inline unsigned int Settings::gain() const
{
    return m_gain;
}


inline void Settings::setGain(unsigned int gain)
{
    m_gain = gain;
    m_bDirty = true;
}


inline bool Settings::faceDetection() const
{
    return m_faceDetection;
}


inline void Settings::setFaceDetection(bool bOn)
{
    m_faceDetection = bOn;
    m_bDirty = true;
}


inline unsigned int Settings::numberOfFaces() const
{
    return m_numberOfFaces;
}


inline void Settings::setNumberOfFaces(unsigned int n)
{
    m_numberOfFaces = n;
    m_bDirty = true;
}


inline Vreo::WhiteBalance Settings::whiteBalance() const
{
    return Vreo::WhiteBalance(m_whiteBalance);
}


inline void Settings::setWhiteBalance(Vreo::WhiteBalance wb)
{
    m_bDirty = true;
    m_whiteBalance = int(wb);
}


inline bool Settings::ccOffset() const
{
    return m_ccOffset;
}


inline void Settings::setCcOffset(bool bOn)
{
    m_bDirty = true;
    m_ccOffset = bOn;
}


inline void Settings::ccOffsetSetup(int& r, int& b) const
{
    r = m_ccR;
    b = m_ccB;
}


inline void Settings::setCcOffsetSetup(int r, int b)
{
    m_bDirty = true;
    m_ccB = b;
    m_ccR = r;
}


inline Vreo::CcSpeed Settings::ccSpeed() const
{
    return Vreo::CcSpeed(m_ccSpeed);
}


inline void Settings::setCcSpeed(Vreo::CcSpeed speed)
{
    m_bDirty = true;
    m_ccSpeed = speed;
}


inline unsigned int Settings::readRegister() const
{
    return m_readRegister;
}


inline void Settings::setReadRegister(unsigned int x)
{
    m_bDirty = true;
    m_readRegister = x;
}


inline unsigned int Settings::readBytes() const
{
    return m_readBytes;
}


inline void Settings::setReadBytes(unsigned int x)
{
    m_bDirty = true;
    m_readBytes = x;
}


inline unsigned int Settings::writeRegister() const
{
    return m_writeRegister;
}


inline void Settings::setWriteRegister(unsigned int x)
{
    m_bDirty = true;
    m_writeRegister = x;
}



