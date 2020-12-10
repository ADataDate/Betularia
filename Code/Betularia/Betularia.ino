

#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.  
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h" 

//  Variables
const int PulseWire = A0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int GSR = A1;             //GSR pin
int threshold1 = 0;
int sensorValue = 0;
int hr;
int sum = 0;
int sum2 = 0;
int threshold2 = 0;
const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550;



const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;



PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"
MAX30105 particleSensor;

void setup() {

  Serial.begin(9600);          // For Serial Monitor
   // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  // Configure the PulseSensor object, by assigning our variables to it.
  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);

  // Double-check the "pulseSensor" object was created and "began" seeing a signal.
  //if (pulseSensor.begin()) {
  //Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.
  //}
  Serial.println("Begin Calibration");
  //Begin GSR/heart rate calibration:
  for (int i = 0; i < 500; i++)
  {
    sensorValue = analogRead(GSR);
    int myBPM = pulseSensor.getBeatsPerMinute();

    sum += sensorValue;
    sum2 += myBPM;
    delay(5);
  }
  threshold1 = sum / 500;
  threshold2 = sum2 / 500;


}

void loop() {
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();
  
  int HRtemp;
  int BPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
  HRtemp = threshold2 - BPM;
  //if (pulseSensor.sawStartOfBeat()) {            // Constantly test to see if "a beat happened".
  // Serial.print("BPM: ");                        // Print phrase "BPM: "
  // Serial.println(BPM);
  // }

  
  int GSRtemp;
  sensorValue = analogRead(GSR);
  Serial.print("BPM: ");
  Serial.println(BPM);
  Serial.print("GSR = ");
  Serial.println(sensorValue);
  GSRtemp = threshold1 - sensorValue;
  if (abs(GSRtemp) > 60 && abs(HRtemp) > 20)
  {
    Serial.println("Possible Stress Detected!");
  }
  delay(20);

}
