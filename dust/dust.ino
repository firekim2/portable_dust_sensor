#include <SoftwareSerial.h>

int blueTx = 4;
int blueRx = 5;
SoftwareSerial mySerial(blueTx, blueRx);

int dustPin =A0;
int dustVal=0;
int count = 0 ;
float ppm =0;
char s[32];
float voltage = 0;
float dustdensity =0;
float ppmpercf =0;
unsigned long lastSensingTime=0;
unsigned long lastConnectionTime=0;
const unsigned long SENSING_INTERVAL = 2000;

int ledPower=2;
int delayTime=280;
int delayTime2=40;
float offTime=9680;
int ppmMax=0;
int ppmMin=0;
const unsigned long POSTING_INTERVAL = 30*1000;

void setup() {
  pinMode(ledPower,OUTPUT);
    Serial.begin(9600);
    mySerial.begin(9600);
   // pinMode(probe,INPUT);
}

void loop() {
  if(millis() - lastSensingTime > SENSING_INTERVAL) {
    readDustSensor();
    lastSensingTime = millis();
  }

  if(millis() - lastConnectionTime > POSTING_INTERVAL) {
    // calculate average voltage, dust density
    ppm -= ppmMin;
    ppm -= ppmMax;
    voltage = ppm/(count-2)*0.004882;
    dustdensity = 100*(voltage-1.25);
    ppmpercf = (voltage-0.0256)*120000;
    if (ppmpercf < 0)
      ppmpercf = 0;
    if (dustdensity < 0 )
      dustdensity = 0;
    if (dustdensity > 500)
      dustdensity = 500;

    // for debug
    String dataString = "";
    dataString += dtostrf(dustdensity, 5, 4, s);
    dataString += " ug/m3";
    Serial.println(dataString);
    writeString(dataString);

    count=0;
    ppm=0;
    ppmMin=0;
    ppmMax=0;
    lastConnectionTime = millis();
  }
   if (mySerial.available()) {
      Serial.write(mySerial.read());
    }
    if (Serial.available()) {
      mySerial.write(Serial.read());
      //mySerial.write(dustdensity);
    }
}
void writeString(String stringData) {

  for (int i = 0; i < stringData.length(); i++)
  {
    mySerial.write(stringData[i]);
  }
  mySerial.write("\n");
}

void readDustSensor() {

  count = count + 1;
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(delayTime);

  dustVal=analogRead(dustPin);
  if(dustVal > ppmMax){
    ppmMax = dustVal;
  }
  else if(dustVal < ppmMin){
    ppmMin = dustVal;
  }
 // Serial.println(dustVal);
  ppm = ppm+dustVal;

  delayMicroseconds(delayTime2);

  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(offTime);

  delay(2000);

  String strVoltage = "Voltage=";
  strVoltage += dtostrf(dustVal, 5, 2, s);
}
