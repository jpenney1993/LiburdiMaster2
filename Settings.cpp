#include "Logger.h"
#include "Settings.h"

#include <boost/program_options.hpp>

#include <fstream>

#include <QDebug>
#include <QDir>



Settings::Settings()
: m_bDirty(false)
, m_bLoaded(false)
{
    loadDefaults(); // at least we always have the defaults loaded
}


Settings::~Settings()
{
    if (m_bDirty)
    {
        save();
    }
}


bool Settings::load(const char* name)
{
    if (!name)
    {
        m_fileName = QDir::homePath().toLocal8Bit().constData();
        m_fileName.append("/OneView.cfg");
        name = m_fileName.c_str();
    }

    try
    {
        std::ifstream file;
        file.open(name);
        bool bLoaded = file.is_open();
        if (!bLoaded)
        {
            ONEVIEW_LOG_ERROR("Failed to open configuration file [%s]", name);
            return false;
        }

        boost::program_options::options_description description;
        description.add_options()("MainWindow.posX", boost::program_options::value<int>()->default_value(-1), "Window position X");
        description.add_options()("MainWindow.posY", boost::program_options::value<int>()->default_value(-1), "Window position Y");
        description.add_options()("MainWindow.posW", boost::program_options::value<int>()->default_value(0), "Window width");
        description.add_options()("MainWindow.posH", boost::program_options::value<int>()->default_value(0), "Window height");
        description.add_options()("MainWindow.posMaximized", boost::program_options::value<int>()->default_value(0), "Window maximized");

        description.add_options()("Logging.level", boost::program_options::value<int>()->default_value(Vreo::Logger::Disable), "Logging level");

        description.add_options()("Video.resolutionX", boost::program_options::value<int>()->default_value(640), "Video width");
        description.add_options()("Video.resolutionY", boost::program_options::value<int>()->default_value(480), "Video height");
        description.add_options()("Video.pixelFormat", boost::program_options::value<uint32_t>()->default_value(Vreo::pfYUYV), "Pixel format");

        std::string videoPath = QDir::homePath().toLocal8Bit().constData();
        description.add_options()("Video.path", boost::program_options::value<std::string>()->default_value(videoPath), "Video file path");

        description.add_options()("Std.brightness", boost::program_options::value<long>()->default_value(-1), "Brightness");
        description.add_options()("Std.contrast", boost::program_options::value<long>()->default_value(-1), "Contrast");
        description.add_options()("Std.hue", boost::program_options::value<long>()->default_value(-1), "Hue");
        description.add_options()("Std.saturation", boost::program_options::value<long>()->default_value(-1), "Saturation");
        description.add_options()("Std.sharpness", boost::program_options::value<long>()->default_value(-1), "Sharpness");
        description.add_options()("Std.blc", boost::program_options::value<long>()->default_value(-1), "Backlight Compensation");
        description.add_options()("Std.powerFreq", boost::program_options::value<long>()->default_value(-1), "Power Line Frequency");
        description.add_options()("Std.zoom", boost::program_options::value<long>()->default_value(-1), "Zoom");

        description.add_options()("Exposure.exposureMode", boost::program_options::value<int>()->default_value(0), "Exposure mode");
        description.add_options()("Exposure.evCorrection", boost::program_options::value<int>()->default_value(0), "EV correction");
        description.add_options()("Exposure.exposureWeighting", boost::program_options::value<int>()->default_value(0), "Exposure weighting");
        description.add_options()("Exposure.exposureSpeed", boost::program_options::value<int>()->default_value(0), "Exposure speed");
        description.add_options()("Exposure.shutterSpeed", boost::program_options::value<unsigned int>()->default_value(31), "Shutter speed");
        description.add_options()("Exposure.gain", boost::program_options::value<unsigned int>()->default_value(800), "Gain");

        description.add_options()("Tools.adaptiveGradation", boost::program_options::value<int>()->default_value(0), "Adaptive gradation");
        description.add_options()("Tools.atrGain", boost::program_options::value<unsigned int>()->default_value(0), "ATR gain");
        description.add_options()("Tools.atrWide", boost::program_options::value<int>()->default_value(0), "ATR wide");
        description.add_options()("Tools.atrContrast", boost::program_options::value<unsigned int>()->default_value(0), "ATR contrast");
        description.add_options()("Tools.atrChroma", boost::program_options::value<unsigned int>()->default_value(0), "ATR chroma");
        description.add_options()("Tools.imageStabilization", boost::program_options::value<int>()->default_value(0), "Image stabilization");
        description.add_options()("Tools.faceDetection", boost::program_options::value<int>()->default_value(1), "Face detection");
        description.add_options()("Tools.numberOfFaces", boost::program_options::value<unsigned int>()->default_value(1), "Number of faces");

        description.add_options()("Image.horizFlip", boost::program_options::value<int>()->default_value(0), "Horizontal flip");
        description.add_options()("Image.vertFlip", boost::program_options::value<int>()->default_value(0), "Vertical flip");

        description.add_options()("Colour.whiteBalance", boost::program_options::value<int>()->default_value(0), "White balance");
        description.add_options()("Colour.ccOffset", boost::program_options::value<int>()->default_value(0), "CC offset");
        description.add_options()("Colour.ccR", boost::program_options::value<int>()->default_value(0), "CC R");
        description.add_options()("Colour.ccB", boost::program_options::value<int>()->default_value(0), "CC B");
        description.add_options()("Colour.ccSpeed", boost::program_options::value<int>()->default_value(0), "CC Speed");

        description.add_options()("Sensor.ReadRegister", boost::program_options::value<unsigned int>()->default_value(0), "RRya");
        description.add_options()("Sensor.WriteRegister", boost::program_options::value<unsigned int>()->default_value(0), "RRyj");
        description.add_options()("Sensor.ReadBytes", boost::program_options::value<unsigned int>()->default_value(1), "RRyu");

        boost::program_options::variables_map  vm;
        boost::program_options::store(boost::program_options::parse_config_file(file, description, true), vm);
        notify(vm);

        m_posX = vm["MainWindow.posX"].as<int>();
        m_posY = vm["MainWindow.posY"].as<int>();
        m_posW = vm["MainWindow.posW"].as<int>();
        m_posH = vm["MainWindow.posH"].as<int>();
        m_posMaximized = (vm["MainWindow.posMaximized"].as<int>() != 0);

        m_logLevel = vm["Logging.level"].as<int>();

        m_videoResX = vm["Video.resolutionX"].as<int>();
        m_videoResY = vm["Video.resolutionY"].as<int>();
        m_pixelFormat = vm["Video.pixelFormat"].as<uint32_t>();

        m_videoPath = vm["Video.path"].as<std::string>();

        m_brightness = vm["Std.brightness"].as<long>();
        m_contrast = vm["Std.contrast"].as<long>();
        m_hue = vm["Std.hue"].as<long>();
        m_saturation = vm["Std.saturation"].as<long>();
        m_sharpness = vm["Std.sharpness"].as<long>();
        m_blc = vm["Std.blc"].as<long>();
        m_powerLineFreq = vm["Std.powerFreq"].as<long>();
        m_zoom = vm["Std.zoom"].as<long>();

        m_exposureMode = vm["Exposure.exposureMode"].as<int>();
        m_evCorrection = vm["Exposure.evCorrection"].as<int>();
        m_exposureWeighting = vm["Exposure.exposureWeighting"].as<int>();
        m_exposureSpeed = vm["Exposure.exposureSpeed"].as<int>();
        m_shuterSpeed = vm["Exposure.shutterSpeed"].as<unsigned int>();
        m_gain = vm["Exposure.gain"].as<unsigned int>();

        m_adaptiveGradation = (vm["Tools.adaptiveGradation"].as<int>() != 0);
        m_atrGain = vm["Tools.atrGain"].as<unsigned int>();
        m_atrWide = (vm["Tools.atrWide"].as<int>() != 0);
        m_atrContrast = vm["Tools.atrContrast"].as<unsigned int>();
        m_atrChroma = vm["Tools.atrChroma"].as<unsigned int>();
        m_imageStabilization = (vm["Tools.imageStabilization"].as<int>() != 0);
        m_faceDetection = (vm["Tools.faceDetection"].as<int>() != 0);
        m_numberOfFaces = vm["Tools.numberOfFaces"].as<unsigned int>();

        m_horizFlip = (vm["Image.horizFlip"].as<int>() != 0);
        m_vertFlip = (vm["Image.vertFlip"].as<int>() != 0);

        m_whiteBalance = vm["Colour.whiteBalance"].as<int>();
        m_ccOffset = (vm["Colour.ccOffset"].as<int>() != 0);
        m_ccR = vm["Colour.ccR"].as<int>();
        m_ccB = vm["Colour.ccB"].as<int>();
        m_ccSpeed = vm["Colour.ccSpeed"].as<int>();

        m_readRegister = vm["Sensor.ReadRegister"].as<unsigned int>();
        m_readBytes = vm["Sensor.ReadBytes"].as<unsigned int>();
        m_writeRegister = vm["Sensor.WriteRegister"].as<unsigned int>();
    }
    catch (std::exception& e)
    {
        ONEVIEW_LOG_ERROR("Failed to parse config file [%s] : %s", name, e.what());
        return false;
    }

    ONEVIEW_LOG_INFO("Loaded configuration file [%s]", name);

    if (m_bLoaded)
    {
        m_bDirty = true;
    }
    else
    {
        m_bLoaded = true;
    }

    return true;
}


void Settings::loadDefaults()
{
    m_posX = -1; // main window should handle this situation by itself
    m_posY = -1;
    m_posW = 0;  // main window should handle this situation by itself
    m_posH = 0;
    m_posMaximized = false;

#ifdef QT_NO_DEBUG
    m_logLevel = Vreo::Logger::Disable;
#else
    m_logLevel = Vreo::Logger::Debug;
#endif

    m_videoResX = 640;
    m_videoResY = 480;
    m_pixelFormat = Vreo::pfYUYV;

    m_videoPath = QDir::homePath().toLocal8Bit().constData();

    m_brightness = -1;
    m_contrast = -1;
    m_hue = -1;
    m_saturation = -1;
    m_sharpness = -1;
    m_blc = -1;
    m_powerLineFreq = -1;
    m_zoom = -1;

    m_exposureMode = 0;
    m_evCorrection = 0;
    m_exposureWeighting = 0;
    m_exposureSpeed = 0;
    m_adaptiveGradation = false;
    m_atrGain = 0;
    m_atrWide = false;
    m_atrContrast = 0;
    m_atrChroma = 0;
    m_horizFlip = false;
    m_vertFlip = false;
    m_imageStabilization = false;
    m_shuterSpeed = 31;
    m_gain = 800;
    m_faceDetection = false;
    m_numberOfFaces = 1;
    m_whiteBalance = 0;
    m_ccOffset = false;
    m_ccR = 0;
    m_ccB = 0;
    m_ccSpeed = 0;

    m_readRegister = 0;
    m_readBytes = 1;
    m_writeRegister = 0;

    if (m_bLoaded)
    {
        m_bDirty = true;
    }
}


bool Settings::save(const char* name)
{
    bool bDifferentFile = false;
    if (!name)
    {
        name = m_fileName.c_str();
    }
    else
    {
        bDifferentFile = true;
    }

    std::ofstream file;
    file.open(name, std::ofstream::trunc);

    if (!file.is_open())
    {
        ONEVIEW_LOG_ERROR("Failed to save configuration file [%s]", name);
        return false;
    }

    file << "[MainWindow]" << std::endl;
    file << "posX=" << m_posX << std::endl;
    file << "posY=" << m_posY << std::endl;
    file << "posW=" << m_posW << std::endl;
    file << "posH=" << m_posH << std::endl;
    file << "posMaximized=" << (m_posMaximized ? 1 : 0) << std::endl;

    file << "[Logging]" << std::endl;
    file << "level=" << m_logLevel << std::endl;

    file << "[Video]" << std::endl;
    file << "resolutionX=" << m_videoResX << std::endl;
    file << "resolutionY=" << m_videoResY << std::endl;
    file << "pixelFormat=" << uint32_t(m_pixelFormat) << std::endl;
    file << "path=" << m_videoPath << std::endl;

    file << "[Std]" << std::endl;
    file << "brightness=" << m_brightness << std::endl;
    file << "contrast=" << m_contrast << std::endl;
    file << "hue=" << m_hue << std::endl;
    file << "saturation=" << m_saturation << std::endl;
    file << "sharpness=" << m_sharpness << std::endl;
    file << "blc=" << m_blc << std::endl;
    file << "powerFreq=" << m_powerLineFreq << std::endl;
    file << "zoom=" << m_zoom << std::endl;

    file << "[Exposure]" << std::endl;
    file << "exposureMode=" << m_exposureMode << std::endl;
    file << "evCorrection=" << m_evCorrection << std::endl;
    file << "exposureWeighting=" << m_exposureWeighting << std::endl;
    file << "exposureSpeed=" << m_exposureSpeed << std::endl;
    file << "shutterSpeed=" << m_shuterSpeed << std::endl;
    file << "gain=" << m_gain << std::endl;

    file << "[Image]" << std::endl;
    file << "horizFlip=" << (m_horizFlip ? 1 : 0) << std::endl;
    file << "vertFlip=" << (m_vertFlip ? 1 : 0) << std::endl;

    file << "[Colour]" << std::endl;
    file << "whiteBalance=" << m_whiteBalance << std::endl;
    file << "ccOffset=" << (m_ccOffset ? 1 : 0) << std::endl;
    file << "ccR=" << m_ccR << std::endl;
    file << "ccB=" << m_ccB << std::endl;
    file << "ccSpeed=" << m_ccSpeed << std::endl;

    file << "[Tools]" << std::endl;
    file << "adaptiveGradation=" << (m_adaptiveGradation ? 1 : 0) << std::endl;
    file << "atrGain=" << m_atrGain << std::endl;
    file << "atrWide=" << (m_atrWide ? 1 : 0) << std::endl;
    file << "atrContrast=" << m_atrContrast << std::endl;
    file << "atrChroma=" << m_atrChroma << std::endl;
    file << "imageStabilization=" << (m_imageStabilization ? 1 : 0) << std::endl;
    file << "faceDetection=" << (m_faceDetection ? 1 : 0) << std::endl;
    file << "numberOfFaces=" << m_numberOfFaces << std::endl;

    file << "[Sensor]" << std::endl;
    file << "ReadRegister=" << m_readRegister << std::endl;
    file << "ReadBytes=" << m_readBytes << std::endl;
    file << "WriteRegister=" << m_writeRegister << std::endl;

    ONEVIEW_LOG_INFO("Saved configuration file [%s]", name);

    if (!bDifferentFile)
    {
        m_bDirty = false;
    }

    return true;
}
