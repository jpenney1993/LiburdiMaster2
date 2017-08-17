#pragma once


#include "VideoSink.h"
#include "VideoSource.h"

#include <QThread>


namespace Vreo
{


class CaptureThread : public QThread
{
    Q_OBJECT

public:
    ~CaptureThread();
    CaptureThread(Vreo::IVideoSource* pSource, Vreo::IVideoSink* pSink);

    double                                       realFps() const;

    void                                         start();
    void                                         stop();

private:
    void                                         run() override;

signals:
    void                                         errorSignal(Vreo::Error e);

private:
    bool                                         m_stopFlag;
    Vreo::VideoSourcePtr                         m_ptrSource;
    Vreo::VideoSinkPtr                           m_ptrSink;

    double                                       m_realFps;
    uint32_t                                     m_fpsFrames;
    uint64_t                                     m_fpsRefTs;

private:
    CaptureThread(const CaptureThread&);
    CaptureThread& operator=(const CaptureThread&);
};


inline double CaptureThread::realFps() const
{
    return m_realFps;
}


}
