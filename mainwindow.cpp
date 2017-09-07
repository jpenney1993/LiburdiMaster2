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

#include "linearactuator.h"

// still testing this stupid thing...
// hello there

int travScaleFactor;
int oscScaleFactor;
int lastEZServo;

double travCurrPos;
double oscCurrPos;

bool actuatorConnection = false; // Firgelli action monitor
int maxActuatorStep = 5; // mm

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
    initializeActuatorThread();
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
        //delay(2);
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
        //ui->replyBox->addItem(tr("Distance: %1").arg(QString::number(dist)));
        //ui->replyBox->addItem(tr("Clicks: %1").arg(QString::number(clicks)));
        //ui->replyBox->addItem(tr("Command:"));
        //ui->replyBox->addItem(command);
        //ui->travCheckButton->click();
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
   // ui->replyBox->addItem("Checking...");
    checkTravPos();
}

void MainWindow::checkTravPos()
{
    if(ezservo->isOpen()){
        QByteArray command = "/1?8\r";
        //ui->replyBox->addItem("Message Sent");
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
        delay(500);
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
        delay(500);
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
    //delay(1);
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
    //delay(1);
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

//********** WIRE FEED UP/DOWN FUNCTION ***************************************************
void MainWindow::initializeActuatorThread() // create thread for actuator
{
    QThread* actuatorThread = new QThread;
    linearActuator* actuatorWorker = new linearActuator;

    actuatorWorker->moveToThread(actuatorThread);

    //Connect to actuator
    connect(actuatorThread,SIGNAL(started()),actuatorWorker,SLOT(startActuatorThread()));

    //Cleanup connections
    connect(actuatorWorker,SIGNAL(finished()),actuatorThread,SLOT(quit()));
    connect(actuatorWorker,SIGNAL(finished()),actuatorWorker,SLOT(deleteLater()));
    connect(actuatorThread,SIGNAL(finished()),actuatorThread,SLOT(deleteLater()));
    actuatorThread->start();

    // --- Making Connections :) --- //
    // CONNCET GUI FUNCTIONS TO ACTUATOR FUNCTIONS
    // Functions labelled as follows:
    // Main GUI: push or fetch parameter (i.e. actuatorPushPosition to send position)
    // Actuator: send or receive parameter (i.e. actuatorReceivePosition to receive position)
    // Please use this naming convention for consistency. -matt

    // Connect to actuator via usb + return
    connect(this,SIGNAL(actuatorConnect()),actuatorWorker,SLOT(actuatorConnect()));
    connect(actuatorWorker,SIGNAL(actuatorConnected()),this,SLOT(actuatorConnected()));

    // Position control + feedback
    connect(this,SIGNAL(actuatorPushPosition(int)),actuatorWorker,SLOT(actuatorReceivePosition(int)));
    connect(actuatorWorker,SIGNAL(actuatorSendPositionRaw(int)),this,SLOT(actuatorFetchPositionRaw(int)));
    connect(actuatorWorker,SIGNAL(actuatorSendPositionMetric(float)),this,SLOT(actuatorFetchPositionMetric(float)));
    connect(this,SIGNAL(actuatorMoveUp(float)),actuatorWorker,SLOT(actuatorMoveUp(float)));
    connect(this,SIGNAL(actuatorMoveDown(float)),actuatorWorker,SLOT(actuatorMoveDown(float)));

    // Velocity control + feedback
    connect(this,SIGNAL(actuatorPushVelocity(float)),actuatorWorker,SLOT(actuatorReceiveVelocity(float)));
    connect(actuatorWorker,SIGNAL(actuatorSendVelocityRaw(int)),this,SLOT(actuatorFetchVelocityRaw(int)));
    connect(actuatorWorker,SIGNAL(actuatorSendVelocityMetric(float)),this,SLOT(actuatorFetchVelocityMetric(float)));
    connect(actuatorWorker,SIGNAL(actuatorSendVelocityReal(float)),this,SLOT(actuatorFetchVelocityReal(float)));

    // Oscillator toggle
    connect(this,SIGNAL(actuatorPushOscillate(int, int)),actuatorWorker,SLOT(actuatorReceiveOscillate(int, int)));
    connect(actuatorWorker,SIGNAL(actuatorSendOscillate(bool)),this,SLOT(actuatorFetchOscillate(bool)));

    // Get loop time
    connect(actuatorWorker,SIGNAL(actuatorSendDT(int)),this,SLOT(actuatorFetchDT(int)));

    // Run up/down control
    connect(this,SIGNAL(actuatorRunUp()),actuatorWorker,SLOT(actuatorRunUp()));
    connect(this,SIGNAL(actuatorRunDown()),actuatorWorker,SLOT(actuatorRunDown()));
    connect(this,SIGNAL(actuatorStopRunUp()),actuatorWorker,SLOT(actuatorStopRunUp()));
    connect(this,SIGNAL(actuatorStopRunDown()),actuatorWorker,SLOT(actuatorStopRunDown()));
}

// callback functions for the slots:
void MainWindow::actuatorConnected()
{
    //DISPLAY CONNECTION STATUS:
    ui->linActActiveCheckBox->setChecked(true);
    actuatorConnection = true;
}

void MainWindow::actuatorFetchDT(int input)
{
    float Hz = 1000.0/input;
    //DISPLAY REFRESH RATE:
    ui->wireThreadRefreshRate->setText(QString::number(Hz, 'f', 1) + "Hz");
}

void MainWindow::actuatorFetchPositionRaw(int input)
{
    //DISPLAY RAW POSITION:
    //ui->posStatus->setText(QString::number(input));
}

void MainWindow::actuatorFetchPositionMetric(float input)
{
    //DISPLAY METRIC POSITION:
    ui->wireHeightReadout->setText(QString::number(input, 'f', 2) + " mm");
}

void MainWindow::actuatorFetchVelocityRaw(int input)
{
    //DISPLAY RAW VELOCITY:
   // ui->label_RawVel->setText(QString::number(input));
}

void MainWindow::actuatorFetchVelocityMetric(float input)
{
    //DISPLAY METRIC VELOCITY:
    ui->wireStepVel->setText(QString::number(input, 'f', 1));
}

void MainWindow::actuatorFetchVelocityReal(float input)
{
    //DISPLAY DISCRETE VELOCITY:
    ui->wireHeightVelReadout->setText(QString::number(-input, 'f', 2) + " mm/s");
}

void MainWindow::actuatorFetchOscillate(bool state)
{
    if (state == true) // is oscillating
    {
        //ui->oscillateButton->setText(tr("Stop!"));
    }
    else if (state == false) // is not oscillating
    {
        //ui->oscillateButton->setText(tr("Oscillate!"));
    }
}

// UI Functions
void MainWindow::on_connectLinActButton_clicked()
{
    if (actuatorConnection == false)
    {
        actuatorConnect();
    }
}

void MainWindow::on_wireJogUpButton_clicked()
{
    float amount = ui->wireStepMM->text().toFloat();

    if (amount < 0)
    {
        amount = 0;
        ui->wireStepMM->setText("0");
    }
    if (amount > maxActuatorStep)
    {
        amount = maxActuatorStep;
        ui->wireStepMM->setText(QString::number(maxActuatorStep));
    }

    actuatorMoveUp(amount);
}

void MainWindow::on_wireJogDownButton_clicked()
{
    float amount = ui->wireStepMM->text().toFloat();

    if (amount < 0)
    {
        amount = 0;
        ui->wireStepMM->setText("0");
    }
    if (amount > maxActuatorStep)
    {
        amount = maxActuatorStep;
        ui->wireStepMM->setText(QString::number(maxActuatorStep));
    }

    actuatorMoveDown(amount);
}

void MainWindow::on_wireStepVel_editingFinished()
{
    actuatorPushVelocity(ui->wireStepVel->text().toFloat());
}

void MainWindow::on_wireRunUpButton_pressed()
{
    actuatorRunUp();
}

void MainWindow::on_wireRunDownButton_pressed()
{
    actuatorRunDown();
}

void MainWindow::on_wireRunUpButton_released()
{
    actuatorStopRunUp();
}

void MainWindow::on_wireRunDownButton_released()
{
    actuatorStopRunDown();
}

//$$$$$$$$$$$$$$$$$$$$$$$ WELDING CONTROLS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//********* WELD BUTTON FUNCTIONS **********************************************************

void MainWindow::TravelADistance(double dist)
{
    ui->replyBox->clear();
    QString clickString;
    QByteArray command;
    int clicks;
    if(ezservo->isOpen())
    {
        if(dist > 0)
        {
            command = "/1PR\r";
            clicks = travDist2Click(dist);
            clickString = QString::number(clicks);
        }
        else
        {
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
    }
    else
    {
        ui->replyBox->addItem(tr("EZServo board not connected"));
        statusBar()->showMessage(tr("ERROR: No EZServo"));
    }
}

void MainWindow::on_weldButton_clicked()
{
        // Start by disabling the weld button so only one operation can run at a time
        ui->weldButton->setEnabled(false);
        // Read the pre-purge, post-purge, travel delay, and downsloap times

        if(!modbusDevice)
            return;
        ui->replyBox->clear();
        statusBar()->clearMessage();

        // Register 279 controls the pre-purge delay time
        const auto prepTable = static_cast<QModbusDataUnit::RegisterType>(4);
        QModbusDataUnit prepRequest = QModbusDataUnit(prepTable,279,2);
        if(auto *reply = modbusDevice->sendReadRequest(prepRequest,0)){
            //ui->replyBox->addItem("Reply recieved");
            if(!reply->isFinished()){
                //ui->replyBox->addItem("It's not finished");
                connect(reply,&QModbusReply::finished,this,[this,reply](){
                   if(reply->error() == QModbusDevice::ProtocolError){
                       statusBar()->showMessage(tr("Write response error: %1 (Modbus exception: 0x%2)").
                                                arg(reply->errorString()).
                                                arg(reply->rawResult().exceptionCode(),-1,16),5000);
                   }else if(reply->error() != QModbusDevice::NoError){
                       statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2").
                                                arg(reply->errorString()).
                                                arg(reply->error(),-1,16),5000);
                   } else{
                       //ui->replyBox->addItem("RESULTS!");
                       auto prepReply = reply->result();
                       int length = prepReply.valueCount();
                       //ui->replyBox->addItem(tr("length of array: %1").arg(length));
                       ui->replyBox->addItem(tr("Pre-purge delay: %1s").arg(prepReply.value(length-1)));
                   }
                   reply->deleteLater();
                });
            } else{
                reply->deleteLater();
            }
        } else{
            statusBar()->showMessage(tr("Write error: ")+modbusDevice->errorString(),5000);
        }

        // Register 280 controls the post-purge delay time
        const auto postpTable = static_cast<QModbusDataUnit::RegisterType>(4);
        QModbusDataUnit postpRequest = QModbusDataUnit(postpTable,280,2);
        if(auto *reply = modbusDevice->sendReadRequest(postpRequest,0)){
            if(!reply->isFinished()){
                connect(reply,&QModbusReply::finished,this,[this,reply](){
                    if(reply->error() == QModbusDevice::ProtocolError){
                        statusBar()->showMessage(tr("Write response error: %1 (Modbus exception: 0x%2").
                                                 arg(reply->errorString()).
                                                 arg(reply->error(),-1,16),5000);
                    } else if(reply->error() != QModbusDevice::NoError){
                        statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                                                 arg(reply->errorString()).
                                                 arg(reply->error(),-1,16),5000);
                    } else{
                        auto postpReply = reply->result();
                        int length = postpReply.valueCount();
                        ui->replyBox->addItem(tr("Post-purge delay: %1s").arg(postpReply.value(length-1)));
                    }
                });
            }
        }
    /*
        // Register 281 controls the travel delay time
        const auto trav_table = static_cast<QModbusDataUnit::RegisterType>(4);
        QModbusDataUnit trav_request = QModbusDataUnit(trav_table,281,1);
        QVector<quint16> trav_delay_array=trav_request.values();
        double trav_delay=trav_delay_array[0];
        ui->replyBox->addItem(tr("Travel delay established as %1s").arg(trav_delay));

        // Register 282 controls the downsloap time
        const auto downsl_table = static_cast<QModbusDataUnit::RegisterType>(4);
        QModbusDataUnit downsl_request = QModbusDataUnit(downsl_table,282,1);
        QVector<quint16> downsl_array=downsl_request.values();
        double downsloap_delay=downsl_array[0];
        ui->replyBox->addItem(tr("Downslope time established as %1s").arg(downsloap_delay));
    */
        // Activate welding
        // Coil 262 controls the welding sequence start
        const auto weldOnTable = static_cast<QModbusDataUnit::RegisterType>(2);
        QModbusDataUnit weldOnRequest = QModbusDataUnit(weldOnTable,262,1);
        weldOnRequest.setValue(0,1);
        if(auto *reply = modbusDevice->sendWriteRequest(weldOnRequest,0)){
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

        // Wait for the travely delay and pre-purge times
        //double startWaitTime=pre_purge_delay+trav_delay;
        double startWaitTime = 15*1000;
        delay(startWaitTime);

        // Travel the desired distance while welding
        double dist = ui->weldDistanceEdit->text().toDouble();
        ui->replyBox->addItem(tr("Weld distance= %1in").arg(dist));
        TravelADistance(dist);
        //double actDist = ui->travCurrentPosEdit->text().toDouble();
        while(travCurrPos < dist*0.95){
            checkTravPos();
            delay(250);
            QApplication::processEvents();
        }

        // Deactivate welding
        // Coil 263 controls the welding sequence stop
        const auto weldOffTable = static_cast<QModbusDataUnit::RegisterType>(2);
        QModbusDataUnit weldOffRequest = QModbusDataUnit(weldOffTable,263,1);
        weldOffRequest.setValue(0,1);
        if(auto *reply = modbusDevice->sendWriteRequest(weldOffRequest,0)){
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

        // Wait for the post-purge and downsloap times
        //double endWaitTime=downsloap_delay+post_purge_delay;
        double endWaitTime = 20*1000;
        delay(endWaitTime);

        // Finish by re-enabling the weld button
        ui->weldButton->setEnabled(true);
}

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
    QTime dieTime = QTime::currentTime().addMSecs(time);
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
        //QString dataString = QString::fromLatin1(data.data());
        int posClicks = selectData.toInt();
        double posInches;
        if(lastEZServo == 1){
            posInches = (double) posClicks/travScaleFactor;
            ui->travCurrentPosEdit->setText(QString::number(posInches));
            travCurrPos = posInches;
            //return posInches;
        } else{
            posInches = (double) posClicks/oscScaleFactor;
            ui->oscCurrentPosEdit->setText(QString::number(posInches));
            oscCurrPos = posInches;
            //return posInches;
        }
        //ui->replyBox->addItem(tr("pos of %1: %2in").arg(lastEZServo).arg(dataString));
}

void MainWindow::ezservoInit()
{
    ui->replyBox->clear();
    //QByteArray servo1FwdVel = "/1V8388608R\r"; // was 16777216
    //QByteArray servo1BwdVel = "/1U8388608R\r"; // was 16777216
    QByteArray servo1FwdVel = "/1V1622768R\r"; // -> 2.2 in/min
    QByteArray servo1BwdVel = "/1U1622768R\r";
    QByteArray servo1Accel = "/1L65000R\r";
    QByteArray servo2FwdVel = "/2V16777216R\r";
    QByteArray servo2BwdVel = "/2U167772168R\r";
    QByteArray servo2Accel = "/2L65000R\r";

    ui->replyBox->addItem(tr("Initializing EZServo1"));
    ezservo->write(servo1FwdVel);
    delay(1000);
    ezservo->write(servo1BwdVel);
    delay(1000);
    ezservo->write(servo1Accel);
    delay(1000);
    ui->replyBox->addItem(tr("Initializing EZServo2"));
    ezservo->write(servo2FwdVel);
    delay(1000);
    ezservo->write(servo2BwdVel);
    delay(1000);
    ezservo->write(servo2Accel);
    delay(1000);
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
