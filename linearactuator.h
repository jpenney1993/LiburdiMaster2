#ifndef LINEARACTUATOR_H
#define LINEARACTUATOR_H

#include <QObject>
#include <QThread>
#include <QTime>

class linearActuator : public QObject
{ Q_OBJECT
public:
    linearActuator();

    // Set Command Codes
    struct actuatorCommands
    {
        int set_accuracy = 0x01;
        int set_retract_limit = 0x02;
        int set_extend_limit = 0x03;
        int set_movement_threshold = 0x04;
        int set_stall_time = 0x05;
        int set_pwm_threshold = 0x06;
        int set_derivative_threshold = 0x07;
        int set_derivative_maximum = 0x08;
        int set_derivative_minimum = 0x09;
        int set_pwm_maximum = 0x0A;
        int set_pwm_minimum = 0x0B;
        int set_proportional_gain = 0x0C;
        int set_derivative_gain = 0x0D;
        int set_average_rc = 0x0E;
        int set_average_adc = 0x0F;
        int get_feedback = 0x10;
        int set_position = 0x20;
        int set_speed = 0x21;
        int disable_manual = 0x30;
        int reset = 0xFF;
    };

    // Set default settings - initialized upon connection
    struct defaultSettings
    {
        int accuracy = 3; // lowest stable #: 2 oscillates some
        int velocity = 630;
        int position = 100;
    };

    actuatorCommands commands; // Instantiate it!
    defaultSettings settings;

private:
    // Startup
    void initializeDefaults();
    void runActuator();

    // Controls + feedback
    int getPositionRaw();
    void setPosition(int position);
    void setVelocity(int velocity);

    // Internal stuff
    int sendPacket(int commandCode, int value);
    void processFinished();
    void delay(int millisecondsToWait);
    void toggleOscillation(int min, int max);

    // Unit conversions
    float convertPosToMetric(int rawValue);
    float convertVelToMetric(int rawValue);
    int convertMetricToInternal(float metric);

public slots:
    // Startup
    void startActuatorThread();
    void actuatorConnect();

    // Controls
    void actuatorReceivePosition(int input);
    void actuatorReceiveVelocity(float input);
    void actuatorReceiveOscillate(int min, int max);
    void actuatorMoveUp(float input);
    void actuatorMoveDown(float input);
    void actuatorRunUp();
    void actuatorRunDown();
    void actuatorStopRunUp();
    void actuatorStopRunDown();

signals:
    // Feedback, generally
    void actuatorConnected();
    void actuatorSendDT(int);
    void actuatorSendPositionRaw(int);
    void actuatorSendPositionMetric(float);
    void actuatorSendVelocityRaw(int);
    void actuatorSendVelocityMetric(float);
    void actuatorSendVelocityReal(float);
    void actuatorSendOscillate(bool);
    void finished();
};

#endif // LINEARACTUATOR_H
