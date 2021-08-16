
#define BLYNK_PRINT SerialUSB

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Adafruit_CircuitPlayground.h>

char auth[] = "";//Set to your Blynk token
char ssid[] = "";//Set your network name
char pass[] = "";//Set password

#define EspSerial Serial1
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

#define IR_protocol NECX
#define IR_bits 32

//IR code for the air conditioner
#define AC 0xEF20FF // Set your IR
                                                                                                  
float temp = 0;
String param = "";
int delay_set = 600000;//10 minutes
// set 0 - air conditioner is off
// set 1 - air conditioner is on
int set = 1;

void setup(){
  CircuitPlayground.begin();
  SerialUSB.begin(9600);
  Serial.begin(9600);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  Blynk.begin(auth, wifi, ssid, pass);
}

void loop(){
    Blynk.run();
    if(set == 1){
        tempFun();
    }
    knock();
    
    delay(delay_set);
}

BLYNK_WRITE(V2){
      int value = param.asInt();
      Serial.println(value);
      Blynk.virtualWrite(V2, value);
      for(int i =0; i < 10; ++i){
          CircuitPlayground.setPixelColor(i,value*255,0,0);
        }
     CircuitPlayground.strip.show();
  }
  

void tempFun(){
          temp = CircuitPlayground.temperature();
          Blynk.virtualWrite(V0, temp);
          Serial.println(temp);
          for (int i = 0; i < 10; ++i) {
            CircuitPlayground.setPixelColor(i, (temp), 0, 255 -(temp));
            }
         param = (temp <= 23 and temp >= 18) ? "Yes" : "No" ; 
         Blynk.virtualWrite(V1, param);//Temp Sensor (webhook)
         if (param == "Yes"){
              turnOffAC();
              set = 0;
          }
          else{
            set = 1;
          }
}

//turn off air conditioner
void turnOffAC(){
  CircuitPlayground.irReceiver.enableIRIn();
  int p = 0;
  while(p < 100){
      CircuitPlayground.irSend.send(IR_protocol, AC, IR_bits);
      p++;
      }
}

//send alert when someone knock on your home and turn the pixels on with blue colors.
void knock(){
          float noiseVal = CircuitPlayground.mic.soundPressureLevel(10);
          String isKnock = (noiseVal > 15) ? "Yes" : "No" ;
          Blynk.virtualWrite(V3, isKnock);//Temp Sensor (webhook) 
          if(isKnock=="Yes"){
                for(int i =0; i < 10; ++i){
                      CircuitPlayground.setPixelColor(i,0,0,255);
                } 
          CircuitPlayground.strip.show(); 
    }
        
}
