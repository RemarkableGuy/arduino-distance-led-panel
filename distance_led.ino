// # Connection:
// #       Pin 1 VCC (URM V3.2) -> VCC (Arduino)
// #       Pin 2 GND (URM V3.2) -> GND (Arduino)
// #       Pin 4 PWM (URM V3.2) -> Pin 11 (Arduino)
// #       Pin 6 COMP/TRIG (URM V3.2) -> Pin 12 (Arduino)
// # Pin mode: PWM
// # Working Mode: PWM passive control mode.

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define CLK 8
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3


int URPWM = 11;        // PWM Output 0-25000us,every 50us represent 1cm
int URTRIG = 12;       // PWM trigger pin

unsigned int distance = 0;

int color = 0;

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

uint8_t EnPwmCmd[4]={0x44,0x22,0xbb,0x01}; // distance measure command

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);
  
void setup() {             // Serial initialization
    Serial.begin(9600);    // Sets the baud rate to 9600
    // initialize all the readings to 0:
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readings[thisReading] = 0;
    }
    PWM_Mode_Setup();
    matrix.begin();
}
  
void loop() {
  distance = PWM_Mode();
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = distance;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  
  //Serial.println(distance);
  //Serial.println(average);
  // send it to the computer as ASCII digits
    matrix.fillCircle(0,0,22.5,matrix.ColorHSV(average * 10, 255, 255, true));
    matrix.fillCircle(matrix.width(),matrix.height(),22.5,matrix.ColorHSV(distance * 10, 255, 255, true));
  //matrix.drawRect(0,0,16,16,matrix.ColorHSV(average * 10, 255, 255, true));
  //matrix.fillScreen(matrix.ColorHSV(average * 10, 255, 255, true));
  delay(20);
}
 
  
void PWM_Mode_Setup(){
    pinMode(URTRIG,OUTPUT);       // A low pull on pin COMP/TRIG
    digitalWrite(URTRIG,HIGH);    // Set to HIGH
  
    pinMode(URPWM, INPUT);        // Sending Enable PWM mode command
     
    for (int i=0; i < 4; i++) {
        Serial.write(EnPwmCmd[i]);
    }
}
  
int PWM_Mode(){                   // a low pull on pin COMP/TRIG  triggering a sensor reading
    digitalWrite(URTRIG, LOW);
    digitalWrite(URTRIG, HIGH);   // reading Pin PWM will output pulses
      
    unsigned long DistanceMeasured=pulseIn(URPWM,LOW);
      
    if (DistanceMeasured==50000) { // the reading is invalid.
      return 0;
    } else {
        return DistanceMeasured/50; // every 50us low level stands for 1cm
    }
}
