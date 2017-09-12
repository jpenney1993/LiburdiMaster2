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

    // Firgelli slots for linear actuator
    void actuatorConnected();
    void actuatorFetchDT(int input);
    void actuatorFetchPositionRaw(int input);
    void actuatorFetchPositionMetric(float input);
    void actuatorFetchVelocityRaw(int input);
    void actuatorFetchVelocityMetric(float input);
    void actuatorFetchVelocityReal(float input);
    void actuatorFetchOscillate(bool state);

    // Motion button controls
    void on_travFwdButton_clicked();
    void on_travRevButton_clicked();
    void on_travGoButton_clicked();
    void on_travStopButton_clicked();
    void on_travZeroButton_clicked();
    void on_travCheckButton_clicked();
    void checkTravPos();
    void on_oscOutButton_clicked();
    void on_oscInButton_clicked();
    void on_oscGoButton_clicked();
    void on_oscStopButton_clicked();
    void on_oscZeroButton_clicked();
    void on_avcUpButton_clicked();
    void on_avcDownButton_clicked();
    void jogAVCUp();
    void jogAVCDown();

    // Welding button controls
    void on_purgeButton_clicked();
    void on_customMessageButton_clicked();
    void readData();
    void on_setCalibration1Button_clicked();
    void on_setCalibration2Button_clicked();
    void on_oscCheckButton_clicked();
    void on_weldButton_clicked();
    void TravelADistance(double dist);
    void oscillateADistance(double dist);
    void weldADistance(double dist);
    void turnOnWeaving();

    // Firgelli button controls
    void on_connectLinActButton_clicked();
    void on_wireJogUpButton_clicked();
    void on_wireJogDownButton_clicked();
    void on_wireStepVel_editingFinished();
    void on_wireRunUpButton_pressed();
    void on_wireRunDownButton_pressed();

    void on_wireRunUpButton_released();

    void on_wireRunDownButton_released();

    void on_multiPassButton_clicked();

    void on_proceedButton_clicked();

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
    void initializeActuatorThread();

signals:
    void actuatorConnect();
    void actuatorPushPosition(int input);// not used rn 8/31/2017 -matt
    void actuatorPushVelocity(float input);
    void actuatorPushOscillate(int min, int max);// not used, nor useful 8/31/2017 -matt
    void actuatorMoveUp(float mmToMove);
    void actuatorMoveDown(float mmToMove);
    void actuatorRunUp();
    void actuatorRunDown();
    void actuatorStopRunUp();
    void actuatorStopRunDown();
};

#endif // MAINWINDOW_H
