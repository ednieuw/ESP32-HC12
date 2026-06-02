// HC12_Tester.ino - Test two HC-12 modules with one Arduino Nano ESP32
// Module A on Serial1, Module B on Serial2, loopback test via onboard RGB LED
// HC-12: TX->RX, RX->TX, VCC->5V (or 3V3), GND->GND, SET->open (transparent mode)

#define A_RX D4          // Nano RX  <- HC-12 A TXD
#define A_TX D5          // Nano TX  -> HC-12 A RXD
#define B_RX D6          // Nano RX  <- HC-12 B TXD
#define B_TX D7          // Nano TX  -> HC-12 B RXD
#define HC12BAUD 9600    // HC-12 default baud
#define CYCLEMS  1000    // ms between test cycles
#define WAITMS   400     // ms to wait for a reply

HardwareSerial HC12A(1);
HardwareSerial HC12B(2);

uint8_t Hr=0, Mn=0, Sec=0;

void setup()
{
  Serial.begin(115200); delay(500);
  HC12A.begin(HC12BAUD,SERIAL_8N1,A_RX,A_TX);
  HC12B.begin(HC12BAUD,SERIAL_8N1,B_RX,B_TX); 
  Serial.println("HC-12 dual tester ready");
}

void loop()
{
  String Tm=TimeStr();
                                                 // --- Test 1: A sends -> B receives ---
  String Msg=Tm; Flash(0,0,30);                                                          // blue = sending
  while(HC12B.available()) HC12B.read();                                                 // flush receiver B
  HC12A.println(Msg); Serial.print("A->B sent: "+ Msg);
  if(WaitReply(HC12B,Msg)){ Serial.println("  OK"); Flash(0,30,0); }                     // green = received
  else                    { Serial.println("  FAIL (B got nothing)"); Flash(40,0,0); }   // red = no reply
  delay(CYCLEMS/2);
 

  Mn++; if(Mn>=60){ Mn=0; Hr++; if(Hr>=24) Hr=0; }                                       // increment by one minute
  Tm=TimeStr();
                                                  // --- Test 2: B sends -> A receives ---
  Msg=Tm; Flash(0,0,30);                                                                 // blue = sending
  while(HC12A.available()) HC12A.read();                                                 // flush receiver A
  HC12B.println(Msg); Serial.print("B->A sent: "+ Msg);
  if(WaitReply(HC12A,Msg)) { Serial.println("  OK"); Flash(0,30,0); }
  else                     { Serial.println("  FAIL (A got nothing)"); Flash(40,0,0); }
  Mn++; if(Mn>=60){ Mn=0; Hr++; if(Hr>=24) Hr=0; }                                       // increment by one minute
  delay(CYCLEMS);
}


void SetStatusLED(int Red, int Green, int Blue)
{
  analogWrite(LED_RED,   Red   == 0 ? 8191 : (512 - Red));
  analogWrite(LED_GREEN, Green == 0 ? 8191 : (512 - Green));
  analogWrite(LED_BLUE,  Blue  == 0 ? 8191 : (512 - Blue));
}

String TimeStr() {char b[16]; snprintf(b, sizeof(b),"T%02d%02d%02d",Hr,Mn,Sec); return String(b);}
void Led(uint8_t r,uint8_t g,uint8_t b)  { SetStatusLED(r,g,b); }
void Flash(uint8_t r,uint8_t g,uint8_t b){ Led(r,g,b); delay(120); Led(0,0,0); }

bool WaitReply(HardwareSerial &S,const String &Want)
{
 uint32_t t=millis(); 
 String In="";
 while(millis()-t<WAITMS)
    {
     while(S.available())
       {
        char c=S.read();
        if(c=='\n'){ Serial.print(" [got:"+In+" want:"+Want+"]"); return In==Want; }
        if(c!='\r') In+=c;
       }
     }
 Serial.print(" [timeout, got:"+In+"]");
 return false;
}
