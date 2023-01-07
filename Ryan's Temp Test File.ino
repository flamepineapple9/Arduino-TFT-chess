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

Adafruit_ST7735 mytft = Adafruit_ST7735(TFT_SS, DC, RST); // idk why this is needed      Grace i'm pretty sure this creates the screen object -Eli
ButtonDebounce button(SW_PIN, MIN_CLICK);// seems to be defining some values for the button debounce library

//array for colors, pretty sure we can delete this, unless it's needed for the screen to operate somehow. no harm in leaving it though
unsigned int dispColors[] = { ST7735_BLACK, ST7735_RED, ST7735_ORANGE, ST7735_YELLOW, ST7735_GREEN, ST7735_CYAN, ST7735_BLUE, ST7735_MAGENTA, ST7735_WHITE };
byte numColors =  sizeof(dispColors) / sizeof(dispColors[0]); //how many colors

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

//--------- ^ setup ^ --------- 
//------ v actual code v -----

void loop() {
  DrawBoard();
  DrawPiece(5, 4, 0, 0);
  delay(100000); //this is just here so it doesn't keep drawing everything all the time
}



int Board[8][8]; //8x8 grid for the board

int ColorArray[2][5] = 
 {{0x0000, 0x10A2, 0x2124, 0x4A49, 0x6B6D}, // white, dark --> light
  {0x0000, 0xA534, 0xC618, 0xE71C, 0xFFFF}}; // black, dark --> light

/*3d array, [piece][row][pixel], 
PiecesArray[0] = pawn
           [1] = rook
           [2] = knight
           [3] = bishop
           [4] = queen
           [5] = king
*/
//remember to change the array length to 6 when we actually add the rest of the pieces lol
int PiecesArray[1][14][14] = 
{{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 0, 0, 0},
  {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
};



//xSquare and ySquare is the square on the board 
void DrawPiece(int xSquare, int ySquare, int color, int piece){
 //Ryan here, could use less loops, but for editings sake and to reduce calculations, this will do.
  for(int y = 0; y < 14; y++){
    for(int x = 0; x < 14; x++){
      if (PiecesArray[piece][y][x] == 0){
        break;
      } else {
       //             |  x position   |  y position  |                     color                      |
       mytft.drawPixel(xSquare*16+x+1, ySquare*16+y+1, ColorArray[color][PiecesArray[piece][y][x] - 1]);
      }
    }
  } 
}

void BlankSquare(int k,int i){
  //Ryan here, if i=0, then (0 + k)%2 just means every other, but as we increase i, i+constant will oscillate between
  //even and odd, meaning (k0 + i0)%2 != (k0 + i1)%2, thus making a given row the inverse of the next.
  if ((k + i) % 2 == 1){
    mytft.fillRect(k*16, i*16, 16, 16, 0x7EB2);
  } else {
    mytft.fillRect(k*16, i*16, 16, 16, 0x2447);
  }
}

void DrawBoard(){
  for (int i = 0; i < 8; i++) {
      for (int k = 0; k < 8; k++){
      BlankSquare(k, i);
    }
  }
}
