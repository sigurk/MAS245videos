

//   Delprosjekt 2


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

  Can1.begin();                               // set bitrate her.
  display.begin(SSD1306_SWITCHCAPVCC);        //  start LED-display
  display.clearDisplay();
  delay(1000);

}


void loop(void) {

  sensors_event_t  accelerometerData;                                          // Leser fra sensor
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  printEvent(&accelerometerData);
  delay(BNO055_SAMPLERATE_DELAY_MS);


  while (Can1.read(rxmsg)) {
    msgCount += 1;               // meldingsteller

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE, BLACK );
    display.setCursor(96, 28);
    display.cp437(true);
    display.println(msgCount);          // skriver antall meldinger på LED skjerm

    display.setCursor(0, 15);
    display.setCursor(96, 37);
    display.println(rxmsg.id, HEX);    // Skriver MeldingsID på LED skjerm
    display.display();
  }



  display.drawRoundRect(1, 1, display.width() - 1, display.height() - 1, display.height() / 4, SSD1306_WHITE);      // tegner et rektangel med runde kanter
  display.display();


  display.setTextSize(1);                 // sett tekststørrelse
  display.setTextColor(SSD1306_WHITE);    // sett tekstfarge
  display.setCursor(8, 7);                // sett posisjon
  display.cp437(true);                    // full 256 char 'Code Page 437' font
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
  double x = -1000000, y = -1000000 , z = -1000000;       //  initialiserer x, y og z med urealistiske verdier. Dersom desse blir printet, er det en feil med målingene
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {         // loggfører akselerasjonsdata i x-, y- og z-retning
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }

  Serial.print("\tx= ");             // printer akselerasjonsdata i x,y og z i seriell monitor
  Serial.print(x);
  Serial.print(" |\ty= ");
  Serial.print(y);
  Serial.print(" |\tz= ");
  Serial.println(z);


  display.setTextSize(1);            // printer akselerasjonsdata i z-retning på LED-skjerm
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
  if (( rxmsg.id == 0x21) & (serialMetro.check() == 1)) {       // sender  via CAN-nettverk dersom Can1.read() får en melding med ID=0x21, og hver gang serialmetro har gjennomørt et intervall.
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
