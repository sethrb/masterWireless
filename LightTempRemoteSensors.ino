#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
RF24 radio(7, 8); // CE, CSN
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
const int tempUpdateTime = 2000; //Time inbetween polling Temp Sensor
const int radioUpdateTime = 100; //Time inbetween polling Temp Sensor
const byte pipe[][6] = {"00001", "00002","00003","00004"};
//const uint64_t pipe[1]= {0xF0F0F0F0E1LL};
double tempF = 0;
int previousMillis0 = 0; 
int previousMillis1 = 0; 
int photocellPin = A7;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
int fromMaster = 0;
int test = 1234;
// DS18S20 Temperature chip i/o
OneWire ds(2);  // on pin 10

void setup(void) {
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  Serial.begin(9600);   
  radio.begin();
  delay(100);
  sensors.begin();
 // radio.setAutoAck(true);
 // radio.enableAckPayload();
 // radio.enableDynamicPayloads();
 // radio.openReadingPipe(1,pipe[1]);
  radio.openWritingPipe(pipe[1]); // 00002
  radio.openReadingPipe(1, pipe[0]); // 00001
 // radio.startListening();

//  radio.setRetries(15,15);
  radio.setPALevel(RF24_PA_MIN);
}
 
void loop(void) {
  delay(5);
  radio.stopListening();
  int currentMillis = millis();
  if (currentMillis - previousMillis0 >= tempUpdateTime) {
      previousMillis0 = currentMillis; //save the last time ran this.
      tempF = getTemp();
  }

  radio.write(&tempF, sizeof(tempF));
  
  
  if (currentMillis - previousMillis1 >= radioUpdateTime) {
      previousMillis1 = currentMillis; //save the last time ran this.
      Serial.print("Temperature for Device 1 is: ");
      Serial.print(tempF); // Why "byIndex"? 
      Serial.print("     ");
      photocellReading = analogRead(photocellPin);
      Serial.print("Analog reading = ");
      Serial.println(photocellReading);     // the raw analog reading
      /*
      const char text[] = "mUptime";
      radio.write(&text, sizeof(text));
      unsigned long uptime = millis();
      radio.write(&uptime, sizeof(unsigned long)); */
  }

  
 
    // You can have more than one IC on the same bus. 
    // 0 refers to the first IC on the wire
/*  if(radio.available() ){
    radio.writeAckPayload( 1, test, sizeof(test));
    radio.read( &fromMaster,sizeof(fromMaster));
    Serial.print("integer recived is: ");
    Serial.println(fromMaster);
  }*/
}
//***************************Functions*********************************//
double getTemp(){
  float tempC = 0;
  Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  tempC = sensors.getTempCByIndex(0);
  tempF = tempC * 1.8 + 32;
  return tempF;
}
