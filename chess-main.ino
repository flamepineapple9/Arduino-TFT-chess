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
  
  
  //--------- ^ setup ^ --------- 
  //------ v actual code v ------
  
  
  Draw.Board(true);
}



//----------CONSTANTS----------

const unsigned int ColorArray[2][5] = 
 {{0x0000, 0x10A2, 0x2124, 0x4A49, 0x6B6D}, // black, dark --> light
  {0x0000, 0xA534, 0xC618, 0xE71C, 0xFFFF}}; // white, dark --> light


/*3d array, [piece][row][column], 
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


//----------VARIABLES----------

unsigned char PosMoves[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};


//Ryan here, each piece is represented by a number. Tens place is color (1 for black, 2 for white)
//and ones place is the piece number (reference PieceArray). 0 is null.
unsigned int Board[8][8] = {
  {21, 22, 23, 24, 25, 23, 22, 21},
  {20, 20, 20, 20, 20, 20, 20, 20},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {10, 10, 10, 10, 10, 10, 10, 10},
  {11, 12, 13, 14, 15, 13, 12, 11}
};


bool LegalMoves[8][8] = {
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false},
  {false,false,false,false,false,false,false,false}
};


unsigned int xCursor = 0;
unsigned int yCursor = 0;
unsigned int xLog = 8;
unsigned int yLog = 8;
bool buttonState = false;
bool cursorState = false;
unsigned int UndoStates = 0;
unsigned int EnPassant = 8;



//----------VISUAL FUNCTIONS----------

class Draw{
  public:
    void Piece(int xSquare, int ySquare, int piece){
      //xSquare and ySquare is the square on the board, 0=black 1=white, refference PiecesArray
      if (piece != 0){
        for(int y = 0; y < 14; y++){
          for(int x = 0; x < 14; x++){
            if (PiecesArray[piece%10][y][x] != 0x00){
              //             |  x position  |   y position  |                          color                                |
              mytft.drawPixel(xSquare*16+x+1, ySquare*16+y+1, ColorArray[round(piece/10)-1][PiecesArray[piece%10][y][x] - 1]);
            }
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


    void Board(bool color){
      //draws board
      if (color){
        for (int i = 0; i < 8; i++) {
          for (int k = 0; k < 8; k++){
            BlankSquare(k, i);
            BlankOutline(k, i);
            DrawPiece(k, i, Board[i][k]);
          }
        }
      }else{
        for (int i = 0; i < 8; i++) {
          for (int k = 7; k >= 0; k--) {
            BlankSquare(k, i);
            BlankOutline(k, i);
            DrawPiece(k, i, Board[i][k]);
          }
        }
      }
      CursorOutline(xCursor, yCursor);
    }
}



//----------INTERNAL FUNCTIONS----------

void MovePiece(int x1, int y1, int x2, int y2){
  //handels promotion
  if (((Board[y1][x1]==10)&&(y2==7))||((Board[y1][x1]==20)&&(y2==0))){
    Board[y1][x1] += 4;
  }
  //saves to undo
  //UndoStates = {x1, y1, Board[y1][x1], x2, y2, Board[y2][x2]};
  //moves piece
  Board[y2][x2] = Board[y1][x1];
  Board[y1][x1] = 0;
  BlankSquare(x1, y1);
  BlankOutline(x1, y1);
  BlankSquare(x2, y2);
  DrawPiece(x2, y2, Board[y2][x2]);
}
 

void UpdateCursor(int xJoy, int yJoy) { // moves the cursor
  //moves cursor outline on x
  if (xJoy > 611){
    if (xCursor < 7){
      if ((xLog == xCursor) && (yLog == yCursor)){
        SelectOutline(xCursor, yCursor);
      }else{
        BlankOutline(xCursor, yCursor);
      }
      xCursor += 1;
      CursorOutline(xCursor, yCursor);
      delay(250);
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
      delay(250);
    }
  }
  //moves cursor outline on y
  if (yJoy > 611){
    if (yCursor < 7){
      if ((xLog == xCursor) && (yLog == yCursor)){
        SelectOutline(xCursor, yCursor);
      }else{
        BlankOutline(xCursor, yCursor);
      }
      yCursor += 1;
      CursorOutline(xCursor, yCursor);
      delay(250);
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
      delay(250);
    }
  } 
}


void UpdateButton(){
  //checks button
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


/*
//Ryan here, DO NOT IMPLIMENT UNTILL NEW BUTTON IS DECLARED
void UndoMove(button){
  if ((button.state() == LOW)&&(UndoStates[0] != 8)){
    BlankSquare(UndoStates[0], UndoStates[1]);
    DrawPiece(UndoStates[0], UndoStates[1], UndoStates[2]);
    BlankSquare(UndoStates[3], UndoStates[4]);
    DrawPiece(UndoStates[3], UndoStates[4], UndoStates[5]);
  }
}
*/



//----------LEGAL MOVES FUNCTIONS----------

class LegalMoves {
  public:
    void VertHoriz(int x, int y, int piece, bool color){
      //VERTICAL
      //down from
      for(var i = y+1; i<=7; i++){
        if ((Board[i][x] == 0) || ((Board[i][x] > 19)&&(piece < 19)) || ((Board[i][x] < 19)&&(piece > 19))){
          LegalMoves[i][x] = true;
        }else{
          break;
        }
      }
      //up from
      for(var i = y-1; i>=0; i+-){
        if ((Board[i][x] == 0) || ((Board[i][x] > 19)&&(piece < 19)) || ((Board[i][x] < 19)&&(piece > 19))){
          LegalMoves[i][x] = true;
        }else{
          break;
        }
      }
      //HORZONTAL
      //right from
      for(var k = x+1; k<=7; k++){
        if (Board[y][k] == 0){
          LegalMoves[y][k] = true;
        }else{
          break;
        }
      }
      //left from
      for(var k = x-1; k>=0; k+-){
        if (Board[y][k] == 0){
          LegalMoves[y][k] = true;
        }else{
          break;
        }
      }
    }
    
    
    void Diagonal(int x, int y, int piece, bool color){
      //quadrant 1
      while(){
        
      }
      //quadrant 2
      while(){
        
      }
      //quadrant 3
      while(){
        
      }
      //quadrant 4
      while(){
        
      }
    }
    
    
    void King(int x, int y, int piece, bool color){
      if (piece == 15){
        for(var i=0, i<4, i++){
          if ((Board[y+round(i/2)*2-1][x+(i%2)*2-1] > 19)||(Board[y+round(i/2)*2-1][x+(i%2)*2-1]==0)){
            LegalMoves[][] = true;
          }
        }
      }
      if (piece == 15){
        for(var i=0, i<4, i++){
          if (Board[y+round(i/2)*2-1][x+(i%2)*2-1] < 19){
            LegalMoves[][] = true;
          }
        }
      }
    }
    
    
    void Knight(int x, int y, int piece, bool color){
      for(var i=0, i<8, i++){
        if (){
          LegalMoves[][] = true;
        }
      }
    }
    
    
    void Pawn(int x, int y, int piece, bool color){
      if (piece == 10){
        //normal move
        if ((Board[y-1][x]>19)||(Board[y-1][x]==0)){
          LegalMoves[y-1][x] = true;
        }
        //pawn jump
        if ((y==7)&&(Board[y-2][x]>19)||(Board[y-2][x]==0)){
          LegalMoves[y-2][x] = true;
        }
      }
      if (piece == 20){
        //normal move
        if (Board[y+1][x]<19){
          LegalMoves[y+1][x] = true;
        }
        //pawn jump
        if ((y==0)&&(Board[y+2][x]<19)){
          LegalMoves[y+2][x] = true;
        }
      }
      //en passant
      if ((piece==10)&&(y==4)&&(BlackEnPassant!=8)){
        if(BlackEnPassant==x-1){
          LegalMoves[][] = true;
        }
        if(BlackEnPassant==x+1){
          LegalMoves[][] = true;
        }
      }
      if ((piece==20)&&(y==3)&&(WhiteEnPassant!=8)){
        if(WhiteEnPassant==x-1){
          LegalMoves[][] = true;
        }
        if(WhiteEnPassant==x+1){
          LegalMoves[][] = true;
        }
      }
    }
    
    
    void Castle(int x, int y, int piece, bool color){
      //Ryan here, this needs work
      if (((piece < 19) && (Board[7][4] == 15))||((piece > 19) && (Board[0][4] == 25))){
        for (var i = 0, i < 2; i++){
          LegalMoves[][] = true;
          LegalMoves[][] = true;
        }
      }
    }
};


//Ryan here, still a big work in progress, might add in a bool.
void GenerateLegalMoves(int x, int y, int piece, bool color){
  switch (piece%10){
    //Pawn
    case 0:
      LegalMoves Pawn(int x, int y, int piece, bool color);
      break;
    //Rook
    case 1:
      LegalMoves VertHoriz(int x, int y, int piece, bool color);
      break;
    //Knight
    case 2:
      LegalMoves Knight(int x, int y, int piece, bool color);
      break;
    //Bishop
    case 3:
      LegalMoves Diagonal(int x, int y, int piece, bool color);
      break;
    //Queen
    case 4:
      LegalMoves VertHoriz(int x, int y, int piece, bool color);
      LegalMoves Diagonal(int x, int y, int piece, bool color);
      break;
    //King
    default:
      LegalMoves King(int x, int y, int piece, bool color);
  }
}


void ResetLegalMoves(){
  for (int x=0; x<8; x++){
    for (int y=0; y<8; y++){
      if (LegalMoves[y][x] == true){
        LegalMoves[y][x] = false;
      };
    };
  };
};



//----------MAIN LOOP----------

void loop(){
  UpdateCursor(analogRead(X_PIN), analogRead(Y_PIN));
  button.update();
  UpdateButton();
  delay(10); //Ryan here, just a small global delay.
}
