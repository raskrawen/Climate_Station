//ALL
int pauseTid = 2000; // 2 sekunder
int n=0; // antal gentagelser før data lagres på SD

//LCD
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR    0x3F // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

//SD
#include <SD.h> // åbner biblioteker for at tale med SD-kortlæser (I2C)
#include<SPI.h>
#include "Wire.h" //(er loaded ovenfor?)
File mySensorData; // laver et fil-objekt, som kan gemme data

//JORDFUGTIGHED
//max 1023 (ingen fugtighed) min 132 (i vand)
double Fugt;
double Fugt2;

//FUGTIGHED og LUFTTEMP
#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 3
#include "DHT.h"
#define DHT11Pin 2
DHT dht(DHT11Pin, DHT11);

//LYS
int sensorPin = A1;    // select the input pin for the potentiometer
double lys = 0;  // variable to store the value coming from the sensor

//JORDTEMP
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


void setup(void)
{
//ALL
// start serial port
  Serial.begin(9600);

//LCD
 lcd.begin (16,2); //  <<----- My LCD was 16x2
// Switch on the backlight
lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
lcd.setBacklight(HIGH);
lcd.home (); // go home
lcd.print("Setup.."); 

//SD
pinMode(10, OUTPUT); // Pin 10 reserveres til SD kortlæseren. Nødvendigt?
SD.begin(4); // starter SD kortlæseren
if (SD.exists("Kdata.txt")) // hvis filen "Kdata.txt" (max 8 tegn) findes på SDkortet..
{
SD.remove("Kdata.txt"); // ..slet filen.
}

//JORDFUGTIGHED

//TEMP OG LUFTFUGTUGHED

//LYS

//JORDTEMP
Serial.println("Setup..");
Serial.println();
  // Start up the library
  sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
}


void loop(void)
{ 
//ALL
String dataString = "";
n=n+1;
delay(pauseTid);

//LCD
 
//JORDFUGTIGHED
  Fugt = analogRead(A2);
  Fugt = 100 - 100*Fugt/1023;
  Serial.print("Jordfugtighed (%): ");
  Serial.println(Fugt);
 lcd.setCursor (0,0);
 lcd.setBacklight(HIGH);
 lcd.print("Jordfugtighed:");
 lcd.setCursor (0,1);
 lcd.print(Fugt);
 lcd.print(" %    ");
 delay(pauseTid);


//JORDTEMP
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.print("Jordtemperatur (grader Celcius): ");
  Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
 lcd.setCursor (0,0);
 lcd.setBacklight(HIGH);
 lcd.print("Jordtemperatur:");
 lcd.setCursor (0,1);
 lcd.print(sensors.getTempCByIndex(0));
 lcd.print(" oC   ");
 delay(pauseTid);

  
//LUFTTEMP OG LUFTFUGTIGHED
  // get humidity
  float humidity = dht.readHumidity();
  // get temperature as C
  float celsius = dht.readTemperature();
  // print results
  Serial.print("Luftfugtighed (%): "); Serial.println(humidity);
 lcd.setCursor (0,0);
 lcd.setBacklight(HIGH);
 lcd.print("Luftfugtighed: ");
 lcd.setCursor (0,1);
 lcd.print(humidity);
 lcd.print(" %   ");
 delay(pauseTid);
 
 
 Serial.print("Lufttemperatur (grader Celcius): "); Serial.println(celsius);
 lcd.setCursor (0,0);
 lcd.setBacklight(HIGH);
 lcd.print("Lufttemperatur:");
 lcd.setCursor (0,1);
 lcd.print(celsius);
 lcd.print(" oC    ");
 delay(pauseTid);

 
//LYS
// read the value from the sensor:
  lys = analogRead(sensorPin);
  lys = 100*lys/1023;
  Serial.print("Lysintensitet: ");
  Serial.println(lys);
 lcd.setCursor (0,0);
 lcd.setBacklight(HIGH);
 lcd.print("Lysintensitet:   ");
 lcd.setCursor (0,1);
 lcd.print(lys);
 lcd.print(" %    ");
 delay(pauseTid);
 lcd.setBacklight(LOW); 

 Serial.println(n);
  //SD
  if (n == 60) {  // ca. 12 sekunder pr. n. n=60 er ca. 12 minutter => 5*24=130 målinger pr. døgn
dataString = String("JF:") + String(Fugt) + String(" JT:") + String(sensors.getTempCByIndex(0)) + String(" LT:") + String(celsius) + String(" LF:") + String(humidity) + String(" LI:") + String(lys);
mySensorData = SD.open("Kdata.txt", FILE_WRITE); // åbn filen på SDkortet
if (mySensorData) {
Serial.println();
Serial.println(dataString);
Serial.println();
mySensorData.println(dataString); //skriv fugtighed til SD-kortet
mySensorData.close(); //luk filen på SD-kortet
}
n=0;
  }
}
