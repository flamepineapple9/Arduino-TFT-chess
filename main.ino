/*
This is chess meant to be played on a 128/160 tft display
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
  
  //  v part of code v
  BoardSetup();
}

//--------- ^ setup ^ --------- 
//------ v actual code v -----

unsigned int xCursor = 0;
unsigned int yCursor = 0;
unsigned int xLog = 8;
unsigned int yLog = 8;
bool buttonState = false;

//each piece is represented by a 2 digit number, first digit is the color 1 = white, 2 = black, to access color in program, < 20 = white, >= 20 = black
//second digit is the piece type, 0-5. To access piece type in program, take the two digit number % 10 
//an element being 9 means no piece on that square, can't use 0, that indicates a pawn
unsigned int Board[8][8] = {
  {11, 12, 13, 14, 15, 13, 12, 11},
  {10, 10, 10, 10, 10, 10, 10, 10},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {20, 20, 20, 20, 20, 20, 20, 20},
  {21, 22, 23, 24, 25, 23, 22, 21}
};

const unsigned int ColorArray[2][5] = 
 {{0x0000, 0xA534, 0xC618, 0xE71C, 0xFFFF}, // white, dark --> light
  {0x0000, 0x10A2, 0x2124, 0x4A49, 0x6B6D}}; // black, dark --> light

/*3d array, [piece][row][pixel], 
PiecesArray[0] = pawn
           [1] = rook
           [2] = knight
           [3] = bishop
           [4] = queen
           [5] = king
*/
const unsigned char PiecesArray[6][14][14] = 
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
        if (PiecesArray[piece][y][x] != 0x00){
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
    mytft.fillRect(k*16+1, i*16+1, 14, 14, 0x7EB2);
  } else {
    mytft.fillRect(k*16+1, i*16+1, 14, 14, 0x2447);
  }
}
  
  
void BlankOutline(int k,int i){
  //Ryan here, if i=0, then (0 + k)%2 just means every other, but as we increase i, i+constant will oscillate between
  //even and odd, meaning (k0 + i0)%2 != (k0 + i1)%2, thus making a given row the inverse of the next.
  if ((k + i) % 2 == 1){
    mytft.drawRect(k*16, i*16, 16, 16, 0x7EB2);
  } else {
    mytft.drawRect(k*16, i*16, 16, 16, 0x2447);
  }
}
  
  
void CursorOutline(int k,int i){
  mytft.drawRect(k*16, i*16, 16, 16, 0xf800);
}


void SelectOutline(int k,int i){
  mytft.drawRect(k*16, i*16, 16, 16, 0x001f);
}


void BoardSetup(){
  //draws board
  for (int i = 0; i < 8; i++) {
    for (int k = 0; k < 8; k++){
      BlankSquare(k, i);
      BlankOutline(k, i);
      if (Board[i][k] != 0){
        DrawPiece(k, i, round((Board[i][k])/10)-1, Board[i][k]%10)
      }
    }
  }
}
  
  
void MovePiece(int x1, int y1, int x2, int y2){
  Board[y2][x2] = Board[y1][x1];
  Board[y1][x1] = 0;
  BlankSquare(x1, y1);
  BlankOutline(x1, y1);
  BlankSquare(x2, y2);
  DrawPiece(x2, y2, round((Board[y2][x2])/10)-1, Board[y2][x2]%10);
}
 

void UpdateCursor(int xJoy, int yJoy) { // moves the cursor
   if (xJoy > 611){
      if (xCursor < 7){
        if ((xLog == xCursor) && (yLog == yCursor)){
          SelectOutline(xCursor, yCursor);
        }else{
          BlankOutline(xCursor, yCursor);
        }
        xCursor += 1;
        CursorOutline(xCursor, yCursor);
        delay(300);
      }
   }else if (xJoy < 411){
      if (xCursor > 0){
        if ((xLog == xCursor) && (yLog == yCursor)){
          SelectOutline(xCursor, yCursor);
        }else{
          BlankOutline(xCursor, yCursor);
        } 
        xCursor -= 1;
        CursorOutline(xCursor, yCursor);
        delay(300);
      }
   }
  
   if (yJoy > 611){
      if (yCursor < 7){
        if ((xLog == xCursor) && (yLog == yCursor)){
          SelectOutline(xCursor, yCursor);
        }else{
          BlankOutline(xCursor, yCursor);
        }
        yCursor += 1;
        CursorOutline(xCursor, yCursor);
        delay(300);
      }
   }else if (yJoy < 411){
      if (yCursor > 0){
        if ((xLog == xCursor) && (yLog == yCursor)){
          SelectOutline(xCursor, yCursor);
        }else{
          BlankOutline(xCursor, yCursor);
        }
        yCursor -= 1;
        CursorOutline(xCursor, yCursor);
        delay(300);
      }
   } 
  
   if ((button.state() == LOW) && (buttonState)){
    if ((xLog == 8) && (Board[yCursor][xCursor] != 0)){
      xLog = xCursor;
      yLog = yCursor;
    }else{
      if (((xLog != xCursor) || (yLog != yCursor)) && (xLog != 8)){
        MovePiece(xLog, yLog, xCursor, yCursor);
        xLog = 8;
        yLog = 8;
      }
    }
    buttonState = false;
   }else if ((button.state() == HIGH) && !(buttonState)){
    buttonState = true;
   }
}

  
  
void loop(){
  button.update();
  UpdateCursor(analogRead(X_PIN), analogRead(Y_PIN));
  delay(10); //this is just here so it doesn't keep drawing everything all the time
}
