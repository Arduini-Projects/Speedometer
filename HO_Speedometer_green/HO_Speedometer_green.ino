/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED 7-Segment backpacks 
  ----> http://www.adafruit.com/products/881
  ----> http://www.adafruit.com/products/880
  ----> http://www.adafruit.com/products/879
  ----> http://www.adafruit.com/products/878

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 /**********
  * Modified 10/2017 as spedomoeter display for Texas Northern Model Railroad.
  * uses NCE BD20 as oppucancy detectors.
  */

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

Adafruit_7segment matrix = Adafruit_7segment();
//------------------------------------
float smph;          // Scale MPH
unsigned long MilliStart;        // Millisecond timer start value
unsigned long MilliEnd;          // Millisecond timer end value
unsigned long MilliDelta;        // Time delta thru trap
float TrapLength = 71.5;        // Length of speed trap in inches
// blue   outside 71.75 - 63.5,
// green    Insidde 71.75 - 63.6
// red  Mountain 71.75 - 62.0 
float scale = 87.1;      // HO scale factor      *** Change for your scale ***
//============================================================================
#define BD20_A A0
#define BD20_B A1

#define s_Idle 0
#define s_Start 1
#define s_End 2
#define s_Reset 3



int Status;

String Result;
int i,y;
unsigned int sensorValue;
unsigned int SensorArray[2] = {1024,1024};
unsigned int CalibrateArray[2] = {1024, 1024};
unsigned int TriggerArray[2] = {1024, 1024};




void setup() {
  Serial.begin(9600);

  Serial.println("HO Spedometer V1.0 ");
  Serial.println("Green");
  Serial.print("Speed trap length is: ");
  Serial.println(TrapLength);
  matrix.begin(0x70);
  pinMode(BD20_A, INPUT_PULLUP); 
  pinMode(BD20_B, INPUT_PULLUP);  
  Status = s_Idle;
  // show startup on display
  matrix.print(0xAAAA, HEX);
  matrix.drawColon(true);
  matrix.writeDisplay();  
  // Calibrate BD20 vaues at rest.
  for (y=0;y<10;y++) { 
  //  loop to get average
    Serial.println("Calibrate");
    sensorValue = analogRead(BD20_A);      
    if ( sensorValue != CalibrateArray[0] ) {
      CalibrateArray[0] = (sensorValue + CalibrateArray[0])/2; // average values
      
    }
    sensorValue = analogRead(BD20_B);      
    if ( sensorValue != CalibrateArray[1] ) {
      CalibrateArray[1] = (sensorValue + CalibrateArray[1])/2; // average values
      
    }
  Serial.print(CalibrateArray[0]);
  Serial.print("....");
  Serial.println(CalibrateArray[1]);
  delay(1000);        // delay in between reads for stability
  }
  TriggerArray[0] = CalibrateArray[0] - (CalibrateArray[0] / 4);
  TriggerArray[1] = CalibrateArray[1] - (CalibrateArray[1] / 4);
  Serial.print("triggers ");
  Serial.print(TriggerArray[0]);
  Serial.print("....");
  Serial.println(TriggerArray[1]);
  smph = 99.99;   
  matrix.print(smph);
//  matrix.setBrightness(7); // 50%
  matrix.writeDisplay();
  Status = s_Reset;
  MilliEnd = millis();
  
}
/////////////////////
float calcSpeed(float distance, unsigned int mStart, unsigned int mEnd) {

 unsigned int elapsed;
 float miles;
 float hours;
 float mph;
 float scaleMPH;
 float upHours;
 float upMinutes;
  
  elapsed = mEnd-mStart; // millis

  elapsed = elapsed /1000; // seconds
  Serial.print("Seconds: ");
  Serial.println(elapsed);
//  miles = distance / 63360; // miles
  float fudge = 0.20206659; // HO
// float fudge = 0.11; // N  
//  hours = elapsed / 3600; // hours
 mph = distance / elapsed;
  scaleMPH = mph / fudge;
//  scaleMPH = mph * scal;
  Serial.print("fudge: ");
  Serial.println(fudge,6);
  Serial.print("mph: ");
  Serial.println(mph, 6);
  Serial.print("fudge: ");
  Serial.println(fudge);
   
  Serial.print("Scale MPH: ");
  Serial.println(scaleMPH);
 
  // print a floating point
  //calcSpeed = scaleMPH;
  matrix.print(scaleMPH);
  matrix.writeDisplay();
  return(scaleMPH);

}

//////////////////////
void loop() {
  
  // read sensor values for processing
    Serial.print("loop---A: ");
    SensorArray[0] = analogRead(BD20_A);
    delay(100);          
    SensorArray[1] = analogRead(BD20_B);
    delay(100);        // delay in between reads for stability      
    
  Serial.print(SensorArray[0]);
  Serial.print("....B: ");
  Serial.println(SensorArray[1]);
  
  
  // case state
  // if idle, read A0, looking for trigger of 1/2 calibrated. DIsplay colon
  //    if trigger, state = s_Start, save current Millisecond count
  // if s_Start, look for second trigger at 1/2 of callibrated value Blink Colon
  //    if trigger, set to s_end, calculate smph, reset millisec counts
  // if s_End look for both A0 and A1 to rturn to callibrated value. Display MPH calue. 
  //     if trigger, set to S-reset, delay 5 sec
  // if s_Reset, clear smph to 0, set state to s_Idle Clear display.

   // set light status
  switch (Status) {
    case s_Idle:
      // no display, looking for next event;
      

 

      if (SensorArray[0] >= CalibrateArray[0]) {
        MilliDelta = SensorArray[0] - CalibrateArray[0];
        
      }
      else
      {
         MilliDelta = CalibrateArray[0] -SensorArray[0];
      }
       
//      Serial.print(SensorArray[0]);
//      Serial.print("....");
//      Serial.print( CalibrateArray[0]);
//      Serial.print("....");
//      Serial.println( MilliDelta);
  
      if ( SensorArray[0] <= TriggerArray[0] ) { // number drops by over 25%
        MilliStart = millis();
        Status = s_Start;
        Serial.print("Start ");  
        Serial.println(MilliStart);
        matrix.drawColon(true);
        matrix.blinkRate(1);
        matrix.writeDisplay();
        }  
      break;
    case s_Start:

//      Serial.print("Waiting for second section ");
//      Serial.print(SensorArray[1]);
//      Serial.print("....");
///      Serial.print( CalibrateArray[1]);
//      Serial.print("....");
//      Serial.println( TriggerArray[1]);
      ///MilliDelta = abs(SensorArray[1] - sensorValue);  
      if ( SensorArray[1] <= TriggerArray[1]) { // number drops by over 25%
        MilliEnd = millis();
        Serial.print("Trigger 2  ");  
        Serial.println(MilliEnd);
        Status = s_End;
        smph = calcSpeed(TrapLength, MilliStart, MilliEnd);
        Serial.print("Smph ");  
        Serial.println(smph);
        MilliStart = 0;
        matrix.drawColon(false);
        matrix.print(smph);
        matrix.blinkRate(0);
        matrix.writeDisplay();
      } 
    
    case s_End:       

        Serial.print("Showing ");
        Serial.println(millis());
        Serial.println("-------");  
        Serial.println(MilliEnd);
        if (( SensorArray[1] > TriggerArray[1]) && ( SensorArray[0] > TriggerArray[0])) { // number drops by over 25%
          if (( millis() - MilliEnd) < 150000) {
            MilliEnd = 0;
            MilliStart = 0;
            smph = 0.01;   
            Status = s_Reset;
            Serial.println("reset");
          }
        }
      break;
    
    case s_Reset:
       Status = s_Idle;
       matrix.blinkRate(2); // blink
       matrix.writeDisplay();
       delay(15000);
       matrix.clear();
       matrix.drawColon(true);
       matrix.blinkRate(0); // blink
       matrix.writeDisplay();;
      Serial.println("Back to idle");
      break;

  } // switch
} // loop
