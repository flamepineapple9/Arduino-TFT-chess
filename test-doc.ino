/* my current idea for how the game will work is as follows:
First a square on an invisible 16x20 grid (8x8 pixel squares) will be randomly selected, and for now it will be yellow. when the joystick
is moved the square below the invisible cursor will be colored red, untill the cursor is under the yellow square. When this happens, the
square will turn green and the game will end.

I plan to get there by making an array, which will be blank for all but the randomly selected square. when the joystick moves, the cursor
will move and the program will check if that square is the key. if it is not, the program will color the previous cursor location with 

the purpose of this code is to lay the framework for a game that could have obsticles to dodge, using the array to keep track of where 
those obsticles are located.


*/
//testing comment



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


//display is 128x160, or 16x20 8x8 squares
int displayGrid[20][16]; // array for the display grid, center [9][8]
int keyX, keyY; //the x and y value of the solution square, if found game ends

Adafruit_ST7735 mytft = Adafruit_ST7735(TFT_SS, DC, RST); // idk why this is needed
ButtonDebounce button(SW_PIN, MIN_CLICK);// seems to be defining some values for the button debounce library

//array for colors
unsigned int dispColors[] = { ST7735_BLACK, ST7735_RED, ST7735_ORANGE, ST7735_YELLOW, ST7735_GREEN, ST7735_CYAN, ST7735_BLUE, ST7735_MAGENTA, ST7735_WHITE };
byte numColors =  sizeof(dispColors) / sizeof(dispColors[0]); //how many colors

int xJoy, yJoy; //current joystick location
int xOldJoy, yOldJoy; //previous Joystick location
int xOld, yOld; //old cursor location
int xCur, yCur; //current cursor location
int xGrid, yGrid; //current cursor location on grid
int GameWon; // if the game has been won yet
int lastMove; //the last time the cursor moved


void setup() {
  mytft.initR(INITR_BLACKTAB); // for 1.8' TFT
  mytft.setTextWrap(false); // Allow text to run off the edge
  mytft.setRotation(0);
  mytft.fillScreen(ST7735_BLACK);
  mytft.setTextSize(1); //smallest
  mytft.setTextColor(ST7735_WHITE);
  mytft.setCursor(0,0);
  randomSeed(analogRead(A5)); //Seed with bogus read
  int pinX = -1;

  
  Serial.begin(9600);
  
}

void loop() {
  //at this point in the code the game has already been won
  //each loop is a new round of the game, so i have to set the previvious...
  //values manually so values from the previous round don't interfere.
  displayGrid[keyY][keyX] = 0; // reseting the solution square
  xOld = 64;
  yOld = 72;

  xOldJoy = 511;
  yOldJoy = 511;

  xCur = 64;
  yCur = 72;

  xGrid = 8;
  yGrid = 9;
  lastMove = 0;
  int GameWon = 0;

  keyX = random(0, 15);
  keyY = random(0, 19);
  

  displayGrid[keyY][keyX] = 1;
  mytft.fillScreen(0x00);
  mytft.setCursor(0,0);
  mytft.print("Game Array");
  mytft.fillRect(xCur, yCur, 8, 8, ST7735_RED);
  mytft.fillRect(map(keyX, 0, 15, 0, 120), map(keyY, 0, 19, 0, 152), 8, 8, ST7735_WHITE);
  int x = map(8, 0, 15, 0, 120);
  int y = map(9, 0, 19, 0, 152);
  
  Serial.print(x);
  Serial.print(y);

  while (GameWon == 0){
    button.update();
    

    //get joystick position
    xJoy = analogRead(X_PIN);//0 - 1023, center 511
    yJoy = analogRead(Y_PIN);//0 - 1023, center 511
    /*
    
      
    }
    */
    //joystick is not at resting
    //check if joystick was at resting before, if it was then move
   // if (millis() - lastMove > 250){
      move(xOld, yOld, xJoy, yJoy);   
  //  }

    
    
    
    if (displayGrid[yGrid][xGrid] == 1){
      endGame();
      break;
    }

    xOld = xCur;
    yOld = yCur;

    xOldJoy = xJoy;
    yOldJoy = yJoy;
    
    if (button.state() == LOW){
      GameWon = 1;
    }
  }
  //at this point in the code the game has already been won
  displayGrid[keyY][keyX] = 0; // reseting the solution square

  
  
}

void move(int xOld, int yOld, int xJoy, int yJoy) { // moves the red square
  
  if (abs(xJoy - 511) > 100) {
    if (xJoy > 511){
      yCur = yOld;
      xGrid = xGrid + 1;
      xCur = map(xGrid, 0, 15, 0, 120);
      
      Serial.println("move right");
    }
    if (xJoy < 511){
      yCur = yOld;
      xGrid = xGrid - 1;
      xCur = map(xGrid, 0, 15, 0, 120);
      Serial.println("move left");
    }
    mytft.fillRect(xOld, yOld, 8, 8, ST7735_BLACK);
    
  }
  if (abs(yJoy - 511) > 100){
    if (yJoy < 511){
      xCur = xOld;
      yGrid = yGrid + 1;
      yCur = map(yGrid, 0, 19, 0, 152);
      Serial.println("move down");
    }
    if (yJoy > 511){
      xCur = xOld;
      yGrid = yGrid - 1;
      yCur = map(yGrid, 0, 19, 0, 152);
      Serial.println("move up");
    } 
    mytft.fillRect(xOld, yOld, 8, 8, ST7735_BLACK);
  }
   

  if (displayGrid[yGrid][xGrid] == 0) {
    mytft.fillRect(xCur, yCur, 8, 8, ST7735_RED);
  }
  delay(250);
  //lastMove = millis();
}

void endGame(){
  
  mytft.print("You Win!");

  mytft.fillRect(xCur, yCur, 8, 8, ST7735_GREEN);
  delay(1000);
  mytft.fillRect(xCur, yCur, 8, 8, ST7735_YELLOW);
  delay(1000);
  

  GameWon = 1;
  
}
