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
#include <Adafruit_GFX_Buffer.h>
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
Adafruit_GFX_Buffer<Adafruit_ST7735> mytft = Adafruit_GFX_Buffer<Adafruit_ST7735>(80, 160,  Adafruit_ST7735(TFT_SS, DC, RST)  );
ButtonDebounce button(SW_PIN, MIN_CLICK);


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
  {8, 9, 10, 11, 12, 10, 9, 8},
  {7, 7, 7, 7, 7, 7, 7, 7},
  {6, 6, 6, 6, 6, 6, 6, 6},
  {6, 6, 6, 6, 6, 6, 6, 6},
  {6, 6, 6, 6, 6, 6, 6, 6},
  {6, 6, 6, 6, 6, 6, 6, 6},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {1, 2, 3, 4, 5, 3, 2, 1}};


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
unsigned int EnPassant = 8;
bool ButtonState = false;
bool CursorState = false;
bool RightCastle = true;
bool LeftCastle = true;
bool Turn = true;



//----------VISUAL FUNCTIONS----------

class DRAW{
  public:
    void CursorOutline(int k,int i){
      mytft.drawRect(k*16, i*16, 16, 16, 0xf800);
    }


    void SelectOutline(int k,int i){
      mytft.drawRect(k*16, i*16, 16, 16, 0x001f);
    }
    
    
    void BlankSquare(int k,int i){
      //Ryan here, if i=0, then (0 + k)%2 just means every other, but as we increase i, i+constant will oscillate between
      //even and odd, meaning (k0 + i0)%2 != (k0 + i1)%2, thus making a given row the inverse of the next.
      if((k + i) % 2 == 1){
        mytft.fillRect(k*16+1, i*16+1, 14, 14, 0x7EB2);
      } else {
        mytft.fillRect(k*16+1, i*16+1, 14, 14, 0x2447);
      }
    }


    void BlankOutline(int k,int i){
      if((k + i) % 2 == 1){
        mytft.drawRect(k*16, i*16, 16, 16, 0x7EB2);
      } else {
        mytft.drawRect(k*16, i*16, 16, 16, 0x2447);
      }
    }
    
    
    void Piece(int xSquare, int ySquare, int piece){
      //xSquare and ySquare is the square on the board, 0=black 1=white, refference PIECES_ARRAY
      if(piece != 6){
        for(int y = 0; y < 14; y++){
          for(int x = 0; x < 14; x++){
            if(PIECES_ARRAY[piece%7][y][x] != 0x00){
              //             |  x position  |   y position  |                           color                                |
              mytft.drawPixel(xSquare*16+x+1, ySquare*16+y+1, COLOR_ARRAY[round(piece/7)][PIECES_ARRAY[piece%7][y][x]-1]);
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
      Draw.Piece(k, i, Board[i][k]);
    }
  }
  Draw.CursorOutline(XCursor, YCursor);
}



//----------LEGAL MOVES FUNCTIONS----------

class LegalMoves {
  public:
    void VertHoriz(int x, int y, bool color){
      //VERTICAL
      //down from
      for(int i = y++; i<=7; i++){
        if(Board[i][x] == 6){
          LegalMovesLog[i][x] = true;
        }else{
          if(((Board[i][x] < 6)&&color) || ((Board[i][x] > 6)&&!color)){
            LegalMovesLog[i][x] = true;
          }
          break;
        }
      }
      //up from
      for(int i = y--; i>=0; i--){
        if(Board[i][x] == 6){
          LegalMovesLog[i][x] = true;
        }else{
          if(((Board[i][x] < 6)&&color) || ((Board[i][x] > 6)&&!color)){
            LegalMovesLog[i][x] = true;
          }
          break;
        }
      }
      //HORZONTAL
      //right from
      for(int k = x++; k<=7; k++){
        if(Board[y][k] == 6){
          LegalMovesLog[y][k] = true;
        }else{
          if(((Board[y][k] < 6)&&color) || ((Board[y][k] > 6)&&!color)){
            LegalMovesLog[y][k] = true;
          }
          break;
        }
      }
      //left from
      for(int k = x--; k>=0; k--){
        if(Board[y][k] == 6){
          LegalMovesLog[y][k] = true;
        }else{
          if(((Board[y][k] < 6)&&color) || ((Board[y][k] > 6)&&!color)){
            LegalMovesLog[y][k] = true;
          }
          break;
        }
      }
    }
    
    
    void Diagonal(int x, int y, bool color){
      //quadrant 1
      for(int i=1; i<=7; i++){
        if(Board[y-i][x+i]==6){
          LegalMovesLog[y-i][x+i] = true;
          continue;
        }else if(color&&(Board[y-i][x+i]<6)){
          LegalMovesLog[y-i][x+i] = true;
        }else if(!color&&(Board[y-i][x+i]>6)){
          LegalMovesLog[y-i][x+i] = true;
        }
        break;
      }
      //quadrant 2
      for(int i=1; i<=7; i++){
        if(Board[y-i][x-i]==6){
          LegalMovesLog[y-i][x-i] = true;
          continue;
        }else if(color&&(Board[y-i][x-i]<6)){
          LegalMovesLog[y-i][x-i] = true;
        }else if(!color&&(Board[y-i][x-i]>6)){
          LegalMovesLog[y-i][x-i] = true;
        }
        break;
      }
      //quadrant 3
      for(int i=1; i<=7; i++){
        if(Board[y+i][x-i]==6){
          LegalMovesLog[y+i][x-i] = true;
          continue;
        }else if(color&&(Board[y+i][x-i]<6)){
          LegalMovesLog[y+i][x-i] = true;
        }else if(!color&&(Board[y+i][x-i]>6)){
          LegalMovesLog[y+i][x-i] = true;
        }
        break;
      }
      //quadrant 4
      for(int i=1; i<=7; i++){
        if(Board[y+i][x+i]==6){
          LegalMovesLog[y+i][x+i] = true;
          continue;
        }else if(color&&(Board[y+i][x+i]<6)){
          LegalMovesLog[y+i][x+i] = true;
        }else if(!color&&(Board[y+i][x+i]>6)){
          LegalMovesLog[y+i][x+i] = true;
        }
        break;
      }
    }
    
    
    void King(int x, int y, bool color){
      for(int i=y-1; i<y+2; i++){
        for(int k=x-1; k<x+2; k++){
          if(((i>=0)&&(i<=7)&&(k>=0)&&(k<=7)&&!((i==0)&&(k==0))) && ((Board[i][k]==6)||((Board[i][k]<6)&&color)||((Board[i][k]>6)&&!color))){
            LegalMovesLog[i][k] = true;
          }
        }
      }
    }
    
    
    void Knight(int x, int y, bool color){
      for(int i=-2; i<=2; i++){
        for(int k=-2; k<=2; k++){
          if(((x+y+4)%2==1) && (i!=0) && (k!=0)){
            LegalMovesLog[i][k] = true;
          }
        }
      }
    }
    
    
    void Pawn(int x, int y, bool color){
      //normal move
      if((Board[y-1][x]==0) || ((Board[y-1][x]<6)&&color) || ((Board[y-1][x]>6)&&!color)){
        LegalMovesLog[y-1][x] = true;
      }
      //pawn jump
      if(((Board[y-2][x]==0) || ((Board[y-2][x]<6)&&color) || ((Board[y-2][x]>6)&&!color)) && (y==6)){
        LegalMovesLog[y-2][x] = true;
      }
      //en passant
      if(EnPassant && y==3){
        if(EnPassant==x--){
          LegalMovesLog[y-1][x-1] = true;
        }else if(EnPassant==x++){
          LegalMovesLog[y-1][x+1] = true;
        }
      }
    }
    
    
    void Castle(int x, int y, int piece, bool color){
      //Ryan here, this needs work
      if(piece%7==1){
        if(RightCastle||LeftCastle){
          LegalMovesLog[7][4] = true;
        }
      }else if(piece%7==5){
        if(RightCastle){
          LegalMovesLog[7][7] = true;
        }
        if(LeftCastle){
          LegalMovesLog[7][0] = true;
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


void GenerateLegalMoves(int x, int y, int piece, bool color){
  switch(piece%7){
    //Pawn
    case 0:
      LegalMoves.Pawn(x, y, color);
      break;
    //Rook
    case 1:
      LegalMoves.VertHoriz(x, y, color);
      LegalMoves.Castle(x, y, piece, color);
      break;
    //Knight
    case 2:
      LegalMoves.Knight(x, y, color);
      break;
    //Bishop
    case 3:
      LegalMoves.Diagonal(x, y, color);
      break;
    //Queen
    case 4:
      LegalMoves.VertHoriz(x, y, color);
      LegalMoves.Diagonal(x, y, color);
      break;
    //King
    default:
      LegalMoves.King(x, y, color);
      LegalMoves.Castle(x, y, piece, color);
  }
}



//----------INTERNAL FUNCTIONS----------

void MovePiece(int x1, int y1, int x2, int y2){
  //handels promotion
  if(((Board[y1][x1]==10)&&(y2==7))||((Board[y1][x1]==20)&&(y2==0))){
    Board[y1][x1] += 4;
  }
  //saves to undo
  //UndoStates = {x1, y1, Board[y1][x1], x2, y2, Board[y2][x2]};
  //moves piece
  Board[y2][x2] = Board[y1][x1];
  Board[y1][x1] = 0;
  Draw.BlankSquare(x1, y1);
  Draw.BlankOutline(x1, y1);
  Draw.BlankSquare(x2, y2);
  Draw.Piece(x2, y2, Board[y2][x2]);
}
 

void UpdateCursor(int XJoy, int YJoy) { // moves the cursor
  //moves cursor outline on x
  if(XJoy > 611){
    if(XCursor < 7){
      if((XLog == XCursor) && (YLog == YCursor)){
        Draw.SelectOutline(XCursor, YCursor);
      }else{
        Draw.BlankOutline(XCursor, YCursor);
      }
      XCursor += 1;
      Draw.CursorOutline(XCursor, YCursor);
      delay(250);
    }
  }else if(XJoy < 411){
    if(XCursor > 0){
      if((XLog == XCursor) && (YLog == YCursor)){
        Draw.SelectOutline(XCursor, YCursor);
      }else{
        Draw.BlankOutline(XCursor, YCursor);
      } 
      XCursor -= 1;
      Draw.CursorOutline(XCursor, YCursor);
      delay(250);
    }
  }
  //moves cursor outline on y
  if(YJoy > 611){
    if(YCursor < 7){
      if((XLog == XCursor) && (YLog == YCursor)){
        Draw.SelectOutline(XCursor, YCursor);
      }else{
        Draw.BlankOutline(XCursor, YCursor);
      }
      YCursor += 1;
      Draw.CursorOutline(XCursor, YCursor);
      delay(250);
    }
  }else if(YJoy < 411){
    if(YCursor > 0){
      if((XLog == XCursor) && (YLog == YCursor)){
        Draw.SelectOutline(XCursor, YCursor);
      }else{
        Draw.BlankOutline(XCursor, YCursor);
      }
      YCursor -= 1;
      Draw.CursorOutline(XCursor, YCursor);
      delay(250);
    }
  } 
}


void UpdateButton(){
  //checks button
  if((button.state() == LOW) && (ButtonState)){
    if((XLog == 8) && (Board[YCursor][XCursor] != 0)){
      XLog = XCursor;
      YLog = YCursor;
    }else{
      if(((XLog != XCursor) || (YLog != YCursor)) && (XLog != 8)){
        MovePiece(XLog, YLog, XCursor, YCursor);
        XLog = 8;
        YLog = 8;
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


/*
//Ryan here, still a work in progress
void UndoMove(button){
  if((button.state() == LOW)&&(UndoLog != )){
    
  }
}
*/



//----------MAIN LOOP----------

void loop(){
  UpdateCursor(analogRead(X_PIN), analogRead(Y_PIN));
  button.update();
  UpdateButton();
  mytft.display();
}
