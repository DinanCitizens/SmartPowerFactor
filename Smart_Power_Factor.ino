
#include <ThingSpeak.h> //library thingspeak
#include <WiFiEsp.h> //library WiFi ESP-01
#include <PZEM004Tv30.h> //Library PZEM-004T-100A v3.0
#include <Wire.h> //Library Wire
#include <LiquidCrystal_I2C.h> //Library LCD
#include "secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;

WiFiEspClient client;
PZEM004Tv30 pzem(11, 12); //Rx Tx PZEM
LiquidCrystal_I2C lcd(0x27, 20, 4);

#ifndef HAVE_HWSERIAL1
#define ESP_BAUDRATE  19200
#else
#define ESP_BAUDRATE  115200
#endif

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

void setup() {
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  while(!Serial){
    ;
  }
  setEspBaudRate(ESP_BAUDRATE);
  while (!Serial){
    ;
  }
  Serial.print("Mencari ESP8266...");
  WiFi.init(&Serial1);
  if (WiFi.status() == WL_NO_SHIELD){
    Serial.println("WiFi tidak ditemukan");
    while (true);
  }
  Serial.println("WiFi ditemukan");
  ThingSpeak.begin(client);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Menghubungkan ke SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);
      Serial.println("Berhasil Terhubung");
      Serial.println("==================================");
      delay(3000);
    }
  }
  float voltage = pzem.voltage();
  if(voltage != NAN){
  Serial.print("Tegangan: "); Serial.print(voltage); Serial.println("V");
  lcd.setCursor(0, 0);
  lcd.print("Tegangan: "); lcd.print(voltage); 
  } else {
  Serial.println("Error reading voltage");
  lcd.println("Error reading voltage");
  }
  float current = pzem.current();
  if(current != NAN){
  Serial.print("Arus: "); Serial.print(current); Serial.println("A");
  lcd.setCursor(0, 1);
  lcd.print("Arus    : "); lcd.print(current); 
  } else {
  Serial.println("Error reading current");
  lcd.println("Error reading current");
  }
  float power = pzem.power();
  if(current != NAN){
  Serial.print("Daya: "); Serial.print(power); Serial.println("W");
  lcd.setCursor(0, 2);
  lcd.print("Daya    : "); lcd.print(power); 
  } else {
  Serial.println("Error reading power");
  lcd.println("Error reading power");
  }
//  float energy = pzem.energy();
//  if(current != NAN){
//  Serial.print("Energy: "); Serial.print(energy,3); Serial.println("kWh");
//  } else {
//  Serial.println("Error reading energy");
//  }
//  float frequency = pzem.frequency();
//  if(current != NAN){
//  Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
//  } else {
//  Serial.println("Error reading frequency");
//  }
  float pf = pzem.pf();
  if(current != NAN){
  Serial.print("PF: "); Serial.println(pf);
  lcd.setCursor(0, 3);
  lcd.print("PF      : "); lcd.print(pf);
  } else {
  Serial.println("Error reading power factor");
  lcd.println("Error reading power factor");
  }
  Serial.println();
  delay(2000);
  
  ThingSpeak.setField(1,(float)voltage);
  ThingSpeak.setField(2,(float)current);
  ThingSpeak.setField(3,(float)power);
//  ThingSpeak.setField(4,(float)energy);
//  ThingSpeak.setField(5,(float)frequency);
  ThingSpeak.setField(6,(float)pf);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Memperbarui Channel Berhasil");
  }else{
    Serial.println("Terjadi Masalah teknis." +String(x));
  }
  delay(3000);
}

void setEspBaudRate(unsigned long baudrate){
  long rates[6] = {115200,74880,57600,38400,19200,9600};

  Serial.print("Setting ESP8266 baudrate to ");
  Serial.print(baudrate);
  Serial.println("...");

  for(int i = 0; i < 6; i++){
    Serial1.begin(rates[i]);
    delay(100);
    Serial1.print("AT+UART_DEF=");
    Serial1.print(baudrate);
    Serial1.print(",8,1,0,0\r\n");
    delay(100);  
  }
    
  Serial1.begin(baudrate);
}
