

int GSR = A0;
int threshold = 0;
int sensorValue;

int vibe = 17;
//int H1 = 16;
//int H2 = 15;

void setup() {
  Serial.begin(9600);
  long sum = 0;
  for (int i = 0; i < 500; i++)
  {
    sensorValue = analogRead(GSR);
    sum += sensorValue;
    delay(5);
  }
  threshold = sum / 500;
  Serial.print("threshold =");
  Serial.println(threshold);

  for (int i = 0; i < 5; i++) {
    analogWrite(vibe, 175);
    //analogWrite(H1, 175);
    //analogWrite(H2, 175);
    delay(250);
    analogWrite(vibe, 0);
    // analogWrite(H1, 0);
    // analogWrite(H2, 0);
    delay(250);
  }

}
void loop() {
  //Stress Gauge Code Start
  int GSRtemp;
  sensorValue = analogRead(GSR);
  Serial.print("GSR = ");
  Serial.println(sensorValue);
  GSRtemp = threshold - sensorValue;
  if (abs(GSRtemp) > 60)
  {
    sensorValue = analogRead(GSR);
    GSRtemp = threshold - sensorValue;
    if (abs(GSRtemp) > 60) {
      Serial.println("Possible Stress Detected!");
      for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
        // sets the value (range from 0 to 255):
        analogWrite(vibe, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(30);
        for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
          // sets the value (range from 0 to 255):
          analogWrite(vibe, fadeValue);
          // wait for 30 milliseconds to see the dimming effect
          delay(30);
        }

      }

    }
  }

}
