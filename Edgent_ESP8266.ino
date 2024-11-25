

#define BLYNK_TEMPLATE_ID "TMPL3XOGTtikF"
#define BLYNK_TEMPLATE_NAME "safety system"
#define BLYNK_AUTH_TOKEN "PZ4TXa9am36ykiEd5a6t_C8xEbSCR--U"

char auth[] = BLYNK_AUTH_TOKEN;   //Blynk Authentication Token
char ssid[] = "IOT";          // WiFi SSID
char pass[] = "12345678";  // WiFi Password

#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define GAS A0

static const int RXPin = 4, TXPin = 5;  // GPIO 4=D2(conneect Tx of GPS) and GPIO 5=D1(Connect Rx of GPS
static const uint32_t GPSBaud = 9600;   //if Baud rate 9600 didn't work in your case then use 4800

TinyGPSPlus gps;  // The TinyGPS++ object

SoftwareSerial mygps(D1, D2);  // The serial connection to the GPS device

BlynkTimer timer;

float latitude;   //Storing the Latitude
float longitude;  //Storing the Longitude
float velocity;   //Variable  to store the velocity
float sats;       //Variable to store no. of satellites response
String bearing;   //Variable to store orientation or direction of GPS



//unsigned int move_index;         // moving index, to be used later
unsigned int move_index = 1;  // fixed location for now
int flag = 0, flag1 = 0;
int i = 0;
void setup() {
  Serial.begin(115200);
  // pinMode(D0, INPUT);
  pinMode(D0, INPUT_PULLUP);
  Serial.println();
  mygps.begin(GPSBaud);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, checkGPS);  // every 5s check if GPS is connected, only really needs to be done once
}

void checkGPS() {
  if (gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    //Blynk.virtualWrite(V3, "GPS ERROR");  // Value Display widget  on V3 if GPS not detected
  }
}

void loop() {

  while (mygps.available() > 0) {
    // sketch displays information every time a new sentence is correctly encoded.
    if (gps.encode(mygps.read()))
      displayInfo();
  }
  Blynk.run();
  timer.run();
}

void displayInfo() {
  if (gps.location.isValid()) {

    sats = gps.satellites.value();    //get number of satellites
    latitude = (gps.location.lat());  //Storing the Lat. and Lon.
    longitude = (gps.location.lng());
    velocity = gps.speed.kmph();                          //get velocity
    bearing = TinyGPSPlus::cardinal(gps.course.value());  // get the direction

    String latitudeStr = String(latitude, 6);
    String longitudeStr = String(longitude, 6);

   // String latitudeStr = convertToDegreesMinutes(latitude);
   // String longitudeStr = convertToDegreesMinutes(longitude);

    Serial.print("SATS:  ");
    Serial.println(sats);  // float to x decimal places
    Serial.print("LATITUDE:  ");
    Serial.println(latitude, 6);  // float to x decimal places
    Serial.print("LONGITUDE: ");
    Serial.println(longitude, 6);
    Serial.print("SPEED: ");
    Serial.print(velocity);
    Serial.println("kmph");
    Serial.print("DIRECTION: ");
    Serial.println(bearing);
    Blynk.virtualWrite(V0, latitudeStr);
    Blynk.virtualWrite(V1, longitudeStr);
    Blynk.virtualWrite(V2, velocity);
    //  Blynk.virtualWrite(V6, String(longitude, 6), String(latitude, 6));
    //int isButtonPressed = digitalRead(D0);
    if (digitalRead(D0) == LOW) {
      Blynk.logEvent("switch", "Need urgent Help");
      Blynk.virtualWrite(V3, 1);
      delay(5000);
    } 
    else{
      Blynk.virtualWrite(V3, 0);
    }
    delay(500);
  }
  //Serial.println();
}
String convertToDegreesMinutes(float decimalDegree) {
  // Get the degrees (integer part)
  int degrees = int(decimalDegree);

  // Get the minutes (fractional part, converted to minutes)
  float minutes = (decimalDegree - degrees) * 60.0;

  // Handle negative values (for S and W directions)
  if (degrees < 0) {
    degrees = -degrees;
    minutes = -minutes;
  }

  // Format the string as "DDD° MM.MMM'"
  String result = String(degrees) + "° " + String(minutes, 3) + "'";

  return result;
}