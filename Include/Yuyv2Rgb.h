#pragma once


#include "VideoSource.h"

#include <atomic>


namespace Vreo
{


class Yuyv2Rgb : public QObject, public Vreo::IVideoSource
{
    Q_OBJECT
    Q_INTERFACES(Vreo::IVideoSource Vreo::IErrorSource)

public:
    explicit Yuyv2Rgb(Vreo::IVideoSource* source);

    void                                 addRef() throw() override;
    void                                 release() throw() override;

    bool                                 startCapture() override;
    void                                 stopCapture() override;
    bool                                 readFrame(Vreo::VideoFrame& vf) override;

    virtual Vreo::Error                  lastError() const override;

protected:
    ~Yuyv2Rgb();

private:
    std::atomic<long>                                m_refs;
    Vreo::ReferenceCountedPtr<Vreo::IVideoSource>    m_source;

};


typedef Vreo::ReferenceCountedPtr<Vreo::Yuyv2Rgb> Yuyv2RgbPtr;


inline Vreo::Error Yuyv2Rgb::lastError() const
{
    return m_source->lastError();
}


}
