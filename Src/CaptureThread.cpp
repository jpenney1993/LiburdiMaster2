#include "CaptureThread.h"
#include "Logger.h"


namespace Vreo
{



CaptureThread::~CaptureThread()
{
    stop();
}


CaptureThread::CaptureThread(IVideoSource* pSource, IVideoSink* pSink)
: m_stopFlag(false)
, m_ptrSource(pSource, true)
, m_ptrSink(pSink, true)
, m_realFps(0)
, m_fpsFrames(0)
, m_fpsRefTs(0)
{

}


void CaptureThread::run()
{
    ONEVIEW_LOG_DEBUG("Capture thread started");

    if (!m_ptrSource->startCapture())
    {
        emit errorSignal(m_ptrSource->lastError());
        return;
    }

    while (!m_stopFlag)
    {
        VideoFrame vf;
        if (!m_ptrSource->readFrame(vf))
        {
            if (m_ptrSource->lastError().isError())
            {
                emit errorSignal(m_ptrSource->lastError());
                return;
            }
            else
            {
                continue;
            }
        }

        m_ptrSink->writeFrame(vf);

        // determine real fps every second
        {
            m_fpsFrames++;
            if(vf.timestamp() - m_fpsRefTs >= 1000000000LL)
            {
                m_realFps = double(m_fpsFrames * 1000000000LL) / double(vf.timestamp() - m_fpsRefTs);
                m_fpsFrames = 0;
                m_fpsRefTs = vf.timestamp();
            }
        }
    }

    m_ptrSource->stopCapture();

    ONEVIEW_LOG_DEBUG("Capture thread stopped");
}


void CaptureThread::start()
{
    m_stopFlag = false;

    QThread::start(QThread::HighPriority);
}


void CaptureThread::stop()
{
    m_stopFlag = true;

    if (QThread::isRunning())
    {
        QThread::wait();
    }
}


}
