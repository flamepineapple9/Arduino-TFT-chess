/*
PROJECT NAME: Arduino Chess
PROJECT VERSION: Sub 2.0
PROJECT DESCRIPTION: Chess made for an Arduino Uno/Pro Mini with a 1.8" tft
                     display and a joystick (will be d-pad) control scheme.
CONTRIBUTERS: RyanYasuda, flamepineapple9, and grace0lkjhb.

CHECK READ ME FOR MORE INFORMATION
*/



//----------SETUP----------

//import libraries
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


//library variable/object definitions
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_SS, DC, RST); // idk why this is needed
ButtonDebounce button(SW_PIN, MIN_CLICK);// seems to be defining some values for the button debounce library


void setup() {
  tft.initR(INITR_BLACKTAB); // for 1.8' TFT
  tft.setTextWrap(false); // Allow text to run off the edge
  tft.setRotation(0);
  tft.fillScreen(ST7735_BLACK); //sets bg as black
  tft.setTextSize(1); //smallest text size
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(0,0); //puts cursor in top left, (which is where the text is printed from)
  randomSeed(analogRead(A5)); //Seed with bogus read
  Serial.begin(9600);
  
  
  
  //----------GAME SETUP----------
  
  BoardSetup();
}



//----------CONSTANTS----------

/*3d array, [piece][row][column], 
PIECES_ARRAY[0] = pawn
            [1] = rook
            [2] = knight
            [3] = bishop
            [4] = queen
            [5] = king
*/
const unsigned char PIECES_ARRAY[6][14][14] = 
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
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}}};


const unsigned int COLOR_ARRAY[2][5] = 
 {{0x0000, 0x10A2, 0x2124, 0x4A49, 0x6B6D},  // black, dark --> light
  {0x0000, 0xA534, 0xC618, 0xE71C, 0xFFFF}}; // white, dark --> light



//----------VARIABLES----------

//Ryan here, each piece is represented by a number. <6 for black, >6 for white,
//and %7 for piece number (reference PieceArray). 6 is null. Also yeah, it's weird.
unsigned int Board[8][8] = {
  {1, 2, 3, 4, 5, 3, 2, 1},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {6, 6, 6, 6, 6, 6, 6, 6},
  {6, 6, 6, 6, 6, 6, 6, 6},
  {6, 6, 6, 6, 6, 6, 6, 6},
  {6, 6, 6, 6, 6, 6, 6, 6},
  {7, 7, 7, 7, 7, 7, 7, 7},
  {8, 9, 10, 11, 12, 10, 9, 8}};


bool LegalMovesLog[8][8] = {
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false}};


unsigned int XCursor = 0;
unsigned int YCursor = 0;
unsigned int XLog = 8;
unsigned int YLog = 8;
unsigned int UndoLog = 0;
unsigned int EnPassant = 9;
bool ButtonState = false;
bool CursorState = false;
bool RightCastle = true;
bool LeftCastle = true;
bool Turn = true;



//----------VISUAL FUNCTIONS----------

class DRAW{
  public:
    void BlankSquare(int k,int i){
      //Ryan here, if i=0, then (0 + k)%2 just means every other, but as we increase i, i+constant will oscillate between
      //even and odd, meaning (k0 + i0)%2 != (k0 + i1)%2, thus making a given row the inverse of the next.
      if((k + i) % 2 == 1){
        tft.fillRect(k*16+1, i*16+1, 14, 14, 0x7EB2);
      } else {
        tft.fillRect(k*16+1, i*16+1, 14, 14, 0x2447);
      }
    }
    
    
    void BlankOutline(int k,int i){
      if((k + i) % 2 == 1){
        tft.drawRect(k*16, i*16, 16, 16, 0x7EB2);
      } else {
        tft.drawRect(k*16, i*16, 16, 16, 0x2447);
      }
    }
    
    
    void CursorOutline(int k,int i){
      tft.drawRect(k*16, i*16, 16, 16, 0xf800);
    }


    void SelectOutline(int k,int i){
      tft.drawRect(k*16, i*16, 16, 16, 0x001f);
    }
    
    void LegalOutline(int k,int i){
      tft.drawRect(k*16, i*16, 16, 16, 0xffe0);
    }
    
    void Piece(int XSquare, int YSquare){
      //xSquare and ySquare is the square on the board, 0=black 1=white, refference PIECES_ARRAY
      if(Board[YSquare][XSquare] != 6){
        for(int y = 0; y < 14; y++){
          for(int x = 0; x < 14; x++){
            if(PIECES_ARRAY[Board[YSquare][XSquare]%7][y][x] != 0x00){
              //             |  x position  |   y position  |                           color                                |
              tft.drawPixel(XSquare*16+x+1, YSquare*16+y+1, COLOR_ARRAY[round(Board[YSquare][XSquare]/7)][PIECES_ARRAY[Board[YSquare][XSquare]%7][y][x]-1]);
            }
          }
        }
      }
    }
}Draw;


void BoardSetup(){
  //draws board
  for(int i = 0; i < 8; i++) {
    for(int k = 0; k < 8; k++){
      Draw.BlankSquare(k, i);
      Draw.BlankOutline(k, i);
      Draw.Piece(k, i);
    }
  }
  Draw.CursorOutline(XCursor, YCursor);
}



//----------LEGAL MOVES FUNCTIONS----------

class LegalMoves {
  public:
    void VertHoriz(){
      //VERTICAL
      //down from
      for(int i = YLog+1; i<=7; i++){
        if(Board[i][XLog] == 6){
          LegalMovesLog[i][XLog] = true;
          Draw.LegalOutline(XLog,i);
        }else{
          if(((Board[i][XLog] < 6)&&Turn) || ((Board[i][XLog] > 6)&&!Turn)){
            LegalMovesLog[i][XLog] = true;
            Draw.LegalOutline(XLog,i);
          }
          break;
        }
      }

      //up from
      for(int i = YLog-1; i>=0; i--){
        if(Board[i][XLog] == 6){
          LegalMovesLog[i][XLog] = true;
          Draw.LegalOutline(XLog,i); //Ryan here, need to update this
        }else{
          if(((Board[i][XLog] < 6)&&Turn) || ((Board[i][XLog] > 6)&&!Turn)){
            LegalMovesLog[i][XLog] = true;
            Draw.LegalOutline(XLog,i);
          }
          break;
        }
      }
      
      //HORZONTAL
      //right from
      for(int k = XLog+1; k<=7; k++){
        if(Board[YLog][k] == 6){
          LegalMovesLog[YLog][k] = true;
          Draw.LegalOutline(k,YLog);
        }else{
          if(((Board[YLog][k] < 6)&&Turn) || ((Board[YLog][k] > 6)&&!Turn)){
            LegalMovesLog[YLog][k] = true;
            Draw.LegalOutline(k,YLog);
          }
          break;
        }
      }

      //left from
      for(int k = XLog-1; k>=0; k--){
        if(Board[YLog][k] == 6){
          LegalMovesLog[YLog][k] = true;
          Draw.LegalOutline(k,YLog);
        }else{
          if(((Board[YLog][k] < 6)&&Turn) || ((Board[YLog][k] > 6)&&!Turn)){
            LegalMovesLog[YLog][k] = true;
            Draw.LegalOutline(k,YLog);
          }
          break;
        }
      }
    }
    
    
    void Diagonal(){
      //quadrant 1
      for(int i=1; i<=7; i++){
        if(Board[YLog-i][XLog+i]==6){
          LegalMovesLog[YLog-i][XLog+i] = true;
          Draw.LegalOutline(XLog+i,YLog-i);
          continue;
        }else if(Turn&&(Board[YLog-i][XLog+i]<6)){
          LegalMovesLog[YLog-i][XLog+i] = true;
          Draw.LegalOutline(XLog+i,YLog-i);
        }else if(!Turn&&(Board[YLog-i][XLog+i]>6)){
          LegalMovesLog[YLog-i][XLog+i] = true;
          Draw.LegalOutline(XLog+i,YLog-i);
        }
        break;
      }
      //quadrant 2
      for(int i=1; i<=7; i++){
        if(Board[YLog-i][XLog-i]==6){
          LegalMovesLog[YLog-i][XLog-i] = true;
          Draw.LegalOutline(XLog-i,YLog-i);
          continue;
        }else if(Turn&&(Board[YLog-i][XLog-i]<6)){
          LegalMovesLog[YLog-i][XLog-i] = true;
          Draw.LegalOutline(XLog-i,YLog-i);
        }else if(!Turn&&(Board[YLog-i][XLog-i]>6)){
          LegalMovesLog[YLog-i][XLog-i] = true;
          Draw.LegalOutline(XLog-i,YLog-i);
        }
        break;
      }
      //quadrant 3
      for(int i=1; i<=7; i++){
        if(Board[YLog+i][XLog-i]==6){
          LegalMovesLog[YLog+i][XLog-i] = true;
          Draw.LegalOutline(XLog-i,YLog+i);
          continue;
        }else if(Turn&&(Board[YLog+i][XLog-i]<6)){
          LegalMovesLog[YLog+i][XLog-i] = true;
          Draw.LegalOutline(XLog-i,YLog+i);
        }else if(!Turn&&(Board[YLog+i][XLog-i]>6)){
          LegalMovesLog[YLog+i][XLog-i] = true;
          Draw.LegalOutline(XLog-i,YLog+i);
        }
        break;
      }
      //quadrant 4
      for(int i=1; i<=7; i++){
        if(Board[YLog+i][XLog+i]==6){
          LegalMovesLog[YLog+i][XLog+i] = true;
          Draw.LegalOutline(XLog+i,YLog+i);
          continue;
        }else if(Turn&&(Board[YLog+i][XLog+i]<6)){
          LegalMovesLog[YLog+i][XLog+i] = true;
          Draw.LegalOutline(XLog+i,YLog+i);
        }else if(!Turn&&(Board[YLog+i][XLog+i]>6)){
          LegalMovesLog[YLog+i][XLog+i] = true;
          Draw.LegalOutline(XLog+i,YLog+i);
        }
        break;
      }
    }
    
    
    void King(){
      for(int i=YLog-1; i<YLog+2; i++){
        for(int k=XLog-1; k<XLog+2; k++){
          if(((i>=0)&&(i<=7)&&(k>=0)&&(k<=7)&&!((i==0)&&(k==0))) && !(((Board[i][k]<6)&&!Turn)||((Board[i][k]>6)&&Turn))){
            LegalMovesLog[i][k] = true;
            Draw.LegalOutline(k,i);
          }
        }
      }
    }
    
    
    void Knight(){
      for(int i=-2; i<=2; i++){
        for(int k=-2; k<=2; k++){
          if((i+k+4)%2==1 && i!=0 && k!=0 && (i+YLog>=0&&i+YLog<=7) && (k+XLog>=0&&k+XLog<=7) && !((Board[YLog+i][XLog+k]<6&&!Turn)||(Board[YLog+i][XLog+k]>6&&Turn))){
            LegalMovesLog[i+YLog][k+XLog] = true;
            Draw.LegalOutline(k+XLog,i+YLog);
          }
        }
      }
    }
    
    
    void Pawn(){
      //normal move
      if(!(((Board[YLog-1][XLog]<6)&&!Turn)||((Board[YLog-1][XLog]>6)&&Turn))){
        LegalMovesLog[YLog-1][XLog] = true;
        Draw.LegalOutline(XLog,YLog-1);
      }
      //pawn jump
      if(!(((Board[YLog-2][XLog]<6)&&!Turn)||((Board[YLog-2][XLog]>6)&&Turn)) && (YLog==6)){
        LegalMovesLog[YLog-2][XLog] = true;
        Draw.LegalOutline(XLog,YLog-2);
      }
      //en passant
      if(EnPassant && YLog==3){
        if(EnPassant==XLog-1){
          LegalMovesLog[YLog-1][XLog-1] = true;
          Draw.LegalOutline(XLog-1,YLog-1);
        }else if(EnPassant==XLog+1){
          LegalMovesLog[YLog-1][XLog+1] = true;
          Draw.LegalOutline(XLog+1,YLog-1);
        }
      }
    }
    
    
    void Castle(){
      //Ryan here, this needs work
      if(Board[YLog][XLog]%7==1){
        if(RightCastle||LeftCastle){
          LegalMovesLog[7][4] = true;
          Draw.LegalOutline(4,7);
        }
      }else if(Board[YLog][XLog]%7==5){
        if(RightCastle){
          LegalMovesLog[7][7] = true;
          Draw.LegalOutline(7,7);
        }
        if(LeftCastle){
          LegalMovesLog[7][0] = true;
          Draw.LegalOutline(0,7);
        }
      }
    }
    
    
    void Reset(){
      for(int x=0; x<8; x++){
        for(int y=0; y<8; y++){
          if(LegalMovesLog[y][x] == true){
            LegalMovesLog[y][x] = false;
          }
        }
      }
    }
}LegalMoves;


void GenerateLegalMoves(){
  switch(Board[YLog][XLog]%7){
    //Pawn
    case 0:
      LegalMoves.Pawn();
      break;
    //Rook
    case 1:
      LegalMoves.VertHoriz();
      LegalMoves.Castle();
      break;
    //Knight
    case 2:
      LegalMoves.Knight();
      break;
    //Bishop
    case 3:
      LegalMoves.Diagonal();
      break;
    //Queen
    case 4:
      LegalMoves.VertHoriz();
      LegalMoves.Diagonal();
      break;
    //King
    default:
      LegalMoves.King();
      LegalMoves.Castle();
  }
}



//----------INTERNAL FUNCTIONS----------

void UpdateCursor(int XJoy, int YJoy) { // moves the cursor
  //moves cursor outline on x
  if(XJoy > 611){
    if(XCursor < 7){
      if((XLog == XCursor) && (YLog == YCursor)){
        Draw.SelectOutline(XCursor, YCursor);
      }else if(LegalMovesLog[YCursor][XCursor]){
        Draw.LegalOutline(XCursor, YCursor);
      }else{
        Draw.BlankOutline(XCursor, YCursor);
      }
      XCursor += 1;
      Draw.CursorOutline(XCursor, YCursor);
      delay(175);
    }
  }else if(XJoy < 411){
    if(XCursor > 0){
      if((XLog == XCursor) && (YLog == YCursor)){
        Draw.SelectOutline(XCursor, YCursor);
      }else if(LegalMovesLog[YCursor][XCursor]){
        Draw.LegalOutline(XCursor, YCursor);
      }else{
        Draw.BlankOutline(XCursor, YCursor);
      }
      XCursor -= 1;
      Draw.CursorOutline(XCursor, YCursor);
      delay(175);
    }
  }
  //moves cursor outline on y
  if(YJoy > 611){
    if(YCursor < 7){
      if((XLog == XCursor) && (YLog == YCursor)){
        Draw.SelectOutline(XCursor, YCursor);
      }else if(LegalMovesLog[YCursor][XCursor]){
        Draw.LegalOutline(XCursor, YCursor);
      }else{
        Draw.BlankOutline(XCursor, YCursor);
      }
      YCursor += 1;
      Draw.CursorOutline(XCursor, YCursor);
      delay(175);
    }
  }else if(YJoy < 411){
    if(YCursor > 0){
      if((XLog == XCursor) && (YLog == YCursor)){
        Draw.SelectOutline(XCursor, YCursor);
      }else if(LegalMovesLog[YCursor][XCursor]){
        Draw.LegalOutline(XCursor, YCursor);
      }else{
        Draw.BlankOutline(XCursor, YCursor);
      }
      YCursor -= 1;
      Draw.CursorOutline(XCursor, YCursor);
      delay(175);
    }
  } 
}


void UpdateButton(){
  //checks button
  if((button.state() == LOW) && (ButtonState)){
    if(XLog == 8 && Board[YCursor][XCursor] != 6 && (Board[YCursor][XCursor]<6&&!Turn || Board[YCursor][XCursor]>6&&Turn)){
      XLog = XCursor;
      YLog = YCursor;
      GenerateLegalMoves();
    }else{
      if((XLog != XCursor || YLog != YCursor) && XLog != 8 && LegalMovesLog[YCursor][XCursor]){
        //saves undo
        UndoLog = XCursor+YCursor*8+XLog*64+YLog*512+Board[YCursor][XCursor]*4096+EnPassant*20480;
        //handels castle booleans
        if(Board[YLog][XLog]%7==1 && XLog==0 && LeftCastle){
          LeftCastle = false;
        }else if(Board[YLog][XLog]%7==1 && XLog==7 && RightCastle){
          RightCastle = false;
        }else if(Board[YLog][XLog]%7==5 && XLog==4 && (LeftCastle||RightCastle)){
          LeftCastle = false;
          RightCastle = false;
        }

        //main move logic
        if(Board[YLog][XLog]%7==0 && YCursor==0){  //handels promotion
          Board[YCursor][XCursor] = Board[YLog][XLog]+4;
          Board[YLog][XLog] = 6;
        }else if(Board[YLog][XLog]%7==0 && YLog==3 && XCursor==EnPassant){  //handels enpassant
          Board[YCursor+1][XCursor] = 6;
          Board[YCursor][XCursor] = Board[YLog][XLog];
          Board[YLog][XLog] = 6;
        }else if(YCursor==7 && ((XCursor==4&&XLog==0)||(XCursor==0&&XLog==4)) && LeftCastle){  //handels left castle
          Board[7][4] -= 4;
          Board[7][0] += 4;
        }else if(YCursor==7 && ((XCursor==4&&XLog==0)||(XCursor==0&&XLog==4)) && RightCastle){  //handels right castle
          Board[7][4] -= 4;
          Board[7][7] += 4;
        }else if(Board[YCursor][XCursor]%7==5){  //handels win loss
          while(true){
            Serial.println(Turn);
          }
        }else{  //normal move
          Board[YCursor][XCursor] = Board[YLog][XLog];
          Board[YLog][XLog] = 6;
        }
        
        //EnPassant set Logic
        if(Board[YCursor][XCursor]%7==0 && YLog==YCursor+2){
          EnPassant = XCursor;
        }else{
          EnPassant = 9;
        }

        //update/reset variables
        XLog = 8;
        YLog = 8;
        Turn = !Turn;
        
        //update board
        InvertBoard();
        LegalMoves.Reset();
        BoardSetup();
      }
    }
    ButtonState = false;
  }else if((button.state() == HIGH) && !(ButtonState)){
    ButtonState = true;
  }
}


void InvertBoard(){
  for(int x=0; x<8; x++){
    for(int y=0; y<4; y++){
      //Board[y][x] -> Board[7-y][x] & Board[7-y][x] -> Board[y][x]
      Board[y][x] += Board[7-y][x];
      Board[7-y][x] = Board[y][x]-Board[7-y][x];
      Board[y][x] -= Board[7-y][x];
    }
  }
}


//Ryan here, this won't be implimented until another button is declared
void UndoMove(){
  if((button.state() == LOW)&&(UndoLog != 184320)){
    //restore pieces
    InvertBoard();
    Board[round(UndoLog/512)%8][round(UndoLog/64)%8] = Board[round(UndoLog/8)%8][round(UndoLog)%8];
    Board[round(UndoLog/8)%8][round(UndoLog)%8] = round(UndoLog/4096)%5;
    BoardSetup();
    //set variables
    Turn = !Turn;
    EnPassant = round(UndoLog/20480);
    XLog = 8;
    YLog = 8;
    UndoLog = 184320;
  }
}



//----------MAIN LOOP----------

void loop(){
  UpdateCursor(analogRead(X_PIN), analogRead(Y_PIN));
  button.update();
  UpdateButton();
}
