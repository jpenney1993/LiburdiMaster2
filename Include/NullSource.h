#pragma once


#include "ErrorSourceBase.h"
#include "VideoSource.h"

#include <atomic>


namespace Vreo
{



class NullSource : public QObject, public Vreo::ErrorSourceBase, public Vreo::IVideoSource
{
    Q_OBJECT
    Q_INTERFACES(Vreo::IVideoSource Vreo::IErrorSource)

public:
    explicit NullSource(const Vreo::VideoFormat& vf);

    void                                 addRef() throw() override;
    void                                 release() throw() override;

    bool                                 startCapture() override;
    void                                 stopCapture() override;
    bool                                 readFrame(Vreo::VideoFrame& frame) override;

    void                                 setFormat(const Vreo::VideoFormat& vf);

protected:
    ~NullSource();

private:
    std::atomic<long>                    m_refs;
    Vreo::VideoFormat                    m_vf;
    uint64_t                             m_timestamp;
};


typedef ReferenceCountedPtr<Vreo::NullSource> NullSourcePtr;


inline void NullSource::setFormat(const Vreo::VideoFormat& vf)
{
    Q_ASSERT(vf.pf != 0);
    Q_ASSERT(vf.width > 0);
    Q_ASSERT(vf.height > 0);

    m_vf = vf;
}


}
