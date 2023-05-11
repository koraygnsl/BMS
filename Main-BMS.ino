// Kullanacağımız kütüphaneleri ekliyoruz

#include <SoftwareSerial.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include "Nextion.h"
#include <SD.h>
#include <SPI.h>
#include <virtuabotixRTC.h> 
#include "LoRa_E22.h"


// SD Kart pin ataması yapıyoruz
File sdcard_file;

int kayitNo = 0;

int CS_pin = 53; 

// Dengeleme devresi için pin ataması yapıyoruz
int pinler[20] = {22, 23, 24, 25, 26, 27, 28, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 3, 4};


int CLK = 36;                       // DS1302'nin CLK pini
int DAT = 38;                       // DS1302'nin DAT pini
int RST = 40;                       // DS1302'nin RST pini
 
virtuabotixRTC RTC(CLK, DAT, RST);


// LORA ayarlarını yapıyoruz

#define DESTINATION 63
LoRa_E22 E22(&Serial1, 30, 32, 34); 

String myStringvoltMax;
String myStringvoltMin;

String myStringpilYuzde;
String myStringvoltSum;
String myStringsantigrat;
String myStringcurrent;
 
struct veriler {
  char voltMin[5] = "";
  char voltMax[5] = "";
  char voltSum[5] = "";
  char pilYuzde[5] = "";
  char santigrat[5] = "";
  char current[5] = "";
} data;


NexText sicaklikX = NexText(0, 2, "sicaklik");
NexText maxVoltX = NexText(0, 5, "maxVolt");
NexText minVoltX = NexText(0, 6, "minVolt");
NexText pilYuzdeX = NexText(0, 3, "pilYuzde");
NexText toplamVX = NexText(0, 7, "toplamV");
NexText hizX = NexText(0, 8, "hiz");
NexText akimX = NexText(0, 4, "akim");


// Sıcaklık sensörü ayarlarını yapıyoruz

SoftwareSerial mySerial(10, 11);
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


DeviceAddress sensorsArray[] = {{0x28, 0xB0, 0xE6, 0x03, 0x00, 0x00, 0x80, 0x5B},
                                {0x28, 0x82, 0x32, 0x96, 0xF0, 0x01, 0x3C, 0x21},
                                {0x28, 0xD9, 0x4C, 0x96, 0xF0, 0x01, 0x3C, 0x38}};

int sensorSayisi = sizeof(sensorsArray) / sizeof(sensorsArray[0]);


// Değişken tanımlamaları yapıyoruz

float voltAll[20];
float volt[10];
float voltMin;
float voltMax;
float voltSum;
float pilYuzde;
float akim = 999.9;
float sicaklik1 = 0;
float sicaklik2 = 0;
float sicaklik3 = 0;
float ortsicaklik = 0;
float sensitivity = 0.066;
int maxIndex, minIndex;
float hiz = 0;

char akimArray[14];
char sicaklikArray[14];
char minVoltArray[14];
char maxVoltArray[14];
char toplamVArray[14];
char pilYuzdeArray[14];
char hizArray[14];


String sicaklikStr;
String minVoltStr;
String maxVoltStr;
String toplamVStr;
String akimStr;
String pilYuzdeStr;
String hizStr;


void setup() {

  // Sensör ve diğer ayarları yapıyoruz

  E22.begin();

  Serial.begin(115200);
  Serial3.begin(9600);
  mySerial.begin(9600);
  pinMode(4,OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  
  nexInit(); 
  
  sensors.begin();
  for (int i = 0; i < sensorSayisi; i++) {
    sensors.setResolution(sensorsArray[i], 9);
  }

    struct veriler {
      char voltMin[5] = "";
      char voltMax[5] = "";
      char voltSum[5] = "";
      char pilYuzde[5] = "";
      char santigrat[5] = "";
      char current[5] = "";
    }data;

    int saniye = 0;
    int yil = 2022;
    int ay = 1;
    int gun = 3;
    int haftanin_gunu = 1;
    int saat = 0;
    int dakika = 0;
    RTC.setDS1302Time(saniye, dakika, saat, haftanin_gunu, gun, ay, yil);
    pinMode(CS_pin, OUTPUT); 

      if (SD.begin())
        {
          Serial.println("SD card is ready to use.");
        } else
        {
          Serial.println("SD card initialization failed");
          return;
        }

           
  String fileName = "data";

  fileName.concat(String(kayitNo) + ".txt");
  for( int index = 0; index < 1000; index++){

    fileName = "data";
    kayitNo = index;
    fileName.concat(String(kayitNo) + ".txt");
    sdcard_file = SD.open(fileName, FILE_READ);
    if (sdcard_file) {
          sdcard_file.close();
      
      }else {
          Serial.println("Error - dosya açılamadı");
          break;
      }
  }

  //dosya kaydı için kayıtNo kullanılacak

  fileName = "data";
  fileName.concat(String(kayitNo) + ".txt");
    sdcard_file = SD.open(fileName, FILE_WRITE);
    if (sdcard_file) { 

      sdcard_file.print("saat");   
      sdcard_file.print(";");
      sdcard_file.print("dakika");   
      sdcard_file.print(";");
      sdcard_file.print("saniye");   
      sdcard_file.print(";");
      sdcard_file.print("hiz_kmh");
      sdcard_file.print(";");
      sdcard_file.print("min_volt");
      sdcard_file.print(";");
      sdcard_file.print("max_volt");
      sdcard_file.print(";");
      sdcard_file.print("sum_volt");
      sdcard_file.print(";");
      sdcard_file.print("pil_yuzde");
      sdcard_file.print(";");
      sdcard_file.print("santigrat");
      sdcard_file.print(";");
      sdcard_file.print("current");
      sdcard_file.print(";");      
      sdcard_file.close();
    }

    else {
      Serial.println("error opening data.txt");
    }
}

void loop() {
   // SD
   RTC.updateTime(); 
  Serial.println(RTC.hours); 
  Serial.println(RTC.minutes); 
  Serial.println(RTC.seconds); 
  String fileName = "data";
  fileName = "data";
  fileName.concat(String(kayitNo) + ".txt");
  sdcard_file = SD.open(fileName, FILE_WRITE);

  Serial.println(fileName);
  if (sdcard_file) {    

    sdcard_file.println(" ");
    sdcard_file.print(RTC.hours);
    sdcard_file.print(";");
    sdcard_file.print(RTC.minutes);
    sdcard_file.print(";");
    sdcard_file.print(RTC.seconds);
    sdcard_file.print(";");
    sdcard_file.print("35");
    sdcard_file.print(";");
    sdcard_file.print(voltMin);
    sdcard_file.print(";");
    sdcard_file.print(voltMax);
    sdcard_file.print(";");
    sdcard_file.print(voltSum);
    sdcard_file.print(";");
    sdcard_file.print(pilYuzde);
    sdcard_file.print(";");
    sdcard_file.print(ortsicaklik);
    sdcard_file.print(";");
    sdcard_file.print(akim);
    sdcard_file.close();
  }
  
  else{
    Serial.println("error opening test.txt");
  }

  //Batarya ölçümlerini alıyoruz. Bu ölçümleri alırken kod okunurluğu ve kolaylık açısından fonskiyonları kullanıyoruz.

  Serial.println("*");
  akim = akimOlc(A2, sensitivity);
  voltajOlc(volt, voltAll);
  ortalamaSicaklik();
  mesajGonder();
   

  // Bataryanın minimum ve maksimum değelerini görebilmek için index atıyoruz

  maxIndex = bataryaDetay(voltAll, true);
  minIndex = bataryaDetay(voltAll, false);

  

  Serial.print("Batarya paketinin toplam geriimi : ");
  Serial.print(voltSum);

  Serial.print("Batarya yüzdesi : ");
  Serial.print(pilYuzde);

  Serial.print("Batarya paketi sıcaklığı 1. sensör : ");
  Serial.print(sicaklik1);
  Serial.println(" C");

  Serial.print("Batarya paketi sıcaklığı 2. sensör : ");
  Serial.print(sicaklik2);
  Serial.println(" C");

  Serial.print("Batarya paketi sıcaklığı 3. sensör : ");
  Serial.print(sicaklik3);
  Serial.println(" C");
  
  Serial.print(ortsicaklik);
  Serial.println(" C");


  bataryaDengeleme(voltAll, voltMin);

}

void mesajGonder(){

  Serial3.print(String(ortsicaklik));
  Serial3.print("/");
  Serial3.print(voltMax);
  Serial3.print("/");
  Serial3.print(voltMin);
  Serial3.print("/");
  Serial3.print(voltSum);
  Serial3.print("/");
  Serial3.print(pilYuzde);
  Serial3.print("/");
  Serial3.println(hiz);


  sicaklikStr = String(ortsicaklik);
  sicaklikStr.toCharArray(sicaklikArray, 14);

  maxVoltStr = String(voltMax);
  maxVoltStr.concat(" H: " + String(maxIndex));
  maxVoltStr.toCharArray(maxVoltArray, 14);

  minVoltStr = String(voltMin);
  minVoltStr.concat(" H: " + String(minIndex));
  minVoltStr.toCharArray(minVoltArray, 14);

  toplamVStr = String(voltSum);
  toplamVStr.toCharArray(toplamVArray, 14);

  pilYuzdeStr = String(pilYuzde);
  pilYuzdeStr.toCharArray(pilYuzdeArray, 14);

  hizStr = String(hiz);
  hizStr.toCharArray(hizArray, 14);

  akimStr = String(akim);
  akimStr.toCharArray(akimArray, 14);

  sicaklikX.setText(sicaklikArray);
  maxVoltX.setText(maxVoltArray);
  minVoltX.setText(minVoltArray);
  toplamVX.setText(toplamVArray);
  pilYuzdeX.setText(pilYuzdeArray);
  hizX.setText(hizArray);
  akimX.setText(akimArray);

  // lora
  myStringvoltMin= String(voltMin);
  myStringvoltMin.toCharArray(data.voltMin, 5);

  myStringvoltMax= String(voltMax);
  myStringvoltMax.toCharArray(data.voltMax, 5);
  
  myStringvoltSum= String(voltSum);
  myStringvoltSum.toCharArray(data.voltSum, 5);
  
  myStringpilYuzde= String(pilYuzde);
  myStringpilYuzde.toCharArray(data.pilYuzde, 5);

  myStringsantigrat= String(ortsicaklik);
  myStringsantigrat.toCharArray(data.santigrat, 5);

  myStringcurrent= String(akim);
  myStringcurrent.toCharArray(data.current, 5);

  ResponseStatus rs = E22.sendFixedMessage(0, DESTINATION, 23, &data, sizeof(veriler));
  Serial.println(rs.getResponseDescription());
  
}



float akimOlc(int sensorPin, float sensitivity) {
  int sensorValue = analogRead(sensorPin);
  float akim = (sensorValue - 512) * sensitivity;
  akim = abs(akim);
  return akim;
}

void voltajOlc(float* volt, float* voltAll) {

  // İlk aşama olarak 1.arduinodan gelen verileri okuyor ve kayıt ediyoruz

  String data = Serial3.readStringUntil('\n');
  int commaIndex = data.indexOf("/");
  if (commaIndex > 0) {
    volt[0] = data.substring(0, commaIndex).toFloat();
    volt[1] = data.substring(commaIndex + 1).toFloat();
    volt[2] = data.substring(commaIndex + 1).toFloat();
    volt[3] = data.substring(commaIndex + 1).toFloat();
    volt[4] = data.substring(commaIndex + 1).toFloat();
    volt[5] = data.substring(commaIndex + 1).toFloat();
    volt[6] = data.substring(commaIndex + 1).toFloat();
    volt[7] = data.substring(commaIndex + 1).toFloat();
    volt[8] = data.substring(commaIndex + 1).toFloat();
    volt[9] = data.substring(commaIndex + 1).toFloat();
  }

  // Ana Arduino'daki hücreleri ölçüyor ve atamaları birleştiriyoruz

  voltAll[0] = volt[0];
  voltAll[1] = volt[1];
  voltAll[2] = volt[2];
  voltAll[3] = volt[3];
  voltAll[4] = volt[4];
  voltAll[5] = volt[5];
  voltAll[6] = volt[6];
  voltAll[7] = volt[7];
  voltAll[8] = volt[8];
  voltAll[9] = volt[9];
  voltAll[10] = analogRead(A4)*(5.0/1023.0);
  voltAll[11] = analogRead(A4)*(5.0/1023.0);
  voltAll[12] = analogRead(A4)*(5.0/1023.0);
  voltAll[13] = analogRead(A4)*(5.0/1023.0);
  voltAll[14] = analogRead(A4)*(5.0/1023.0);
  voltAll[15] = analogRead(A4)*(5.0/1023.0);
  voltAll[16] = analogRead(A4)*(5.0/1023.0);
  voltAll[17] = analogRead(A4)*(5.0/1023.0);
  voltAll[18] = analogRead(A4)*(5.0/1023.0);
  voltAll[19] = analogRead(A4)*(5.0/1023.0);

}


int bataryaDetay(float arr[], bool isMax) {
  int index = -1;
  float temp;

  // Bubble Sort algoritması kullanarak minimum, maksimum voltajı ve kaçıncı hücre olduğunu buluyoruz.

  for (int i=0; i<20; i++) {
    for (int j=0; j<19-i; j++) {
      if (arr[j]>arr[j+1]) {
        temp = arr[j];
        arr[j] = arr[j+1];
        arr[j+1] = temp;
      }
    }
  }

  ortalamaSicaklik();

  if (isMax) {
    voltMax = arr[19];
    
    for(int m=0; m < 20; m++){
        if(arr[m] == voltMax)
          index = m+1;
    }

  } else {
    voltMin = arr[0];
    
    for(int m=0; m < 20; m++){
      if(arr[m] == voltMin)
         index = m+1;
    }
  }

  // Hücre voltajlarının toplamlarını ve doluluk oranını hesaplıyoruz

  for (int i=0; i<20; i++){
    voltSum += voltAll[i];
  }
  ortalamaSicaklik();
  
  pilYuzde = (voltSum-50)/(80-50)*100;
  return index;
}

void ortalamaSicaklik() {
  float toplam = 0;

  // İlk önce sıcaklık sensörlerinden gelen veriler okuyoruz

  for (int i = 0; i < sensorSayisi; i++) {
    sensors.requestTemperaturesByAddress(sensorsArray[i]);

    // Eğer sıcaklık 35 C'den fazlaysa batarya havalandırması çalışıyor.
    if(sensors.getTempCByIndex(i) > 35)
      digitalWrite(5,HIGH);

    // Eğer sıcaklık 55 C'den fazlaysa flaşör çalışıyor.
    if(sensors.getTempCByIndex(i) > 55)
      digitalWrite(6,HIGH);

    // Eğer sıcaklık 70 C'den fazlaysa araç kendini kapatıyor.
    if(sensors.getTempCByIndex(i) > 70)
      digitalWrite(7,HIGH);

    // Alınan sıcaklık verilerine göre ortalama batarya sıcaklığı hesaplanıyor
    toplam += sensors.getTempCByIndex(i);
  }
  ortsicaklik = toplam / sensorSayisi;

  sicaklikStr = String(ortsicaklik);
  sicaklikStr.toCharArray(sicaklikArray, 14);
  sicaklikX.setText(sicaklikArray);
}

void bataryaDengeleme(float* voltAll, float minV) {

  // İlk önce bir denge voltajı belirliyoruz.
  float balanceVoltage = minV + 0.1;

  // Bir 'for' döngüsü kullanarak tüm hücreleri tek tek kontrol ediyor ve eğer denge voltajından fazlaysa dengeleme devresini tetikliyoruz.
  for (int i = 0; i < 20; i++) {
    if(voltAll[i] > balanceVoltage) {
        digitalWrite(pinler[i], HIGH);
        // Veri kaybı yaşanmaması için her hücre dengelemesinde verileri tekrar kontrol edip ekrana gönderiyoruz.
        veri_kontrol();
        // 1 saniye boyunca dengeleme devresini tetikliyor ardından durduruyoruz.
        delay(1000);
        digitalWrite(pinler[i], LOW);
    }
  }
}

void veri_kontrol() {
  akim = akimOlc(A2, sensitivity);
  voltajOlc(volt, voltAll);
  ortalamaSicaklik();
  mesajGonder();
}