// HC12_Tester.ino - Test two HC-12 modules with one Arduino Nano ESP32
// Module A on Serial1, Module B on Serial2, loopback test via onboard RGB LED
// HC-12: TX->RX, RX->TX, VCC->5V (or 3V3), GND->GND, SET->open (transparent mode)

#define A_RX D4          // Nano RX  <- HC-12 A TXD
#define A_TX D5          // Nano TX  -> HC-12 A RXD
#define B_RX D6          // Nano RX  <- HC-12 B TXD
#define B_TX D7          // Nano TX  -> HC-12 B RXD
#define HC12BAUD 9600   // HC-12 default baud
#define CYCLEMS  1000   // ms between test cycles
#define WAITMS   400    // ms to wait for a reply

HardwareSerial HC12A(1);
HardwareSerial HC12B(2);
uint32_t Counter = 0;

void SetStatusLED(int Red, int Green, int Blue)
{
  analogWrite(LED_RED,   Red   == 0 ? 8191 : (512 - Red));
  analogWrite(LED_GREEN, Green == 0 ? 8191 : (512 - Green));
  analogWrite(LED_BLUE,  Blue  == 0 ? 8191 : (512 - Blue));
}

void Led(uint8_t r,uint8_t g,uint8_t b){ SetStatusLED(r,g,b); }
void Flash(uint8_t r,uint8_t g,uint8_t b){ Led(r,g,b); delay(120); Led(0,0,0); }
bool WaitReply(HardwareSerial &S,const String &Want)
{
  uint32_t t=millis(); String In="";
  while(millis()-t<WAITMS){ while(S.available()){ char c=S.read(); if(c=='\n') return In==Want; if(c!='\r') In+=c; } }
  return false;
}

void setup()
{
  Serial.begin(115200); delay(500);
  HC12A.begin(HC12BAUD,SERIAL_8N1,A_RX,A_TX);
  HC12B.begin(HC12BAUD,SERIAL_8N1,B_RX,B_TX);
  Serial.println("HC-12 dual tester ready");
}

void loop()
{
  // --- Test 1: A sends -> B receives ---
  String Msg="A2B#"+String(Counter); Flash(0,0,30);;                                        // blue = sending
    while(HC12B.available()) HC12B.read();
  HC12A.println(Msg); Serial.print("A->B sent: "+ Msg);
  if(WaitReply(HC12B,Msg)){ Serial.println("  OK"); Flash(0,30,0); }                       // green = received
  else                    { Serial.println("  FAIL (B got nothing)"); Flash(40,0,0); }     // red = no reply
  delay(CYCLEMS/2);

  // --- Test 2: B sends -> A receives ---
  Msg="B2A#"+String(Counter); Flash(0,0,30);
      while(HC12A.available()) HC12A.read();

  HC12B.println(Msg); Serial.print("B->A sent: "+ Msg);

  if(WaitReply(HC12A,Msg)) { Serial.println("  OK"); Flash(0,30,0); }
  else                     { Serial.println("  FAIL (A got nothing)"); Flash(40,0,0); }

  Counter++; delay(CYCLEMS);
}
