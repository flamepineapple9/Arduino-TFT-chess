/*
This will be chess meant to be played on a 128/160 tft display
*/
#include <SPI.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <ButtonDebounce.h>

// For the joystick
#define Y_PIN A2 //orange wire
#define X_PIN A3 //yellow wire
#define SW_PIN 2 //green wire
#define MIN_CLICK 100 //debounce ms
#define MAX_DCLICK 500 //double-click cutoff

//for the lcd rgb display
#define TFT_SS 7
#define RST 8
#define DC 9
#define SD_SS 10

int Board[8][8]; //8x8 grid for the board


Adafruit_ST7735 mytft = Adafruit_ST7735(TFT_SS, DC, RST); // idk why this is needed      Grace i'm pretty sure this creates the screen object -Eli
ButtonDebounce button(SW_PIN, MIN_CLICK);// seems to be defining some values for the button debounce library

//array for colors
unsigned int dispColors[] = { ST7735_BLACK, ST7735_RED, ST7735_ORANGE, ST7735_YELLOW, ST7735_GREEN, ST7735_CYAN, ST7735_BLUE, ST7735_MAGENTA, ST7735_WHITE };
byte numColors =  sizeof(dispColors) / sizeof(dispColors[0]); //how many colors


int RefPnt[2]; //refference point, top right corner pixel of the square, 0 = x, 1 = y


void setup() {
  mytft.initR(INITR_BLACKTAB); // for 1.8' TFT
  mytft.setTextWrap(false); // Allow text to run off the edge
  mytft.setRotation(0);
  mytft.fillScreen(ST7735_BLACK); //sets bg as black
  mytft.setTextSize(1); //smallest text size
  mytft.setTextColor(ST7735_WHITE);
  mytft.setCursor(0,0); //puts cursor in top left, (which is where the text is printed from)
  randomSeed(analogRead(A5)); //Seed with bogus read
  Serial.begin(9600);
}



void loop() {
  DrawBoard();
}

void BlankSquare(k, i){
  //i honestly don't know how to concicsely explain this logic, but it makes sense once you look at examples
  if ((k + i) % 2 == 1){
    mytft.fillRect(RefPnt[0], RefPnt[1], 16, 16, 0x7CD691);
  } else {
    mytft.fillRect(RefPnt[0], RefPnt[1], 16, 16, 0x278A3E);
  }
}

//its in the name
void DrawBoard(){
  for (int i = 0; i < 8; i++) {
      for (int k = 0; k < 8; k++){
      //sets the refference point that the squares are created from, ref point is the top right pixel of a square
      RefPnt[0] = k * 16;
      RefPnt[1] = i * 16;
      BlankSquare(k, i);
    }
  }
}


//also in the name
void DrawPawn(int x, int y, int player){
  //sets the refference point that all the other pixels build off of, ref point is the top right pixel of a square
  RefPnt[0] = x * 16;
  RefPnt[1] = y * 16;
  mytft.drawLine(RefPnt[0] + 5, RefPnt[1] + 2, RefPnt[0] + 9, RefPnt[1] + 2, ST7735_BLACK);
}
