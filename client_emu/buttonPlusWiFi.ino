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
//28 buttons
const unsigned long threshold = 5000; //5 SECONDS THRESHOLD
unsigned long timeLast[28] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //TIMER FOR EVERY STUDENT
//const String irCodes[17] = {"0xff52ad", "0xff42bd", "0xff4ab5", "0xff5aa5","0xff38c7","0xff10ef","0xff7a85",
//        "0xff18e7","0xff30cf","0xffb04f","0xff9867","0xff6897","0xff02fd","0xffa857","0xff22dd","0xffc23d","0xff629d"};

void espReset(){
  espSend("AT+RST");
  delay(5000);
  espSend("AT+CIPSTART=\"TCP\",\"192.168.0.171\",9090");
}

void espSend(String command) {
  Serial2.println(command);
  Serial2.find("OK");//OPTION
}

void espDataSend(String data) {
  Serial2.print("AT+CIPSEND=");
  Serial2.println(data.length());
  Serial2.find(">");
  Serial2.print(data);
}


void setup() {
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
  Mirf.powerUpRx();
  Mirf.flushRx();
  Serial2.setTimeout(5000);

  espReset();  

  irrecv.enableIRIn();
  
  digitalWrite(GRNLED,LOW);
}

void loop() {
  byte data[2];
  while(!Mirf.dataReady()){
        
        if (irrecv.decode(&results)) {
        String irData = String("0x")+String(results.value, HEX);
        
      //IR REMOTE CONTROL CODES CHECK

        if ((irData == "0xff42bd") || (irData == "0xff4ab5")
        || (irData == "0xff5aa5") || (irData == "0xff38c7") || (irData == "0xff10ef")
        || (irData == "0xff7a85") || (irData == "0xff18e7") || (irData == "0xff30cf")
        || (irData == "0xffb04f") || (irData == "0xff9867") || (irData == "0xff6897")
        || (irData == "0xff02fd") || (irData == "0xffa857") || (irData == "0xff22dd")
        || (irData == "0xffc23d") || (irData == "0xff629d") )
          espDataSend(irData);
      // #SYMBOL => ESP RESET
        else if ( irData == "0xff52ad") {
          espReset();
        }
/*
        int i=0;
        bool truCode = false;
        while(!truCode) {
          if (irData == irCodes[i])
            trueCode = true;
          i++;
        }
*/        
        irrecv.resume();// Receive the next value
    }
    delay(1);//10
  }
  digitalWrite(GRNLED, HIGH);
  while(Mirf.dataReady()) {
  
    Mirf.getData(data);
    if ( ( data[0] > 0 ) && ( data[0] < 29 ) ) {
      int id = int( data[0] ); //BUTTON ID
      unsigned long curTime = millis();
      if ( ( curTime - timeLast[ id-1 ] ) > threshold ) {
        int voltage = data[1] * 24;
        String mData;
        if (data[0] == byte(28))
          mData = "24; " + String(voltage);
        else {
          mData = String(data[0]) + "; " + String(voltage);
        }
       
        espDataSend(mData);
        timeLast[id-1] = curTime;
      }
    }
  }
  delay(1);//10
  digitalWrite(GRNLED, LOW);

}
