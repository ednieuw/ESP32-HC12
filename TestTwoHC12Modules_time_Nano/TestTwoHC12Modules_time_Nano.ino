// HC12_Tester_Nano.ino - Test two HC-12 modules with one Arduino Nano (ATmega328)
// Module A en B via SoftwareSerial, hardware-UART blijft vrij voor de seriele monitor
// HC-12: TX->RX, RX->TX, VCC->5V, GND->GND, SET->open (transparent mode)

#include <SoftwareSerial.h>

#define A_RX 4          // Nano RX  <- HC-12 A TXD
#define A_TX 5          // Nano TX  -> HC-12 A RXD
#define B_RX 6          // Nano RX  <- HC-12 B TXD
#define B_TX 7          // Nano TX  -> HC-12 B RXD
#define HC12BAUD 9600   // HC-12 default baud
#define CYCLEMS  1000   // ms between test cycles
#define WAITMS   400    // ms to wait for a reply

SoftwareSerial HC12A(A_RX, A_TX);
SoftwareSerial HC12B(B_RX, B_TX);

uint8_t Hr=0, Mn=0, Sec=0;
String TimeStr(){ char b[8]; sprintf(b,"T%02d%02d%02d",Hr,Mn,Sec); return String(b); }

bool WaitReply(SoftwareSerial &S,const String &Want)
{
  uint32_t t=millis(); String In="";
  while(millis()-t<WAITMS){ while(S.available()){ char c=S.read(); if(c=='\n') return In==Want; if(c!='\r') In+=c; } }
  return false;
}

void setup()
{
  Serial.begin(9600); delay(500);
  HC12A.begin(HC12BAUD);
  HC12B.begin(HC12BAUD);
  Serial.println("HC-12 dual tester ready");
}

void loop()
{
  String Tm=TimeStr();

  // --- Test 1: A sends -> B receives ---
  String Msg=Tm;
  HC12B.listen(); while(HC12B.available()) HC12B.read();   // luister op B, leeg buffer
  HC12A.println(Msg); Serial.print("A->B sent: "+Msg);
  if(WaitReply(HC12B,Msg)) Serial.println("  OK");
  else                     Serial.println("  FAIL (B got nothing)");
  delay(CYCLEMS/2);

  Mn++; if(Mn>=60){ Mn=0; Hr++; if(Hr>=24) Hr=0; }
  Tm=TimeStr();

  // --- Test 2: B sends -> A receives ---
  Msg=Tm;
  HC12A.listen(); while(HC12A.available()) HC12A.read();   // luister op A, leeg buffer
  HC12B.println(Msg); Serial.print("B->A sent: "+Msg);
  if(WaitReply(HC12A,Msg)) Serial.println("  OK");
  else                     Serial.println("  FAIL (A got nothing)");

  Mn++; if(Mn>=60){ Mn=0; Hr++; if(Hr>=24) Hr=0; }

  delay(CYCLEMS);
}