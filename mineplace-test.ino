/*
PROJECT NAME: Mineplace Test
PROJECT VERSION: N.A.
PROJECT DESCRIPTION: Just some rendering testing.
CONTRIBUTERS: RyanYasuda and grace0lkjhb.
CHECK READ ME FOR MORE INFORMATION
*/





//----------SETUP----------


//import libraries
#include <SPI.h>
#include <Adafruit_GFX_Buffer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <ButtonDebounce.h>
#include <math.h>


//for the joystick
#define Y_PIN A2 //orange wire
#define X_PIN A3 //yellow wire
#define SW_PIN 2 //green wire
#define MIN_CLICK 100 //debounce ms
#define MAX_DCLICK 500 //double-click cutoff


//for the lcd rgb display
#define BROWN 0x69e0
#define TFT_SS 7
#define RST 8
#define DC 9
#define SD_SS 10


//library variable/object definitions
Adafruit_GFX_Buffer<Adafruit_ST7735> screen = Adafruit_GFX_Buffer<Adafruit_ST7735>(80, 160,  Adafruit_ST7735(TFT_SS, DC, RST)  );
ButtonDebounce button(SW_PIN, MIN_CLICK);


void setup() {
  screen.initR(INITR_BLACKTAB); // for 1.8' TFT
  screen.setTextWrap(false); // Allow text to run off the edge
  screen.setRotation(0);
  screen.fillScreen(ST7735_BLACK); //sets bg as black
  screen.setTextSize(1); //smallest text size
  screen.setTextColor(ST7735_WHITE);
  screen.setCursor(0,0); //puts cursor in top left, (which is where the text is printed from)
  randomSeed(analogRead(A5)); //Seed with bogus read
  Serial.begin(9600);
  test_setup();
}





//----------MAIN----------

void test_setup(){
  Serial.println()
}


void test(){
  return;
}


void loop(){
  button.update();
  test();
  screen.display();
}