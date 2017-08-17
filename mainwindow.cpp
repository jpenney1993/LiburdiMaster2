#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "liburdiconnectiondialog.h"
#include "ezservoconnectiondialog.h"
#include "time.h"
#include "math.h"

#include <QModbusTcpClient>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QtSerialPort/QSerialPort>
#include <QUrl>
#include <QMessageBox>
#include <QTime>

// still testing this stupid thing...
// hello there

int travScaleFactor;
int oscScaleFactor;
int lastEZServo;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    lastRequest(nullptr),
    modbusDevice(nullptr)
{
    ui->setupUi(this);

    ezservoSettings = new ezservoConnectionDialog;
    ezservo = new QSerialPort(this);

    liburdiSettings = new liburdiConnectionDialog;
    initActions();

    // TEMPORARY DISABLING BUTTONS/BOXES
    ui->avcCurrentPosEdit->setDisabled(true);
    ui->avcCurPosLabel->setDisabled(true);
    ui->avcDistanceEdit->setDisabled(true);
    ui->avcDistLabel->setDisabled(true);
    ui->avcGoButton->setDisabled(true);
    ui->avcStopButton->setDisabled(true);
    ui->avcSSLabel->setDisabled(true);

    connect(ezservo,&QSerialPort::readyRead,this,&MainWindow::readData);
}

//******* DECONSTRUCTOR FUNCTION **********************************************
/* Destroys the window when the program exits */
MainWindow::~MainWindow(){
    delete ui;
}

//******** INITIAL SETUP FUNCTION ****************************************************
/* Performs the necessary initial setup functions for correct operation */
void MainWindow::initActions()
{
    // LIBURDI CONNECTION SETUP ****************************************
    // This some of this needs to be moved to the liburdiConnectionDialog.cpp file
    if(modbusDevice) {
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice = nullptr;
    }

    modbusDevice = new QModbusTcpClient(this);
    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusClient::Error){
        statusBar()->showMessage(modbusDevice->errorString(),5000);
    });

    if(!modbusDevice){
        ui->connectLiburdiButton->setDisabled(true);
        statusBar()->showMessage(tr("Could not create Modbus Client"),5000);
    } else{
        connect(modbusDevice,&QModbusClient::stateChanged,this,&MainWindow::onStateChanged);
    }

    // CONNECTING MENU ACTIONS ********************************************
    connect(ui->actionEZServo_Connection,&QAction::triggered,ezservoSettings,&ezservoConnectionDialog::show);
    connect(ui->actionLiburdi_Connection,&QAction::triggered,liburdiSettings,&liburdiConnectionDialog::show);

    // DISABLE BUTTONS UNTIL EZSERVO IS CONNECTED **************************
    ui->travCheckButton->setDisabled(true);
    ui->travCurrentPosEdit->setDisabled(true);
    ui->travDistanceEdit->setDisabled(true);
    ui->travFwdButton->setDisabled(true);
    ui->travRevButton->setDisabled(true);
    ui->travGoButton->setDisabled(true);
    ui->travStopButton->setDisabled(true);
    ui->travZeroButton->setDisabled(true);
    ui->oscCurrentPosEdit->setDisabled(true);
    ui->oscDistanceEdit->setDisabled(true);
    ui->oscGoButton->setDisabled(true);
    ui->oscInButton->setDisabled(true);
    ui->oscOutButton->setDisabled(true);
    ui->oscStopButton->setDisabled(true);
    ui->oscCheckButton->setDisabled(true);
    ui->oscZeroButton->setDisabled(true);

    travScaleFactor = 1350626; // clicks/in
    oscScaleFactor = 1798144; // clicks/in
}

//$$$$$$$$$$$$$$$$$$$$$$$ CONNECTION CONTROLS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//********* LIBURDI CONNECTION BUTTION FUNCTIONS *******************************************
/* These functions control the connecting and disconnecting of the
 * TCP/IP Modbus device */
void MainWindow::on_connectLiburdiButton_clicked(){
    statusBar()->clearMessage();
    ui->replyBox->clear();
    liburdiConnectionDialog::Settings p = liburdiSettings->settings();
    if(modbusDevice->state() != QModbusDevice::ConnectedState){
        //const QUrl url = QUrl::fromUserInput(ui->IPedit->text());
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter,p.liburdiURL.port());
        modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter,p.liburdiURL.host());
        ui->replyBox->addItem("Hello, you are connected");
        if(!modbusDevice->connectDevice())
            statusBar()->showMessage(tr("Connect Failed: ") + modbusDevice->errorString(), 5000);
    } else {
        modbusDevice->disconnectDevice();
        statusBar()->showMessage("Disconnect successful");
        ui->replyBox->addItem("Thanks for disconnecting");
        ui->connectLiburdiButton->setText("Connect");
    }
}
/* Determines the current connection state */
void MainWindow::onStateChanged(int state){
    //bool connected = (state != QModbusDevice::UnconnectedState);

    if(state == QModbusDevice::UnconnectedState){
        ui->connectLiburdiButton->setText("Connect");
        ui->liburdiActiveCheckBox->setChecked(false);
        statusBar()->showMessage(tr("Liburdi is disconnected"));
    }
    else if(state == QModbusDevice::ConnectedState){
        ui->connectLiburdiButton->setText("Disconnect");
        ui->liburdiActiveCheckBox->setChecked(true);
        statusBar()->showMessage(tr("Liburdi is connected"));
    }
}

//********* EZSERVO CONNECTION BUTTON FUNCTIONS*****************************************
/* These functions control the connecting and disconnecting of the
 * serial communications between the computer and the EZSERVo boards */
void MainWindow::on_connectEzservoButton_clicked()
{
    if(ui->connectEzservoButton->text() == "Connect")
        openEzservoPort();
    else if(ui->connectEzservoButton->text() == "Disconnect")
        closeEzservoPort();
}
/* Opens the serial port to the EZServo Boards */
void MainWindow::openEzservoPort()
{
    ezservoConnectionDialog::Settings p = ezservoSettings->settings();
    ezservo->setPortName(p.name);
    ezservo->setBaudRate(p.baudRate);
    ezservo->setDataBits(p.dataBits);
    ezservo->setParity(p.parity);
    ezservo->setStopBits(p.stopBits);
    ezservo->setFlowControl(p.flowControl);
    if(ezservo->open(QIODevice::ReadWrite)){
        ui->replyBox->clear();
        ui->replyBox->addItem(tr("EZServos Connected"));
        ui->actionEZServo_Connection->setEnabled(false);
        ui->ezservoActiveCheckBox->setChecked(true);
        ui->connectEzservoButton->setText("Disconnect");
        ui->ezservoActiveCheckBox->setChecked(true);
        delay(2);
        ezservoInit();
        ui->travCheckButton->setDisabled(false);
        ui->travCurrentPosEdit->setDisabled(false);
        ui->travDistanceEdit->setDisabled(false);
        ui->travFwdButton->setDisabled(false);
        ui->travRevButton->setDisabled(false);
        ui->travGoButton->setDisabled(false);
        ui->travStopButton->setDisabled(false);
        ui->travZeroButton->setDisabled(false);
        ui->oscCurrentPosEdit->setDisabled(false);
        ui->oscDistanceEdit->setDisabled(false);
        ui->oscGoButton->setDisabled(false);
        ui->oscInButton->setDisabled(false);
        ui->oscOutButton->setDisabled(false);
        ui->oscStopButton->setDisabled(false);
        ui->oscCheckButton->setDisabled(false);
        ui->oscZeroButton->setDisabled(false);
    } else{
        QMessageBox::critical(this,tr("Error"),ezservo->errorString());
        statusBar()->showMessage(tr("EZServo Open Error"));
    }
}
/* Closes the serial port to the EZServo Boards */
void MainWindow::closeEzservoPort()
{
    if(ezservo->isOpen()){
        ui->replyBox->clear();
        ezservo->close();
        ui->replyBox->addItem(tr("EZServos Disconnected"));
    }
    ui->actionEZServo_Connection->setEnabled(true);
    ui->ezservoActiveCheckBox->setChecked(false);
    ui->connectEzservoButton->setText("Connect");
    ui->ezservoActiveCheckBox->setChecked(false);
}

//$$$$$$$$$$$$$$$$$$$$$$$ MOTION CONTROLS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//********* TRAVEL FUNCTIONS ************************************************
/* This function controls the jogging of forward travel of
 * the main base of the Liburdi */
void MainWindow::on_travFwdButton_clicked()
{
    ui->replyBox->clear();
    if(ezservo->isOpen()){
        QByteArray jogTravelFwd = "/1PR\r"; // Jog button moves by 1/16 in
        double dist = (double)1/16;
        int clicks = travDist2Click(dist);
        QString clickString = QString::number(clicks);
        jogTravelFwd.insert(3,clickString);
        ezservo->write(jogTravelFwd);
        lastEZServo = 1;
        ui->travCheckButton->click();
    } else{
        ui->replyBox->addItem(tr("EZServo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}
/* This function controls the jogging of reverse travel of
 * the main base of the Liburdi */
void MainWindow::on_travRevButton_clicked()
{
    ui->replyBox->clear();
    if(ezservo->isOpen()){
        QByteArray jogTravelBkwd = "/1DR\r"; // Jog button moves by 1/16 in
        double dist = (double)1/16;
        int clicks = travDist2Click(dist);
        QString clickString = QString::number(clicks);
        jogTravelBkwd.insert(3,clickString);
        ezservo->write(jogTravelBkwd);
        lastEZServo = 1;
        ui->travCheckButton->click();
    } else{
        ui->replyBox->addItem(tr("EZServo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}
/* This function commands the liburdi base to travel along the track
 * to a desired position specifiec by the text box */
void MainWindow::on_travGoButton_clicked()
{
    ui->replyBox->clear();
    QString clickString;
    QByteArray command;
    int clicks;
    if(ezservo->isOpen()){
        double dist = ui->travDistanceEdit->text().toDouble();
        if(dist > 0){
            command = "/1PR\r";
            clicks = travDist2Click(dist);
            clickString = QString::number(clicks);
        } else{
            command = "/1DR\r";
            clicks = travDist2Click(abs(dist));
            clickString = QString::number(clicks);
        }
        command.insert(3,clickString);
        ezservo->write(command);
        lastEZServo = 1;
        ui->replyBox->addItem(tr("Distance: %1").arg(QString::number(dist)));
        ui->replyBox->addItem(tr("Clicks: %1").arg(QString::number(clicks)));
        ui->replyBox->addItem(tr("Command:"));
        ui->replyBox->addItem(command);
        ui->travCheckButton->click();
    } else{
        ui->replyBox->addItem(tr("EZServo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}
/* This function stops the travel motion of the Liburdi when the Stop button
 * is clicked */
void MainWindow::on_travStopButton_clicked()
{
    ui->replyBox->clear();
    if(ezservo->isOpen()){
        QByteArray command = "/1T\r";
        ezservo->write(command);
        lastEZServo = 1;
        ui->replyBox->addItem(tr("Travel Motion Stopped!"));
    } else{
        ui->replyBox->addItem(tr("EZServo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}
/* This function checks the current position of the travel motor and
 * displays the result in inches */
void MainWindow::on_travCheckButton_clicked()
{
    ui->replyBox->clear();
    if(ezservo->isOpen()){
        QByteArray command = "/1?8\r";
        ezservo->write(command);
        lastEZServo = 1;
    } else{
        ui->replyBox->addItem(tr("EZServo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}
/* This function sets the current position of the travel motor to zero */
void MainWindow::on_travZeroButton_clicked()
{
    ui->replyBox->clear();
    if(ezservo->isOpen()){
        QByteArray command = "/1z0R\r";
        ezservo->write(command);
        lastEZServo = 1;
        delay(1);
        ui->travCheckButton->click();
        ui->replyBox->addItem(tr("Travel motor zeroed"));
    }
}


//********* OSCILATION FUNCTIONS ************************************************
/* This function controls the jogging of oscilation away from the track for
 * the torch */
void MainWindow::on_oscOutButton_clicked()
{
    ui->replyBox->clear();
    if(ezservo->isOpen()){
        QByteArray jogOscOut = "/2PR\r"; // Jog button moves by 1/16 in
        double dist = (double) 1/16;
        int clicks = oscDist2Click(dist);
        QString clickString = QString::number(clicks);
        jogOscOut.insert(3,clickString);
        ezservo->write(jogOscOut);
        lastEZServo = 2;
        ui->oscCheckButton->click();
    } else{
        ui->replyBox->addItem(tr("EZServo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}
/* This function controls the jogging of oscilation towards from the track for
 * the torch */
void MainWindow::on_oscInButton_clicked()
{
    ui->replyBox->clear();
    if(ezservo->isOpen()){
        QByteArray jogOscIn = "/2DR\r"; // Jog button moves by 100,000 clicks
        double dist = (double) 1/16;
        int clicks = oscDist2Click(dist);
        QString clickString = QString::number(clicks);
        jogOscIn.insert(3,clickString);
        ezservo->write(jogOscIn);
        lastEZServo = 2;
        ui->oscCheckButton->click();
    } else{
        ui->replyBox->addItem(tr("EZServo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}
/* This function commands the liburdi to move the welding torch to a desired
 * position in the cross seam using the oscialltion motor */
void MainWindow::on_oscGoButton_clicked()
{
    ui->replyBox->clear();
    QString clickString;
    QByteArray command;
    int clicks;
    double dist = ui->oscDistanceEdit->text().toDouble();
    double total = ui->oscCurrentPosEdit->text().toDouble()+dist;
    if(ezservo->isOpen()){
        if(total < 2){
            if(dist > 0){
                command = "/2PR\r";
                clicks = oscDist2Click(dist);
                clickString = QString::number(clicks);
            } else{
                command = "/2DR\r";
                clicks = oscDist2Click(abs(dist));
                clickString = QString::number(clicks);
            }
            command.insert(3,clickString);
            ezservo->write(command);
            lastEZServo = 2;
            ui->replyBox->addItem(tr("Distance: %1").arg(QString::number(dist)));
            ui->replyBox->addItem(tr("Clicks: %1").arg(QString::number(clicks)));
            ui->replyBox->addItem(tr("Command:"));
            ui->replyBox->addItem(command);
            ui->oscCheckButton->click();
        } else{
            ui->replyBox->addItem(tr("OSC LIMIT ERROR"));
            ui->replyBox->addItem(tr("Desired distance exceeds max total distace"));
        }
    } else{
        ui->replyBox->addItem(tr("EZServo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}
/* This function stops the oscialltion motion of the Liburdi when the Stop button
 * is clicked */
void MainWindow::on_oscStopButton_clicked()
{
    ui->replyBox->clear();
    if(ezservo->isOpen()){
        QByteArray command = "/2T\r";
        ezservo->write(command);
        ui->replyBox->addItem(tr("Oscillation Motion Stopped!"));
    } else{
        ui->replyBox->addItem(tr("EZServo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}
/* This function checks the current position of the oscillation motor and
 * displays the result in inches */
void MainWindow::on_oscCheckButton_clicked()
{
    ui->replyBox->clear();
    if(ezservo->isOpen()){
        QByteArray command = "/2?8\r";
        ezservo->write(command);
        lastEZServo = 2;
    } else{
        ui->replyBox->addItem(tr("EZServo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}
/* This function sets the current posisiton of the travel motor to zero */
void MainWindow::on_oscZeroButton_clicked()
{
    ui->replyBox->clear();
    if(ezservo->isOpen()){
        QByteArray command = "/2z0R\r";
        ezservo->write(command);
        delay(1);
        ui->oscCheckButton->click();
        ui->replyBox->addItem(tr("Oscillation motor zeroed"));
    }
}

//********** AVC FUNCTIONS *******************************************************
/* This function controls the jogging of movement up from the work piece using the
 * AVC motor */
void MainWindow::on_avcUpButton_clicked()
{
    if(!modbusDevice)
        return;
    ui->replyBox->clear();
    statusBar()->clearMessage();

    // Coil 271 controls AVC Up
    const auto table = static_cast<QModbusDataUnit::RegisterType>(2);
    QModbusDataUnit request = QModbusDataUnit(table,271,1);
    request.setValue(0,1);
    if(auto *reply = modbusDevice->sendWriteRequest(request,0)){
        if(!reply->isFinished()){
            connect(reply,&QModbusReply::finished,this,[this,reply](){
                if(reply->error()==QModbusDevice::ProtocolError){
                    statusBar()->showMessage(tr("Write response error: %1 (Modbus exception: 0x%2)").
                                             arg(reply->errorString()).
                                             arg(reply->error(),-1,16),5000);
                } else if(reply->error() != QModbusDevice::NoError){
                    statusBar()->showMessage(tr("Write response error: %1 (Code: 0x%2)").
                                             arg(reply->errorString()).
                                             arg(reply->error(),-1,16),5000);
                }
                reply->deleteLater();
            });
        } else{
            reply->deleteLater();
        }
    } else{
        statusBar()->showMessage(tr("Write error: ")+modbusDevice->errorString(),5000);
    }
    delay(1);
    request.setValue(0,0);
    if(auto *reply = modbusDevice->sendWriteRequest(request,0)){
        if(!reply->isFinished()){
            connect(reply,&QModbusReply::finished,this,[this,reply](){
                if(reply->error()==QModbusDevice::ProtocolError){
                    statusBar()->showMessage(tr("Write response error: %1 (Modbus exception: 0x%2)").
                                             arg(reply->errorString()).
                                             arg(reply->error(),-1,16),5000);
                } else if(reply->error() != QModbusDevice::NoError){
                    statusBar()->showMessage(tr("Write response error: %1 (Code: 0x%2)").
                                             arg(reply->errorString()).
                                             arg(reply->error(),-1,16),5000);
                }
                reply->deleteLater();
            });
        } else{
            reply->deleteLater();
        }
    } else{
        statusBar()->showMessage(tr("Write error: ")+modbusDevice->errorString(),5000);
    }
}
/* This function controls the jogging of movement down towards the work piece using
 * the AVC motor */
void MainWindow::on_avcDownButton_clicked()
{
    if(!modbusDevice)
        return;
    ui->replyBox->clear();
    statusBar()->clearMessage();

    // Coil 271 controls AVC Up
    const auto table = static_cast<QModbusDataUnit::RegisterType>(2);
    QModbusDataUnit request = QModbusDataUnit(table,272,1);
    request.setValue(0,1);
    if(auto *reply = modbusDevice->sendWriteRequest(request,0)){
        if(!reply->isFinished()){
            connect(reply,&QModbusReply::finished,this,[this,reply](){
                if(reply->error()==QModbusDevice::ProtocolError){
                    statusBar()->showMessage(tr("Write response error: %1 (Modbus exception: 0x%2)").
                                             arg(reply->errorString()).
                                             arg(reply->error(),-1,16),5000);
                } else if(reply->error() != QModbusDevice::NoError){
                    statusBar()->showMessage(tr("Write response error: %1 (Code: 0x%2)").
                                             arg(reply->errorString()).
                                             arg(reply->error(),-1,16),5000);
                }
                reply->deleteLater();
            });
        } else{
            reply->deleteLater();
        }
    } else{
        statusBar()->showMessage(tr("Write error: ")+modbusDevice->errorString(),5000);
    }
    delay(1);
    request.setValue(0,0);
    if(auto *reply = modbusDevice->sendWriteRequest(request,0)){
        if(!reply->isFinished()){
            connect(reply,&QModbusReply::finished,this,[this,reply](){
                if(reply->error()==QModbusDevice::ProtocolError){
                    statusBar()->showMessage(tr("Write response error: %1 (Modbus exception: 0x%2)").
                                             arg(reply->errorString()).
                                             arg(reply->error(),-1,16),5000);
                } else if(reply->error() != QModbusDevice::NoError){
                    statusBar()->showMessage(tr("Write response error: %1 (Code: 0x%2)").
                                             arg(reply->errorString()).
                                             arg(reply->error(),-1,16),5000);
                }
                reply->deleteLater();
            });
        } else{
            reply->deleteLater();
        }
    } else{
        statusBar()->showMessage(tr("Write error: ")+modbusDevice->errorString(),5000);
    }
}

//$$$$$$$$$$$$$$$$$$$$$$$ WELDING CONTROLS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

// Sam's going to put a button here

//********* PURGE BUTTON FUNCTIONS *********************************************************
/* This function controls the starting and stopping of the purge gas function */
void MainWindow::on_purgeButton_clicked()
{
    // Read coil 256 to determine if Liburdi is currently purging
    if(!modbusDevice)
        return;
    ui->replyBox->clear();
    statusBar()->clearMessage();

    // Coil 256 controls the purge function
    const auto table = static_cast<QModbusDataUnit::RegisterType>(2);
    QModbusDataUnit request = QModbusDataUnit(table,256,1);

    // If it is not purging, turn on purging, change button text to "Purging" and background
    if(ui->purgeButton->text()=="Purge"){
        ui->replyBox->clear();
        ui->replyBox->addItem("Turning purging ON");
        ui->purgeButton->setText("Purging");
        ui->purgeButton->setStyleSheet("background-color:red");

        request.setValue(0,1);
        if(auto *reply = modbusDevice->sendWriteRequest(request,0)){
            if(!reply->isFinished()){
                connect(reply,&QModbusReply::finished,this,[this,reply](){
                   if(reply->error() == QModbusDevice::ProtocolError){
                       statusBar()->showMessage(tr("Write response error: %1 (Modbus exception: 0x%2)").
                                                arg(reply->errorString()).
                                                arg(reply->rawResult().exceptionCode(),-1,16),5000);
                   }else if(reply->error() != QModbusDevice::NoError){
                       statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2").
                                                arg(reply->errorString()).
                                                arg(reply->error(),-1,16),5000);
                   }
                   reply->deleteLater();
                });
            } else{
                reply->deleteLater();
            }
        } else{
            statusBar()->showMessage(tr("Write error: ")+modbusDevice->errorString(),5000);
        }
    }

    // Else turn off purging, change button text to "Purge"
    else if(ui->purgeButton->text()=="Purging"){
        ui->replyBox->clear();
        ui->replyBox->addItem("Turning purging OFF");
        ui->purgeButton->setText("Purge");
        ui->purgeButton->setStyleSheet("background-color:button");

        request.setValue(0,0);
        if(auto *reply = modbusDevice->sendWriteRequest(request,0)){
            if(!reply->isFinished()){
                connect(reply,&QModbusReply::finished,this,[this,reply](){
                   if(reply->error() == QModbusDevice::ProtocolError){
                       statusBar()->showMessage(tr("Write response error: %1 (Modbus exception: 0x%2)").
                                                arg(reply->errorString()).
                                                arg(reply->rawResult().exceptionCode(),-1,16),5000);
                   }else if(reply->error() != QModbusDevice::NoError){
                       statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2").
                                                arg(reply->errorString()).
                                                arg(reply->error(),-1,16),5000);
                   }
                   reply->deleteLater();
                });
            } else{
                reply->deleteLater();
            }
        } else{
            statusBar()->showMessage(tr("Write error: ")+modbusDevice->errorString(),5000);
        }
    }
}

//$$$$$$$$$$$$$$$$$$$$$$$ MISC FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//******* DELAY FUNCTION *******************************************************************
/* Waits for a specified amount of time in milliseconds */
void MainWindow::delay(int time)
{
    QTime dieTime = QTime::currentTime().addSecs(time);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//******* UNIT CONVERSIONS **************************************************
/* This function converts the user input distance (in inches) to the number
 * of encoder clicks on the travel motor necessary to achieve the desired
 * motion */
int MainWindow::travDist2Click(double dist)
{
    // DETERMINE CONVERSION EXPERIMENTALLY
    int clicks = travScaleFactor*dist;
    return clicks;
}
/* This function converts the user input distance (in inches) to the number
 * of encoder clicks on the oscialltion motor necessary to achieve the desired
 * motion */
int MainWindow::oscDist2Click(double dist)
{
    // DETERMINE CONVERSION EXPERIMENTALLY
    int clicks = oscScaleFactor*dist;
    return clicks;
}

void MainWindow::on_customMessageButton_clicked()
{
    ui->replyBox->clear();
    if(ezservo->isOpen()){
        QByteArray command = "\r";
        QString message = ui->customMessageEdit->text();
        command.insert(0,message);
        ezservo->write(command);
    } else{
        ui->replyBox->addItem(tr("EZSErvo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}

void MainWindow::readData()
{
        QByteArray data = ezservo->readAll();
        int readSize = data.size()-3-4;
        QString selectData = QString::fromLatin1(data.mid(4,readSize));
        int posClicks = selectData.toInt();
        double posInches;
        if(lastEZServo == 1){
            posInches = (double) posClicks/travScaleFactor;
            ui->travCurrentPosEdit->setText(QString::number(posInches));
        } else{
            posInches = (double) posClicks/oscScaleFactor;
            ui->oscCurrentPosEdit->setText(QString::number(posInches));
        }
}

void MainWindow::ezservoInit()
{
    ui->replyBox->clear();
    QByteArray servo1FwdVel = "/1V16777216R\r";
    QByteArray servo1BwdVel = "/1U16777216R\r";
    QByteArray servo1Accel = "/1L65000R\r";
    QByteArray servo2FwdVel = "/2V16777216R\r";
    QByteArray servo2BwdVel = "/2U167772168R\r";
    QByteArray servo2Accel = "/2L65000R\r";

    ui->replyBox->addItem(tr("Initializing EZServo1"));
    ezservo->write(servo1FwdVel);
    delay(1);
    ezservo->write(servo1BwdVel);
    delay(1);
    ezservo->write(servo1Accel);
    delay(1);
    ui->replyBox->addItem(tr("Initializing EZServo2"));
    ezservo->write(servo2FwdVel);
    delay(1);
    ezservo->write(servo2BwdVel);
    delay(1);
    ezservo->write(servo2Accel);
    delay(1);
    ui->replyBox->addItem(tr("EZServos Initialized"));
}

void MainWindow::on_setCalibration1Button_clicked()
{
    double commandDist = ui->travDistanceEdit->text().toDouble();
    double actualDist = ui->calibration1Edit->text().toDouble();
    double ratio = commandDist/actualDist;
    travScaleFactor = travScaleFactor*ratio;
    ui->replyBox->clear();
    ui->replyBox->addItem(tr("New travel scale factor:"));
    ui->replyBox->addItem(QString::number(travScaleFactor));
}

void MainWindow::on_setCalibration2Button_clicked()
{
    double commandDist = ui->oscDistanceEdit->text().toDouble();
    double actualDist = ui->calibration2Edit->text().toDouble();
    double ratio = commandDist/actualDist;
    oscScaleFactor = oscScaleFactor*ratio;
    ui->replyBox->clear();
    ui->replyBox->addItem(tr("New oscillation scale factor:"));
    ui->replyBox->addItem(QString::number(oscScaleFactor));
}
