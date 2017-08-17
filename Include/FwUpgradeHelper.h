#pragma once

#include "Xu.h"

#include <QString>
#include <QThread>


namespace Vreo
{


class FwUpgradeHelper : public QThread
{
    Q_OBJECT

public:
    ~FwUpgradeHelper();
    FwUpgradeHelper(const char* fileName, Vreo::IXuControls* pCtrl);

    void                                         start();
    void                                         stop();

    bool                                         succeeded() const;

signals:
    void                                         errorSignal(Vreo::Error e);
    void                                         progressSignal(const QString& status, int progress);

private:
    void                                         run() override;

    bool                                         _waitForDeviceReady();
    bool                                         _eraseSector(unsigned int sector);
    bool                                         _eraseAllSectors();
    bool                                         _writePages(const void* data, size_t size);
    bool                                         _verifyPages(const void* data, size_t size);

private:
    QString                                      m_fileName;
    Vreo::XuControlsPtr                          m_ptrXu;
    bool                                         m_bStop;
    bool                                         m_bSuccess;
};


inline bool FwUpgradeHelper::succeeded() const
{
    return m_bSuccess;
}


}
