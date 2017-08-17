#pragma once

#include "ErrorSourceBase.h"
#include "VideoSink.h"

#include <atomic>
#include <mutex>

#include <QGLWidget>

namespace Vreo
{


class QtGlVideoSink : public QGLWidget, public Vreo::ErrorSourceBase, public Vreo::IVideoSink
{
    Q_OBJECT
    Q_INTERFACES(Vreo::IVideoSink Vreo::IErrorSource)

public:
    ~QtGlVideoSink();
    explicit QtGlVideoSink(QWidget* parent = nullptr);

    void                                 addRef() throw() override;
    void                                 release() throw() override;

    bool                                 writeFrame(const Vreo::VideoFrame& frame) override;

private:
    void                                 initializeGL() override;
    void                                 paintGL() override;
    void                                 resizeGL(int width, int height) override;

private:
    std::atomic<long>                    m_refs;
    int                                  m_errorCode;
    std::mutex                           m_lock;
    Vreo::VideoFrame                     m_currentFrame;
    GLuint                               m_texture;
};


}
