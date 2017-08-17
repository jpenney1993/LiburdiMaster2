#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "VideoDevice.h"
#include "Settings.h"

#include <QtCore/QtGlobal>
#include <QtSerialPort/QSerialPort>
#include <QMainWindow>
#include <QModbusDataUnit>

QT_BEGIN_NAMESPACE
class QModbusClient;
class QModbusReply;

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ezservoConnectionDialog;
class liburdiConnectionDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initActions();
    QModbusDataUnit readRequest() const;

    // Vreo
    std::vector<Vreo::DeviceDesc> m_devices;
    int m_deviceSelected;
    Vreo::VideoDevicePtr m_videoDevice;
    Vreo::VideoFormat m_format;
    Settings m_settings;

private slots:
    void on_connectLiburdiButton_clicked();
    void onStateChanged(int state);
    void on_connectEzservoButton_clicked();
    void openEzservoPort();
    void closeEzservoPort();

    // Motion button controls
    void on_travFwdButton_clicked();
    void on_travRevButton_clicked();
    void on_travGoButton_clicked();
    void on_travStopButton_clicked();
    void on_travZeroButton_clicked();
    void on_travCheckButton_clicked();
    void on_oscOutButton_clicked();
    void on_oscInButton_clicked();
    void on_oscGoButton_clicked();
    void on_oscStopButton_clicked();
    void on_oscZeroButton_clicked();
    void on_avcUpButton_clicked();
    void on_avcDownButton_clicked();

    // Welding button controls
    void on_purgeButton_clicked();

    void on_customMessageButton_clicked();

    void readData();

    void on_setCalibration1Button_clicked();
    void on_setCalibration2Button_clicked();

    void on_oscCheckButton_clicked();

private:
    Ui::MainWindow *ui;
    QModbusReply *lastRequest;
    liburdiConnectionDialog*liburdiSettings;
    QModbusClient *modbusDevice;
    ezservoConnectionDialog *ezservoSettings;
    QSerialPort *ezservo;

    void delay(int time);
    int travDist2Click(double dist);
    int oscDist2Click(double dist);
    void ezservoInit();
};

#endif // MAINWINDOW_H
