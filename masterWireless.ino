#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "RTClib.h"

#define DHTPIN 2     // what digital pin we're connected to
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
//*********************************************LCD Stuff******************//
#define I2C_ADDR    0x27 // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define SLAVES  4
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

DHT dht(DHTPIN, DHTTYPE);
RF24 radio(7, 8); // CE, CSN
RTC_DS3231 rtc;

int msg[1] = {1};
int rec[1] = {5};
bool stat = true;
//const byte pipe[][6] = {"00001", "00002","00003","00004"};

const int tempUpdateTime = 1000; //Time inbetween polling Temp Sensor
const int radioUpdateTime = 1000; //Time inbetween polling Temp Sensor
const int clearDisplayTime = 5000; //Time inbetween polling Temp Sensor
const byte pipe[][6] = {"00001", "00002"};
//const uint64_t pipe[1]= {0xF0F0F0F0E1LL};
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


double humidity[SLAVES];
double ftemp[SLAVES];
int fromSlaves[SLAVES];
int previousMillis1 = 0; 
int previousMillis2 = 0; 
int previousMillis3 = 0;
byte tempreturn = 0;
int tempTesting = 4444;

void setup()
{

  memset(humidity,0,sizeof(humidity));
  memset(ftemp,0,sizeof(ftemp));
  Serial.begin(9600);
  lcd.begin (20,4);
  dht.begin();
  radio.begin();
  delay(100);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.stopListening();
  radio.openWritingPipe(pipe[0]);
  radio.setRetries(15,15);
   
  // Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  //radio.openWritingPipe(addresses[0]); // 00002
  //radio.openReadingPipe(1, addresses[1]); // 00001
  radio.setPALevel(RF24_PA_MIN);
 // radio.setPALevel(RF24_PA_HIGH);

 // if (rtc.lostPower()) {
  //  Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
  //  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  //}
}


void loop()
{

  DateTime now = rtc.now();
  delay(5);
  radio.startListening();

  
 // if(radio.available()){
 //   radio.read(&ftemp[1], sizeof(ftemp[1]));
   
 // }
  
  int currentMillis = millis();
  if (currentMillis - previousMillis1 >= tempUpdateTime) {
      previousMillis1 = currentMillis; //save the last time ran this.
  //    tempreturn = getTemp();
  }

  if (currentMillis - previousMillis2 >= radioUpdateTime) {
      previousMillis2 = currentMillis; //save the last time ran this.
   //   const char text[] = "mUptime";
    //  radio.write(&text, sizeof(text));
   //   unsigned long uptime = millis();
   //   radio.write(&uptime, sizeof(unsigned long));
  }

  if (currentMillis - previousMillis3 >= clearDisplayTime) {
      previousMillis3 = currentMillis; //save the last time ran this.
   //   const char text[] = "mUptime";
    //  radio.write(&text, sizeof(text));
   //   unsigned long uptime = millis();
   //   radio.write(&uptime, sizeof(unsigned long));
     lcd.clear();  }
     
  lcd.home (); // go home
  lcd.print(now.year(), DEC); lcd.print('/'); lcd.print(now.month(), DEC);
  lcd.print('/'); lcd.print(now.day(), DEC); lcd.print(" ");
  if(now.hour() <= 9) { lcd.print("0"); lcd.print(now.hour(),DEC);}
  else {lcd.print(now.hour(),DEC);}
  if(now.minute() <= 9) { lcd.print("0"); lcd.print(now.minute(),DEC);}
  else {lcd.print(now.minute(),DEC);}
  lcd.print(":"); 
  if(now.second() <= 9) { lcd.print("0"); lcd.print(now.second(),DEC);}
  else {lcd.print(now.second(),DEC);}
  lcd.setCursor (0,1);
  lcd.print("Remote Sensors: ");
  lcd.setCursor (0,2);
  lcd.print("1 "); lcd.print(ftemp[1]); lcd.print("F");
  lcd.setCursor (11,2); 
  lcd.print("2 "); lcd.print("  .  F");
  lcd.setCursor (0,3); 
  lcd.print("3 "); lcd.print("  .  F");
  lcd.setCursor (11,3); 
  lcd.print("4 "); lcd.print("  .  F");




  if(radio.write(tempTesting, sizeof(tempTesting)))
  {
 Serial.print("bacon");  }/*
    Serial.print( tempTesting );
    Serial.println("...tx sucess");
    if(radio.isAckPayloadAvailable())
    {
      radio.read(fromSlaves[0], sizeof(fromSlaves[0]));
      Serial.print("received ack payload is: ");
      Serial.println(fromSlaves[0]);
    }
    else
    {
      Serial.println("something is wrong");
    }
  }*/
        
}

//==================================Functions============================================//

byte getTemp(){
  humidity[0] =  dht.readHumidity();
  ftemp[0] =  dht.readTemperature(true); // Read temperature as Fahrenheit (isFahrenheit = true)
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity[0]) || isnan(ftemp[0])) {  //isnan(ctemp) ||
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  return 0;
} 
