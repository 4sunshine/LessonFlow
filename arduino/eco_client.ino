#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <IRremote.h>
#include "display.h"

const int RECEIVE_PIN = 8;	// ir receiver pin
const int INT_PIN = 22; 	// nrf irq pin
const int GRNLED = 43;		// green led pin
const int RST_SIG = -1;		// ir code to reset esp

IRrecv irrecv(RECEIVE_PIN);
decode_results results;
display disp;

void setup() {
  pinMode(GRNLED,OUTPUT);
  pinMode(53, OUTPUT);
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

  Mirf.channel = 0; 
  Mirf.config();
  Mirf.powerUpRx();

  Mirf.flushRx();
  Serial2.setTimeout(5000);

  espReset();  

  irrecv.enableIRIn();
  
  digitalWrite(GRNLED,LOW);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), btn_pressed_isr, LOW); 
}

void loop() {
	if (irrecv.decode(&results)) {
		if(results.value==RST_SIG){
			noInterrupts();
			espReset();
			interrupts();
		}
		String irData = String("0x")+String(results.value, HEX);
		noInterrupts();
		espDataSend(irData);
		interrupts()
		irrecv.resume();// Receive the next value
	}

}

void btn_pressed_isr(){
	int data_timer = 100;
	for(;!Mirf.dataReady()&&data_timer;data_timer--);
	if(!data_timer) return;
	byte data[2];
	Mirf.getData(data);
	int voltage = data[1] * 24; 
	String mData;
	mData = String(data[0]) + "; " + String(voltage);
	espDataSend(mData);
}

void espSend(String command){
	Serial2.println(command);
	Serial2.find("OK");
}

void espSendData(String data){
	Serial2.print("AT+CIPSEND=");
	Serial2.println(data.length());
	Serial2.find(">");
	Serial2.print(data);
}

void espReset(){

  espSend("AT+RST");
  delay(5000);
  espSend("AT+CIPSTART=\"TCP\",\"192.168.0.171\",9090");
}
