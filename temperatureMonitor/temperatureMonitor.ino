#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define DHTTYPE    DHT11   
#define buzzer 14
#define tempSensor 2
#define led 12 //D6
#define TEMP_THRES 23

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
DHT_Unified dht(tempSensor, DHTTYPE);

#define FIREBASE_HOST "warrior-bulls-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "2JNTy9dXCRiq3Yyf78K9DqOhKZMzwnLeNIgZF5G3"
#define WIFI_SSID "mardes home"
#define WIFI_PASSWORD "Prateek123@@"

void setup() {
  Serial.begin(9600);

  dht.begin();
  timeClient.begin();
  pinMode(tempSensor,INPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(led,OUTPUT);
  
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int n = 0;

void loop() {
  timeClient.update();
  unsigned long startMillis = millis(); // Start of sample window
  
  long num = timeClient.getEpochTime();

  
  sensors_event_t event;

  float temperature=0,humidity = 0;
  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }else {
    
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    temperature = event.temperature;
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    humidity = event.relative_humidity;
    Serial.println(F("%"));
  }

  if(temperature < 20.0){
    digitalWrite(led,HIGH);
    digitalWrite(buzzer,LOW);
    delay(150);
    digitalWrite(buzzer,LOW);

  }else if(temperature > 29.0){
    digitalWrite(led,LOW);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);

  }else{
    digitalWrite(led,LOW);
    digitalWrite(buzzer,LOW);
  }
  
  if(temperature < 20.0 || temperature > 29.0){
    

    char strBuf[100],strBufH[100];
    sprintf(strBuf, "RECORD/%ld/TEMP",num);

    Firebase.setFloat(strBuf,temperature);

    sprintf(strBufH, "RECORD/%ld/HUMID",num);
    
//    char resultFinalS[] = "RECORD/";
//    strcat(resultFinalS, result);
//    strcat(resultFinalS, resultHumid);
    Firebase.setFloat(strBufH,humidity);

    Serial.println("--------------------------------");

  }
  delay(4000);
}
