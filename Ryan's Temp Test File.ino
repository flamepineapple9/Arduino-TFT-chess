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
  BoardSetup();
  
  //should draw one of each piece type, 
  DrawPiece(1, 1, 0, 0);
  DrawPiece(1, 2, 1, 0);
  DrawPiece(2, 1, 0, 1);
  DrawPiece(2, 2, 1, 1);
  DrawPiece(3, 1, 0, 2);
  DrawPiece(3, 2, 1, 2);
  DrawPiece(4, 1, 0, 3);
  DrawPiece(4, 2, 1, 3);
  DrawPiece(5, 1, 0, 4);
  DrawPiece(5, 2, 1, 4);
  DrawPiece(6, 1, 0, 5);
  DrawPiece(6, 2, 1, 5);
  //mytft.drawPixel(51,50,ST7735_BLACK);
  //mytft.drawPixel(50,50,ST7735_BLACK);
  //mytft.drawPixel(50,51,ST7735_BLACK);
  //mytft.drawPixel(51,51,ST7735_BLACK);
  delay(100000); //this is just here so it doesn't keep drawing everything all the time
}


//how are we going to represent the board. We have to also be able to tell what color each piece is
//maybe a two digit number, second digit being piece type, take the digit % 10 to figure out piece number, first digit is color?
//subtract 6 from the number (largest number would be a black king, 25), if > 10, black, if < 10, white.
int Board[8][8]; //8x8 grid for the board

const unsigned int ColorArray[2][5] = 
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
const unsigned int PiecesArray[6][14][14] = 
{{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 5, 4, 4, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 4, 3, 3, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 3, 3, 3, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 3, 2, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 4, 4, 3, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 3, 2, 2, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 3, 2, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 4, 3, 3, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 4, 4, 3, 3, 3, 2, 1, 0, 0, 0},
  {0, 0, 0, 1, 4, 2, 2, 2, 2, 2, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},

 {{0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0},
  {0, 0, 1, 5, 1, 0, 1, 3, 3, 3, 2, 1, 0, 0},
  {0, 0, 1, 4, 1, 1, 1, 3, 3, 3, 2, 1, 0, 0},
  {0, 0, 1, 4, 3, 3, 3, 3, 3, 2, 2, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 1, 5, 4, 3, 3, 2, 2, 1, 0, 0, 0},
  {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 4, 3, 3, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 4, 3, 3, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 4, 3, 3, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 5, 4, 3, 3, 2, 2, 1, 0, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
  {0, 1, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 0},
  {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}},

 {{0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 5, 4, 3, 1, 1, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 4, 3, 3, 3, 2, 1, 0, 0, 0},
  {0, 0, 0, 1, 4, 1, 3, 3, 3, 3, 2, 1, 0, 0},
  {0, 0, 1, 4, 3, 3, 3, 2, 3, 3, 2, 1, 0, 0},
  {0, 1, 4, 3, 3, 3, 2, 1, 3, 3, 2, 1, 0, 0},
  {0, 1, 4, 3, 1, 1, 1, 4, 3, 3, 2, 1, 0, 0},
  {0, 0, 1, 1, 0, 0, 1, 4, 3, 3, 2, 1, 0, 0},
  {0, 0, 0, 0, 0, 1, 4, 4, 3, 3, 2, 1, 0, 0},
  {0, 0, 0, 0, 1, 4, 3, 3, 3, 2, 1, 0, 0, 0},
  {0, 0, 0, 1, 5, 4, 3, 3, 2, 2, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 1, 4, 4, 3, 3, 3, 2, 2, 2, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}},
 
 {{0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 5, 2, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 4, 2, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 4, 2, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 5, 3, 3, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 5, 3, 3, 0, 1, 2, 1, 0, 0, 0},
  {0, 0, 0, 1, 4, 3, 3, 1, 3, 2, 1, 0, 0, 0},
  {0, 0, 0, 1, 4, 3, 3, 3, 3, 2, 1, 0, 0, 0},
  {0, 0, 0, 1, 4, 3, 3, 3, 3, 2, 1, 0, 0, 0},
  {0, 0, 0, 0, 1, 4, 3, 3, 2, 1, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 1, 4, 4, 3, 3, 3, 2, 2, 2, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}},

 {{0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 5, 2, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 4, 2, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
  {0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0},
  {0, 1, 5, 1, 0, 1, 4, 3, 1, 0, 1, 2, 1, 0},
  {0, 1, 5, 4, 1, 4, 3, 3, 3, 1, 2, 2, 1, 0},
  {0, 0, 1, 4, 4, 3, 3, 3, 3, 3, 2, 1, 0, 0},
  {0, 0, 1, 4, 4, 3, 3, 3, 3, 2, 2, 1, 0, 0},
  {0, 0, 0, 1, 4, 3, 3, 3, 2, 2, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 1, 4, 4, 3, 3, 3, 2, 2, 2, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}},

 {{0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 5, 2, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 4, 2, 1, 0, 0, 0, 0, 0},
  {0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0},
  {0, 1, 5, 3, 3, 1, 0, 0, 1, 3, 3, 2, 1, 0},
  {0, 1, 4, 0, 1, 3, 1, 1, 3, 1, 0, 2, 1, 0},
  {0, 1, 4, 0, 0, 1, 3, 3, 1, 0, 0, 2, 1, 0},
  {0, 1, 4, 1, 0, 0, 3, 3, 0, 0, 1, 2, 1, 0},
  {0, 0, 1, 4, 1, 1, 3, 3, 1, 1, 2, 1, 0, 0},
  {0, 0, 1, 4, 4, 3, 3, 3, 3, 2, 2, 1, 0, 0},
  {0, 0, 0, 1, 4, 3, 3, 3, 2, 2, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 1, 4, 4, 3, 3, 3, 2, 2, 2, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}}
};



//xSquare and ySquare is the square on the board, 0=black 1=white, refference PiecesArray
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

void BoardSetup(){
  for (int i = 0; i < 8; i++) {
    for (int k = 0; k < 8; k++){
      BlankSquare(k, i);
    }
  }
}

/*void DrawBoard(){
  //goes through board position array, and uses DrawPiece() to draw pieces in correct position depending on board position.
  //should be called every time board pos changes.
  for(int y = 1; i <= 8; i++){
    for(int x = 1; i <= 8; i++){
      DrawPiece(x, y, I do not know how to make this the color , Board[y][x];
    }
  }
}*/
