#include <AccelStepper.h>

// for the Arduino Uno + CNC shield V3 + A4988 + FL42STH47-1684A

#define MOTOR_X_ENABLE_PIN 8
#define MOTOR_X_STEP_PIN 2
#define MOTOR_X_DIR_PIN 5
#define MOTOR_Y_ENABLE_PIN 8
#define MOTOR_Y_STEP_PIN 3
#define MOTOR_Y_DIR_PIN 6
#define MOTOR_Z_ENABLE_PIN 8
#define MOTOR_Z_STEP_PIN 4
#define MOTOR_Z_DIR_PIN 7

#define NUM_MOTORS 3

AccelStepper motor_X(1, MOTOR_X_STEP_PIN, MOTOR_X_DIR_PIN);
AccelStepper motor_Y(1, MOTOR_Y_STEP_PIN, MOTOR_Y_DIR_PIN);
AccelStepper motor_Z(1, MOTOR_Z_STEP_PIN, MOTOR_Z_DIR_PIN);
AccelStepper *motors[NUM_MOTORS] = {&motor_X, &motor_Y, &motor_Z};

unsigned char serialBuffer[10];
int bufferIndex = 0;
bool dataReady = false;

void setup()
{
  SerialUSB.begin(1843200);
  (*motors[0]).setEnablePin(MOTOR_X_ENABLE_PIN);
  (*motors[1]).setEnablePin(MOTOR_Y_ENABLE_PIN);
  (*motors[2]).setEnablePin(MOTOR_Z_ENABLE_PIN);
  for(int i=0; i < NUM_MOTORS; i++)
  {
    (*motors[i]).setPinsInverted(false, false, true);
    (*motors[i]).setAcceleration(800000);  
    (*motors[i]).setMaxSpeed(8000);
    (*motors[i]).moveTo(1600);
    (*motors[i]).enableOutputs();
  }
}

void loop()
{
  if(dataReady)
  {
    // get 4-bit motor address and command
    unsigned char motor = serialBuffer[0] & 0x03;
    unsigned char command = (serialBuffer[0] >> 2) & 0x3F;

    switch(command)
    {
      case 0x00: 
      {
        SerialUSB.print("home motor ");
        SerialUSB.println(motor);
        (*motors[motor]).moveTo(0);
        break;
      }
      default: SerialUSB.println("aahhhhhh"); break;
    }
    dataReady = false;
    bufferIndex = 0;
  }

  for(int i=0; i < NUM_MOTORS; i++) (*motors[i]).run();
  
  // call SerialEvent manually because Arduino Due firmware is awful
  if (SerialUSB.available()) serialEvent();
}

// "interrupt" for serial data receiving between iterations of loop()
void serialEvent() {
  while (SerialUSB.available()) {
    serialBuffer[bufferIndex] = (unsigned char)SerialUSB.read();
    if(serialBuffer[bufferIndex] == 0x0A)
    {
      dataReady = true;
    }
    bufferIndex++;
  }
}
