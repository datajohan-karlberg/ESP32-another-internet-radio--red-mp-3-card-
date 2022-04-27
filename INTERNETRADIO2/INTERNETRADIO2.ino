// Tested: ESP8266, ESP32, M0+WINC1500

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>

#include <EasyButton.h>
#include <RingBuf.h>
RingBuf<byte, 30000> myBuffer;

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 2);


char* ssid     = "zzzzz";
const char* password = "zzzzzzz";



int httpPort = 80;//443;//80;


EasyButton button(0);
boolean blink = false;
// These are the pins used
#define VS1053_RESET   -1     // VS1053 reset pin (not used!)
#define VS1053_CS      5     // VS1053 chip select pin (output)
#define VS1053_DCS     16//15     // VS1053 Data/command select pin (output)
#define VS1053_DREQ    4// 0     // VS1053 Data request, ideally an Interrupt pin

#define VOLUME_KNOB    A0
#define ON_OFF_SWITCH  33

int lastvol = 30;
int prog = 0;
unsigned long times = millis();

Adafruit_VS1053 musicPlayer =  Adafruit_VS1053(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ);

// Use WiFiClient class to create HTTP/TCP connection
WiFiClient client;

void setup() {
  Serial.begin(115200);

  Serial.println("\n\n RED VS1053 card Feather WiFi Radio");

  /************************* INITIALIZE MP3 WING */
  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1) delay(10);
  }

  Serial.println(F("VS1053 found"));
  musicPlayer.sineTest(0x44, 50);    // Make a tone to indicate VS1053 is working

  //musicPlayer.setEarSpeaker(3);

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(lastvol, lastvol);

  // don't use an IRQ, we'll hand-feed

  pinMode(ON_OFF_SWITCH, INPUT_PULLUP);
  /************************* INITIALIZE WIFI */
  Serial.print("Connecting to SSID "); Serial.println(ssid);

  lcd.begin();
  lcd.setCursor(0, 0);
  lcd.print("connecting   ");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 0);
  lcd.print("Connected!!!   ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());


  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  /************************* INITIALIZE STREAM */
  Serial.print("connecting to ");








  // We now create a URI for the request
  // Serial.print("Requesting URL: ");
  //Serial.println(path);

  onPressed();

  button.begin();
  button.onPressed(onPressed);
  pinMode(2, OUTPUT);

  myBuffer.clear();


}




// our little buffer of mp3 data
byte mp3buff[32];   // vs1053 likes 32 bytes at a time
byte buffbuff[5001];
byte byteread;
int venta = 4000;




void loop() {
  if (! digitalRead(ON_OFF_SWITCH)) {
    yield();
    return;
  }



  ///////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////


  if (musicPlayer.readyForData() & myBuffer.size() > venta) {
    //Serial.print("ready ");
    venta = 64;
    //byte byteread;
    for (int i = 0; i < 32; i++) {
      myBuffer.pop(byteread);
      mp3buff[i] = byteread;
    }
    musicPlayer.playData(mp3buff, 32);
  }

  ///////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////

  uint16_t rr = myBuffer.size();
  if (client.available() > 50 & (rr < 10000)) {
    //Serial.print("set ");
    // yea! read up to 32 bytes
    uint16_t bytesread1 = client.read(buffbuff, 5000);


    for (int i = 0; i < bytesread1; i++) {
      myBuffer.push(buffbuff[i]);
    }



    //byte byteswrite = client.read();
    //myBuffer.push(byteswrite);

    digitalWrite(2, blink);
    blink = blink ^ 1;

    //Serial.println("stream!");
  }








  ///////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////



  if ( millis() - times >= 3000) {
    times = millis();
    // adjust volume!
    Serial.println(myBuffer.size());

    //musicPlayer.currentPosition();



    if (!client.connected()) {
      Serial.println();
      Serial.println("disconnecting.");
      //client.stop();
      conectradio();
    }




    int vol = 0;
    vol = 800;//analogRead(VOLUME_KNOB);
    vol /= 10;
    vol = 0;
    if (abs(vol - lastvol) > 3) {
      // Serial.println(vol);
      lastvol = 0;//vol;
      musicPlayer.setVolume(lastvol, lastvol);
    }
  }

  button.read();
}


void onPressed() {
  prog = prog + 1;
  if (prog > 21) prog = 1;


  conectradio();

}



void conectradio() {
  //client.flush();

  while (client.connected()) {
    while (client.available()) client.flush();
  }
  client.stop();

  delay(100);
  client.stop();

  Serial.println("Button has been pressed!");
  delay(100);



  Serial.println(prog);

  //musicPlayer.sineTest(0x44, 50);

  char text[40];
  char ax1[40];
  char ax1k[40];
  int porten = 80;
  //kanal(prog,a1,a2);



  switch (prog) {
    //////////////////////////////////
    case 1:
      strcpy(ax1, "http-live.sr.se");
      strcpy(text, "/p1-mp3-192");
      strcpy(ax1k, "SR P1.");
      break;
    //////////////////////////////////
    case 2:
      strcpy(ax1, "http-live.sr.se");
      strcpy(text, "/p2-mp3-192");
      strcpy(ax1k, "SR P2.");
      break;
    //////////////////////////////////
    case 3:
      strcpy(ax1, "http-live.sr.se");
      strcpy(text, "/p3-mp3-192");
      strcpy(ax1k, "SR P3.");
      break;
    //////////////////////////////////
    case 4:
      strcpy(ax1, "http-live.sr.se");
      strcpy(text, "/p4blekinge-mp3-192");
      strcpy(ax1k, "SR P4 Blekinge");
      break;
    //////////////////////////////////
    case 5:
      strcpy(ax1, "http-live.sr.se");
      strcpy(text, "/p4stockholm-mp3-192");
      strcpy(ax1k, "SR P4 Stockholm");
      break;
    //////////////////////////////////
    case 6:
      strcpy(ax1, "http-live.sr.se");
      strcpy(text, "/p4uppland-mp3-192");
      strcpy(ax1k, "SR P4 Uppland");
      break;
    //////////////////////////////////
    case 7:
      strcpy(ax1, "http-live.sr.se");
      strcpy(text, "/dingata-mp3-192");
      strcpy(ax1k, "SR Din gata");
      break;
    //////////////////////////////////
    case 8:
      strcpy(ax1, "http-live.sr.se");
      strcpy(text, "/srinternational-mp3-192");
      strcpy(ax1k, "SR International.");
      break;
    //////////////////////////////////
    case 999:
      strcpy(ax1, "23.106.36.91");
      strcpy(text, "/listen.pls.mp3");
      strcpy(ax1k, "Old Radio.");
      porten = 8070;
      break;
    //////////////////////////////////
    case 10:
      strcpy(ax1, "ice1.somafm.com");
      strcpy(text, "/u80s-128-mp3");
      strcpy(ax1k, "SomaFM UK-80.");
      break;
    //////////////////////////////////
    case 11:
      strcpy(ax1, "ice1.somafm.com");
      strcpy(text, "/seventies-128-mp3");
      strcpy(ax1k, "SomaFM 1970");
      break;
    //////////////////////////////////
    case 12:
      strcpy(ax1, "ice1.somafm.com");
      strcpy(text, "/u80s-128-mp3");
      strcpy(ax1k, "SomaFM UK-80.");
      break;
    //////////////////////////////////
    case 13:
      strcpy(ax1, "ice6.somafm.com");
      strcpy(text, "/deepspaceone-128-mp3");
      strcpy(ax1k, "SomaFM DeepSpace.");
      break;
    //////////////////////////////////
    case 14:
      strcpy(ax1, "ice2.somafm.com");
      strcpy(text, "/secretagent-128-mp3");
      strcpy(ax1k, "SomaFM Secret Agent");
      break;
    //////////////////////////////////
    case 15:
      strcpy(ax1, "ice2.somafm.com");
      strcpy(text, "/reggae-128-mp3");
      strcpy(ax1k, "SomaFM Reggae");
      break;
    //////////////////////////////////
    case 16:
      strcpy(ax1, "ice6.somafm.com");
      strcpy(text, "/dubstep-128-mp3");
      strcpy(ax1k, "SomaFM Dubstep.");
      break;
    //////////////////////////////////
    case 17:
      strcpy(ax1, "ice4.somafm.com");
      strcpy(text, "/suburbsofgoa-128-mp3");
      strcpy(ax1k, "SomaFM Suburb");
      break;
    //////////////////////////////////
    case 18:
      strcpy(ax1, "stream.live.vc.bbcmedia.co.uk");
      strcpy(text, "/bbc_radio_one");
      strcpy(ax1k, "BBC London");
      break;
    //////////////////////////////////
    case 19:
      strcpy(ax1, "stream.live.vc.bbcmedia.co.uk");
      strcpy(text, "/bbc_radio_york-mp3");
      strcpy(ax1k, "BBC NewYork");
      break;
    //////////////////////////////////
    case 20:
      strcpy(ax1, "icecast.thisisdax.com");
      strcpy(text, "/LBCNewsUKMP3");
      strcpy(ax1k, "LBC News");
      break;
    //////////////////////////////////
    case 21:
      strcpy(ax1, "213.239.218.99");
      strcpy(text, "/listen.pls.mp3");
      strcpy(ax1k, "Anticorona Radio");
      porten = 7086;
      break;
    default:
      // statements
      //ax1="+";
      //text="+";
      break;
  }




  if (!client.connect(ax1, porten)) {
    Serial.println("Connection failed");
    return;
  }
  delay(100);
  Serial.println(ax1);
  Serial.println(text);
  Serial.println(ax1k);

  lcd.setCursor(0, 0);
  lcd.print("                    ");
  lcd.setCursor(0, 0);
  lcd.print(ax1k);


  int uu = 15 * 16;
  uu = uu + (15 * 4096);
  musicPlayer.sciWrite(2, uu);

  client.print(String("GET ") + text + " HTTP/1.1\r\n" +
               "Host: " + ax1 + "\r\n" +
               "Connection: close\r\n\r\n");


  venta = 4000;
}
