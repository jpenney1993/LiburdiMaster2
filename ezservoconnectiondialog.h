#ifndef EZSERVOCONNECTIONDIALOG_H
#define EZSERVOCONNECTIONDIALOG_H

#include <QDialog>
#include <QSerialPort>

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace Ui {
class ezservoConnectionDialog;
}

class QIntValidator;

QT_END_NAMESPACE

class ezservoConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ezservoConnectionDialog(QWidget *parent = 0);
    ~ezservoConnectionDialog();
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
        bool localEchoEnabled;
    };
    Settings settings() const;

private slots:
    void showPortInfo(int idx);
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

private:
    Ui::ezservoConnectionDialog *ui;
    Settings currentSettings;
    QIntValidator *intValidator;

    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();
};

#endif // EZSERVOCONNECTIONDIALOG_H
