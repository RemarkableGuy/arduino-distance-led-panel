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
 
uint8_t EnPwmCmd[4]={0x44,0x22,0xbb,0x01}; // distance measure command

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);
  
void setup() {             // Serial initialization
    Serial.begin(9600);    // Sets the baud rate to 9600
    PWM_Mode_Setup();
    matrix.begin();
}
  
void loop() {
  distance = PWM_Mode();
  Serial.println(distance);
  matrix.fillScreen(matrix.ColorHSV(distance * 10, 255, 255, true));
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
