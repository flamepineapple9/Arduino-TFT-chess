/*
PROJECT NAME: Mineplace
PROJECT VERSION: Sub 1.0
PROJECT DESCRIPTION: Minecraft ripoff designed for an Arduino Uno/Pro Mini with a 1.8" tft
                     display and a joystick (will be d-pad) control scheme.
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
  game_setup();
}





//----------CONSTANTS----------

const unsigned int WORLD_X = 7;
const unsigned int WORLD_Y = 7;
const unsigned int WORLD_Z = 7;
const unsigned int FOCAL = 4;
const unsigned int SCALE = 1;
const unsigned int SCREEN_HEIGHT = 128;
const unsigned int SCREEN_WIDTH = 180;



//----------VARIABLES----------

//        [x][y][z]
bool World[8][8][8] = 
  {{{true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false}},
    
   {{true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false}},
    
   {{true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false}},
    
   {{true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false}},
    
   {{true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false}},
    
   {{true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false}},
    
   {{true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false}},
    
   {{true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false}}};


//player location data
unsigned float PlayerPitch = 0; //from negative z
unsigned float PlayerYaw = 0;   //from positive x
unsigned float PlayerX = 0;
unsigned float PlayerY = 0;
unsigned int PlayerZ = 3;     //from feet
unsigned int JumpTimer = 0;


//player internal data
unsigned int PlayerDirt = 0;





//----------GAME MECHANICS----------

void PlayerUpdate(){
  JumpUpdate();
  MovementUpdate();
}


//Ryan here, update for head Z0
void JumpUpdate(){
  if(JumpTimer==0 && World[round(PlayerX)][round(PlayerY)][PlayerZ-1] && button.state()==LOW){
    PlayerZ++;
    JumpTimer+=5;
  }else if(JumpTimer==0 && !World[round(PlayerX)][round(PlayerY)][PlayerZ-1]){
    PlayerZ--;
  }else if(JumpTimer!=0){
    JumpTimer--;
  }
}


void MovementUpdate(){
  return;
}





//----------RENDERING----------

void Render(){
  for(var l=WORLD_X+WORLD_Y+WORLD_Z; l>0; l--){                             //layer
    for(var z=round(PlayerZ)-WORLD_Z; z<=WORLD_Z-round(PlayerZ); z++){     //z
      if(l-abs(z)>0){                                                     //l z ratio possible?
        for(var r=0; r<=l-abs(z); r++){                                  //ratio
          if(){                                                         //inner ratio possible?
            if(&&World[round(PlayerX)+r][round(PlayerY)+r][z]){  //Ryan here, could error, out of index, should be fine
              GenerateSides(round(PlayerX)+r,round(PlayerY)+r,z);
            }
            if(&&World[round(PlayerX)-r][round(PlayerY)+r][z]){
              GenerateSides(round(PlayerX)-r,round(PlayerY)+r,z);
            }
            if(&&World[round(PlayerX)-r][round(PlayerY)-r][z]){
              GenerateSides(round(PlayerX)-r,round(PlayerY)-r,z);
            }
            if(&&World[round(PlayerX)+r][round(PlayerY)-r][z]){
              GenerateSides(round(PlayerX)+r,round(PlayerY)-r,z);
            }
          }
        }
      }else if(l-abs(z)==0){                                               //if middle
        GenerateSides(round(PlayerX),round(PlayerY),z)
      }
    }
  }
}


void GenerateSides(int x, int y, int z){
  if(round(PlayerX)<x){
    Rasterize(ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z));
  }
  if(round(PlayerX)>x){
    Rasterize(ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z));
  }
  if(round(PlayerY)<y){
    Rasterize(ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z));
  }
  if(round(PlayerY)>y){
    Rasterize(ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z));
  }
  if(PlayerZ<z){
    Rasterize(ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z));
  }
  if(PlayerZ>z){
    Rasterize(ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z),ProjX(x,y,z),ProjY(x,y,z));
  }
}


//Ryan here, these are very inefficient
void ProjX(int x, int y, int z){
  //roll
  if(x>=0 && y>0){ //quadrant 1
    x = cos(acos(x/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
    y = sin(asin(y/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
  }else if(x>=0 && y<=0){ //quadrant 2
    x = sin(asin(x/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
    y = sin(asin(y/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
  }else if(x<0 && y<=0){ //quadrant 3
    x = sin(asin(x/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
    y = cos(acos(y/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
  }else{ //quadrant 4
    x = cos(acos(x/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
    y = cos(acos(y/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
  }
  //pitch
  if(y>=0 && z>0){ //quadrant 1
    y = cos(acos(y/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
    z = sin(asin(z/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
  }else if(y>=0 && z<=0){ //quadrant 2
    y = sin(asin(y/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
    z = sin(asin(z/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
  }else if(y<0 && z<=0){ //quadrant 3
    y = sin(asin(y/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
    z = cos(acos(z/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
  }else{ //quadrant 4
    y = cos(acos(y/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
    z = cos(acos(z/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
  }
  return SCALE*FOCAL*x/(FOCAL+z)+SCREEN_WIDTH/2;
}


void ProjY(int x, int y, int z){
  //roll
  if(x>=0 && y>0){ //quadrant 1
    x = cos(acos(x/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
    y = sin(asin(y/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
  }else if(x>=0 && y<=0){ //quadrant 2
    x = sin(asin(x/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
    y = sin(asin(y/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
  }else if(x<0 && y<=0){ //quadrant 3
    x = sin(asin(x/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
    y = cos(acos(y/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
  }else{ //quadrant 4
    x = cos(acos(x/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
    y = cos(acos(y/sqrt(x**2+y**2))+a)*sqrt(x**2+y**2);
  }
  //pitch
  if(y>=0 && z>0){ //quadrant 1
    y = cos(acos(y/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
    z = sin(asin(z/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
  }else if(y>=0 && z<=0){ //quadrant 2
    y = sin(asin(y/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
    z = sin(asin(z/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
  }else if(y<0 && z<=0){ //quadrant 3
    y = sin(asin(y/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
    z = cos(acos(z/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
  }else{ //quadrant 4
    y = cos(acos(y/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
    z = cos(acos(z/sqrt(y**2+z**2))+a)*sqrt(y**2+z**2);
  }
  return SCALE*FOCAL*y/(FOCAL+z)+SCREEN_WIDTH/2;
}


void Rasterize(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3){
  screen.fillTriangle(x0,y0,x1,y1,x2,y2,BROWN);
  screen.fillTriangle(x0,y0,x3,y3,x2,y2,BROWN);
  screen.drawLine(x0,y0,x1,y1,BROWN);
  screen.drawLine(x1,y1,x2,y2,BROWN);
  screen.drawLine(x2,y2,x3,y3,BROWN);
  screen.drawLine(x3,y3,x0,y0,BROWN);
}





//----------MAIN----------

void game_setup(){
  return;
}


void loop(){
  button.update();
  PlayerUpdate();
  Render();
  screen.display();
}
