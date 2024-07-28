//คำสั่งยังไม่สามารถทำงานได้เนื่องจาก ยังไม่มีคำสั่งและอุปกรณ์ที่ใช้วัดค่าในดิน แก้คำสั่งวัดค่าดิน และค่าความชื้น และแก้คำสั่งแสดงผลข้อความ
#define IO_USERNAME  "Hanami8"
#define IO_KEY       "aio_bIIv83emkurxvCwKML0dZ9TUMb9m"
#define LINE_TOKEN  "fjOLzC1dSr2CjC6WQgKFHu4fubAd7MmTJvyHOq5RPQb"
#define WIFI_SSID "Phombud_2.4G"
#define WIFI_PASS "0899589431"
#include "AdafruitIO_WiFi.h"
#include <Ticker.h>
#if defined(USE_AIRLIFT) || defined(ADAFRUIT_METRO_M4_AIRLIFT_LITE) || \
    defined(ADAFRUIT_PYPORTAL)
#if !defined(SPIWIFI_SS)
#define SPIWIFI SPI
#define SPIWIFI_SS 10
#define NINA_ACK 9 
#define NINA_RESETN 6
#define NINA_GPIO0 -1 
#endif
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS, SPIWIFI_SS,
                   NINA_ACK, NINA_RESETN, NINA_GPIO0, &SPIWIFI);
#else
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
#endif

#include <Adafruit_Sensor.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Ticker.h>
#include <TridentTD_LineNotify.h>
#define Buzzer 11

#define DATA_PIN 13

pinMode (34,OUTPUT);
pinMode(Buzzer,OUTPUT); 
DHT_Unified dht(DATA_PIN, DHT11);

AdafruitIO_Feed *temperature = io.feed("Temperature");
AdafruitIO_Feed *humidity = io.feed("Humidity");
AdafruitIO_Feed *LineText = io.feed("LineText");
AdafruitIO_Feed *PH = io.feed("PH");
AdafruitIO_Feed *Potassium = io.feed("Potassium");
AdafruitIO_Feed *Phosphorus = io.feed("Phosphorus");
AdafruitIO_Feed *Nitrogen = io.feed("Nitrogen");
AdafruitIO_Feed *sensorValue = io.feed("sensorValue");

void setup() {

  Serial.begin(115200);

  while(! Serial);

  dht.begin();

  Serial.print("Connecting to Adafruit IO");
  io.connect();

  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());

  LINE.setToken(LINE_TOKEN);


}

void loop() {

  io.run();

  sensors_event_t event;
  dht.temperature().getEvent(&event);

  float celsius = event.temperature;
  float fahrenheit = (celsius * 1.8) + 32;
  float sensorValue = analogRead(A0)
  int Fertility,PH;
  float PH_F;
  

  Serial.print("celsius: ");
  Serial.print(celsius);
  Serial.println("C");

  Serial.print("fahrenheit: ");
  Serial.print(fahrenheit);
  Serial.println("F");

  temperature->save(celsius);

  dht.humidity().getEvent(&event);

  Serial.print("humidity: ");
  Serial.print(event.relative_humidity);
  Serial.println("%");

  humidity->save(event.relative_humidity);
    
  Serial.print("humidityValue: ");
  Serial.print(sensorValue);
  Serial.println("%");

  sensorValue->save(sensorValue);

  Fertility = Read_Fertility();
  PH_F = Read_PH();
  PH = PH_F * 10;
  PH->save(PH);
  
  
  Fertility = Read_Fertility(); 
  Serial.print("Fertility" + Fertility);

  if(Fertility <= 20){Nitrogen    = (Fertility * 10)/4;// 0-50
                      Serial.print(Nitrogen );

                      Phosphorus  =  (Fertility / 5);     // 0-4
                      Serial.print(Phosphorus);

                      Potassium   = (Fertility * 10)/4; 
                      Serial.print(Potassium);
                     }else
  if(Fertility <= 70){Nitrogen    = ((Fertility-20)*3)+50; // 50-200
                      Serial.print(Nitrogen );

                      Phosphorus  = ((Fertility-20)/5)+4;  // 5-14
                      Serial.print(Phosphorus); 

                      Potassium   = ((Fertility-20)*3)+50;
                      Serial.print(Potassium);
                     }else
  if(Fertility <= 100){Nitrogen   = ((Fertility-70)*10)+200; // 200-300
                      Serial.print(Nitrogen );

                      Phosphorus  = ((Fertility-70)/5)+14;   // 14-20
                      Serial.print(Phosphorus);
                      
                      Potassium   = ((Fertility-70)*10)+200;
                      Serial.print(Potassium);
                     }
  Nitrogen->save(Nitrogen);
  Phosphorus->save(Phosphorus);
  Potassium->save(Potassium);
  

  delay(30000);

  yield();
  LINE.notify(" อุณหภูมิ : " + String(celsius) + " °C  " + "  ความชื้น : " + String(event.relative_humidity) + " %");
  Serial.print(" อุณหภูมิ : " + String(celsius) + " °C  " + "  ความชื้น : " + String(event.relative_humidity) + " %");
  
  if (celsius > 40) {
    String LineText;
    String string1 = "ค่าอุณหภูมิไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
    
  }
  if (celsius < 15) {
    String LineText;
    String string1 = "ค่าอุณหภูมิไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (15 <= celsius <= 40) {
    String LineText;
    String string1 = "ค่าอุณหภูมิเหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (PH_F > 7.5) {
    String LineText;
    String string1 = "ค่าPH ในดินไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
    
  }
  if (PH_F < 6) {
    String LineText;
    String string1 = "ค่า PH ในดินไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (6 <= PH_F <= 7.5) {
    String LineText;
    String string1 = "ค่าPH ในดินเหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (์Nitrogen > 200) {
    String LineText;
    String string1 = "ค่าไนโตรเจนในดินไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
    
  }
  if (์Nitrogen < 50) {
    String LineText;
    String string1 = "ค่าไนโตรเจนในดินไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (50 <= Nitrogen <= 200) {
    String LineText;
    String string1 = "ค่าไนโตรเจนในดินเหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (Phosphorus > 40) {
    String LineText;
    String string1 = "ค่าฟอสฟอรัสในดินไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
    
  }
  if (Phosphorus < 15) {
    String LineText;
    String string1 = "ค่าฟอสฟอรัสในดินไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (15 <= Phosphorus <= 40) {
    String LineText;
    String string1 = "ค่าฟอสฟอรัสในดินเหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (Potassium > 40) {
    String LineText;
    String string1 = "ค่าโพแทสเซียมในดินไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
    }
  if (Potassium < 15) {
    String LineText;
    String string1 = "ค่าโพแทสเซียมในดินไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (15 <= Potassium <= 40) {
    String LineText;
    String string1 = "ค่าโพแทสเซียมในดินเหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (sensorValue < 15){
    String LineText;
    String string1 = "ค่าความชื้นในดินต่ำเกินไปไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (sensorValue > 15){
    String LineText;
    String string1 = "ค่าความชื้นในดินสูงเกินไปไม่เหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
  if (15 <= sensorValue <= 40){
    String LineText;
    String string1 = "ค่าความชื้นในดินเหมาะสมกับการเพาะปลูกพืช ";
    LineText = string1 ;
    Serial.print("Line ");
    Serial.println(LineText);
    LINE.notify(LineText);
  }
}
float Read_PH()
{int i;
  float PH;
  PH = 0;
  for(i=0;i<10;i++){PH  = PH + analogRead(0);delay(10); } 
  PH       = PH/10;
  if(PH >= 480){PH = (10-(PH - 480)/14);}else
  if(PH >= 360){PH = (20-(PH - 360)/12);}else
  if(PH >= 275){PH = (30-(PH - 275)/8.5);}else
  if(PH >= 200){PH = (40-(PH - 200)/7.5);}else
  if(PH >= 125){PH = (50-(PH - 125)/8.5);}else
  if(PH >= 65 ){PH = (60-(PH - 60)/6.5);}else
  if(PH >=  0 ){PH = (70-(PH - 0)/6.0);}
  return(PH/10);
}
int Read_Fertility()
{int i,Fertility;
  Fertility = 0; 
  for(i=0;i<10;i++){Fertility = Fertility + analogRead(0);delay(1);}
  Fertility = Fertility/10;
  if(Fertility >= 480){Fertility = ((Fertility - 480)/10) + 93;}else
  if(Fertility >= 360){Fertility = ((Fertility - 360)/7.5) + 77;}else
  if(Fertility >= 275){Fertility = ((Fertility - 275)/5) + 59;}else
  if(Fertility >= 200){Fertility = ((Fertility - 200)/6.25) + 47;}else
  if(Fertility >= 125){Fertility = ((Fertility - 125)/5.3) + 31;}else
  if(Fertility >= 65){Fertility = ((Fertility - 65)/4) + 16;}else
  if(Fertility >=  0){Fertility = ((Fertility - 0)/3.75) + 0;}
  return(Fertility);
  

}
