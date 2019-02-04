#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

//IR RECEIVER PART
#include <IRremote.h>
int RECEIVE_PIN = 8;
IRrecv irrecv(RECEIVE_PIN);
decode_results results;

#define GRNLED 43

void espSend(String command) {
//  Serial.println("espSent: "+command+" //Waiting for RESPONSE");
  Serial2.println(command);
//  Serial.println("Response from ESP: "+Serial2.readString());
  Serial2.find("OK");//OPTION
//  if (!Serial2.find("OK")) {
//      Serial.println("Error: "+ command);
//    }

}

void espDataSend(String data) {
//  Serial.print(data);
  Serial2.print("AT+CIPSEND=");
  Serial2.println(data.length());
//  Serial.print(Serial2.readString());
  Serial2.find(">");
  Serial2.print(data);
}


void setup() {
//  Serial.begin(115200);

  pinMode(GRNLED,OUTPUT);
  digitalWrite(GRNLED,HIGH);
  Serial2.begin(115200);
  
  Mirf.cePin = 44;
  Mirf.csnPin = 46;
  
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  
  byte radr[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
  Mirf.ceLow();
  Mirf.writeRegister(RX_ADDR_P0,radr,mirf_ADDR_LEN);
  Mirf.ceHi();
  Mirf.payload = 2;
  pinMode(53, OUTPUT);

  Mirf.channel = 0; 
  Mirf.config();
//  Serial.println("Beginning NRLF... "); 
  Mirf.powerUpRx();
  Mirf.flushRx();
  Serial2.setTimeout(5000);
//WIFI parameters  
  espSend("AT+RST");
  Serial2.println("AT+CIPSTATUS");
  Serial2.readString();//INSTEAD OF NEXT COMMAND
//  Serial.println(Serial2.readString());
  delay(5000);
  espSend("AT");
  delay(5000);
  espSend("AT+CIPSTART=\"TCP\",\"192.168.0.171\",9090");//0.171 must be
  delay(5000);
  Serial2.setTimeout(500);
  
  irrecv.enableIRIn();
  
  digitalWrite(GRNLED,LOW);
}

/*
on  NRF Interrupt espSend

on IR before espSend -> noInterrupts?
after interrupts; 

IR command to restart esp;


*/



void loop() {
  byte data[2];
  while(!Mirf.dataReady()){
        
        if (irrecv.decode(&results)) {
        String irData = String("0x")+String(results.value, HEX);
        espDataSend(irData);
        irrecv.resume();// Receive the next value
    }
    delay(1);//10
  }
  digitalWrite(GRNLED, HIGH);
  while(Mirf.dataReady()) {
  
  Mirf.getData(data);
  int voltage = data[1] * 24; 
  String mData;
  mData = String(data[0]) + "; " + String(voltage);
//  Serial.println("Data from NRLF: "+mData);//
  espDataSend(mData);
  }
//  delay(1);//10
  digitalWrite(GRNLED, LOW);

}
