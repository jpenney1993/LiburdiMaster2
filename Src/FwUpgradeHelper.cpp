#include "FwUpgradeHelper.h"
#include "Logger.h"

#include <QFile>


//#define FAKE_UPGRADE


namespace Vreo
{


FwUpgradeHelper::~FwUpgradeHelper()
{
    stop();
}


FwUpgradeHelper::FwUpgradeHelper(const char* fileName, IXuControls* pCtrl)
: m_fileName(fileName)
, m_ptrXu(pCtrl, true)
, m_bStop(false)
, m_bSuccess(false)
{

}


void FwUpgradeHelper::run()
{
    m_bSuccess = false;

    ONEVIEW_LOG_INFO("Starting firmware upgrade from %s", m_fileName.toUtf8().constData());

    emit progressSignal(tr("Loading image file..."), 0);

    // open and map the image file
    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        ONEVIEW_LOG_ERROR("Failed to open the image file");
        emit errorSignal(Error(file.error()));
        return;
    }

    if (file.size() < PageSize)
    {
        ONEVIEW_LOG_ERROR("Image file is too small");
        emit errorSignal(Error(tr("Image file is invalid")));
        return;
    }

    void* pMapping = file.map(0, file.size());
    if (!pMapping)
    {
        ONEVIEW_LOG_ERROR("Failed to map the image file");
        emit errorSignal(Error(file.error()));
        return;
    }

#ifndef FAKE_UPGRADE
    const char* pSig = (const char*)pMapping;
    if (pSig[0] != 'C' || pSig[1] != 'Y')
    {
        ONEVIEW_LOG_ERROR("Invalid image file signature");
        emit errorSignal(Error(tr("Image file is invalid")));
        return;
    }
#endif

    if (!_eraseAllSectors())
    {
        return;
    }

    if (!_writePages(pMapping, file.size()))
    {
        return;
    }

    if (!_verifyPages(pMapping, file.size()))
    {
        return;
    }

    ONEVIEW_LOG_INFO("Firmware upgrade complete");

    m_bSuccess = true;
}


void FwUpgradeHelper::start()
{
    m_bStop = false;

    QThread::start(QThread::HighPriority);
}


void FwUpgradeHelper::stop()
{
    m_bStop = true;

    if (QThread::isRunning())
    {
        QThread::wait();
    }
}


bool FwUpgradeHelper::_waitForDeviceReady()
{
#ifdef FAKE_UPGRADE

    return true;

#else

    SpiStatus status = SpiBusy;
    int retries = 10;
    do {
        if (m_bStop)
        {
            ONEVIEW_LOG_INFO("Operation canceled");
            return false;
        }

        if (!m_ptrXu->getSpiStatus(status))
        {
            emit errorSignal(m_ptrXu->lastError());
            ONEVIEW_LOG_ERROR("Failed to wait for device");
            return false;
        }

        if (status == SpiReady)
        {
            return true;
        }

        retries--;
        QThread::msleep(10);
    } while (retries > 0);

    ONEVIEW_LOG_ERROR("Device is busy");
    emit errorSignal(Error(tr("Device is busy")));

    return false;

#endif
}


bool FwUpgradeHelper::_eraseSector(unsigned int sector)
{
    ONEVIEW_LOG_INFO("Erasing sector %d", sector);

    if (!_waitForDeviceReady())
    {
        return false;
    }

    if (m_bStop)
    {
        ONEVIEW_LOG_INFO("Operation canceled");
        return false;
    }

#ifndef FAKE_UPGRADE
    if (!m_ptrXu->eraseSector(sector))
    {
        ONEVIEW_LOG_ERROR("Failed to erase sector");
        emit errorSignal(Error(tr("Failed to erase sector")));
        return false;
    }
#else

    QThread::msleep(1000);
#endif

    return true;
}


bool FwUpgradeHelper::_eraseAllSectors()
{
    ONEVIEW_LOG_INFO("Erasing sectors...");

    for (unsigned int sector = 0; sector < SectorCount; sector++)
    {
        emit progressSignal(QString(tr("Erasing sector %1")).arg(sector), int(100 * double(sector) / double(SectorCount)));

        if (m_bStop)
        {
            ONEVIEW_LOG_INFO("Operation canceled");
            return false;
        }

        if (!_eraseSector(sector))
        {
            return false;
        }
    }

    return true;
}


bool FwUpgradeHelper::_writePages(const void* data, size_t size)
{
    ONEVIEW_LOG_INFO("Writing %d bytes of image", size);

    unsigned int nPages = (unsigned int)(size / PageSize);
    unsigned int iPage = 0;
    const uint8_t* pCurrent = (const uint8_t*)data;
    while (iPage < nPages)
    {
        if (m_bStop) return false;

        ONEVIEW_LOG_INFO("Writing page %d...", iPage);

        emit progressSignal(QString(tr("Writing page %1 of %2")).arg(iPage + 1).arg(nPages), int(100 * double(iPage) / double(nPages)));

        Page page;
        ::memcpy(page.data, pCurrent, PageSize);

        if (!_waitForDeviceReady())
        {
            return false;
        }

#ifndef FAKE_UPGRADE
        if (!m_ptrXu->setCurrentPage(iPage))
        {
            ONEVIEW_LOG_ERROR("Failed to set current page %d", iPage);
            emit errorSignal(m_ptrXu->lastError());
            return false;
        }

        if (!m_ptrXu->writePage(page))
        {
            ONEVIEW_LOG_ERROR("Failed to write page %d", iPage);
            emit errorSignal(m_ptrXu->lastError());
            return false;
        }
#else
        QThread::msleep(50);
#endif

        pCurrent += PageSize;
        iPage++;
    }

    // do we need to write the last page?
    unsigned int cbRemainder = size % PageSize;
    if (cbRemainder > 0)
    {
        if (m_bStop) return false;

        ONEVIEW_LOG_INFO("Writing page %d...", iPage);

        emit progressSignal(QString(tr("Writing page %1 of %2")).arg(iPage + 1).arg(nPages), int(100 * double(iPage) / double(nPages)));

        if (!_waitForDeviceReady())
        {
            return false;
        }

        Page page;
        ::memset(page.data, 0, sizeof(page.data));
        ::memcpy(page.data, pCurrent, cbRemainder);

#ifndef FAKE_UPGRADE

        if (!m_ptrXu->setCurrentPage(iPage))
        {
            ONEVIEW_LOG_ERROR("Failed to set current page %d", iPage);
            emit errorSignal(m_ptrXu->lastError());
            return false;
        }

        if (!m_ptrXu->writePage(page))
        {
            ONEVIEW_LOG_ERROR("Failed to write page %d", iPage);
            emit errorSignal(m_ptrXu->lastError());
            return false;
        }
#else
        QThread::msleep(50);
#endif
    }

    return true;
}


bool FwUpgradeHelper::_verifyPages(const void* data, size_t size)
{
    ONEVIEW_LOG_INFO("Verifying pages...");

    unsigned int nPages = (unsigned int)(size / PageSize);
    unsigned int iPage = 0;
    const uint8_t* pCurrent = (const uint8_t*)data;
    while (iPage < nPages)
    {
        if (m_bStop) return false;

        ONEVIEW_LOG_INFO("Verifying page %d...", iPage);

        emit progressSignal(QString(tr("Verifying page %1 of %2")).arg(iPage + 1).arg(nPages), int(100 * double(iPage) / double(nPages)));

        if (!_waitForDeviceReady())
        {
            return false;
        }

#ifndef FAKE_UPGRADE
        if (!m_ptrXu->setCurrentPage(iPage))
        {
            ONEVIEW_LOG_ERROR("Failed to set current page %d", iPage);
            emit errorSignal(m_ptrXu->lastError());
            return false;
        }

        Page page;
        if (!m_ptrXu->readPage(page))
        {
            ONEVIEW_LOG_ERROR("Failed to read page %d", iPage);
            emit errorSignal(m_ptrXu->lastError());
            return false;
        }

        if (::memcmp(page.data, pCurrent, PageSize) != 0)
        {
            ONEVIEW_LOG_ERROR("Page %d is not written properly", iPage);
            emit errorSignal(Error(QString(tr("Page %1 is not written properly")).arg(iPage)));
            return false;
        }
#else
        QThread::msleep(50);
#endif

        pCurrent += PageSize;
        iPage++;
    }

    // do we need to verify the last page?
    unsigned int cbRemainder = size % PageSize;
    if (cbRemainder > 0)
    {
        if (m_bStop) return false;

        ONEVIEW_LOG_INFO("Verifying page %d...", iPage);

        emit progressSignal(QString(tr("Verifying page %1 of %2")).arg(iPage + 1).arg(nPages), int(100 * double(iPage) / double(nPages)));

        if (!_waitForDeviceReady())
        {
            return false;
        }

#ifndef FAKE_UPGRADE
        if (!m_ptrXu->setCurrentPage(iPage))
        {
            ONEVIEW_LOG_ERROR("Failed to set current page %d", iPage);
            emit errorSignal(m_ptrXu->lastError());
            return false;
        }

        Page page;
        if (!m_ptrXu->readPage(page))
        {
            ONEVIEW_LOG_ERROR("Failed to read page %d", iPage);
            emit errorSignal(m_ptrXu->lastError());
            return false;
        }

        if (::memcmp(page.data, pCurrent, cbRemainder) != 0)
        {
            ONEVIEW_LOG_ERROR("Page %d is not written properly", iPage);
            emit errorSignal(Error(QString(tr("Page %1 is not written properly")).arg(iPage)));
            return false;
        }
#else
        QThread::msleep(50);
#endif
    }

    return true;
}


}
