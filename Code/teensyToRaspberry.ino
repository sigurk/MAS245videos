

// Delprosjekt 3



#include <FlexCAN.h>    
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Metro.h> 

Metro serialMetro = Metro(1000);  // initier et tidsintervall på 1 sekund
int msgCount = 0;      // meldingsteller

#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);


#if (SSD1306_LCDHEIGHT != 64)                                 // feilmelding 
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

static CAN_message_t msg, rxmsg;       // derfinerer msg=melding som skal sendes, og rxmsg = melding som skal mottas


uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;   // set delay mellom nye samples

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);   // set i2c adress for BNO 055


void setup(void)
{
  Serial.begin(115200);
  Serial.println("Orientation Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  Can1.begin(); //sett bitrate her.  default er 250 kBit/sek

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  delay(1000);

}


void loop(void) {

  sensors_event_t  accelerometerData;
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);   // Leser fra sensor:
  printEvent(&accelerometerData);
  delay(BNO055_SAMPLERATE_DELAY_MS);

  while (Can1.read(rxmsg)) {     // denne løkken teller alle meldingene som Can1.read(rxmsg) leser, og printer antall meldinger + meldingsID
    msgCount += 1;     // meldingsteller

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE, BLACK );
    display.setCursor(96, 28);
    display.cp437(true);
    display.println(msgCount);    // skriver meldingstelleren på LED skjermen

    display.setCursor(0, 15);
    display.setCursor(96, 37);
    display.println(rxmsg.id, HEX);  // skriver meldings-ID på LED-skjerm
    display.display();
  }


  display.drawRoundRect(1, 1, display.width() - 1, display.height() - 1, display.height() / 4, SSD1306_WHITE); // tegner et rektangel med runde kanter
  display.display();


  display.setTextSize(1);                      // sett tekststørrelse
  display.setTextColor(SSD1306_WHITE, BLACK ); // sett farge
  display.setCursor(8, 7);                     // setter posisjon
  display.cp437(true);                         // bruk full 256 char 'Code Page 437' font
  display.write("MAS245 - Gruppe 8");

  display.setCursor(8, 16);
  display.write("CAN-statistikk");

  display.setCursor(5, 22);
  display.write("-------------------");

  display.setCursor(5, 28);
  display.write("Antall motatt:");

  display.setCursor(5, 37);
  display.write("Mottok sist ID: ");


  display.setCursor(5, 43);
  display.write("-------------------");

}  /// loop end


void printEvent(sensors_event_t* event) {
  double x = -1000000, y = -1000000 , z = -1000000; // definerer x, y og z som urealistiske verdier. Dersom desse plottes er det noe feil med målingene
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }

  Serial.print("\tx= ");
  Serial.print(x);
  Serial.print(" |\ty= ");
  Serial.print(y);
  Serial.print(" |\tz= ");
  Serial.println(z);

  display.setTextSize(1);
  display.setTextColor(WHITE );
  display.setCursor(7, 50);
  display.cp437(true);
  display.write("z-aks. :");
  display.setCursor(87, 50);
  display.write("m/s^2 ");

  display.setTextColor(WHITE, BLACK );
  display.setCursor(60, 50);
  display.println(z);
  display.display();

  Can1.read(rxmsg);
  if (( rxmsg.id == 0x245) & (serialMetro.check() == 1)) {   // sender sensordata dersom can1.read() leser en melding med ID=0x245 og hver gang serialMetro har gjennomført et intervall
    msg.len = 8;
    msg.id = 0x22;
    msg.buf[0] = x;
    msg.buf[1] = y;
    msg.buf[2] = z;
    msg.buf[3] = 0;
    msg.buf[4] = 0;
    msg.buf[5] = 0;
    msg.buf[6] = 0;
    msg.buf[7] = 0;

    Can1.write(msg);
  }
}
