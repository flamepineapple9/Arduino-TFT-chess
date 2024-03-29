#include <SPI.h> 

#include <ButtonDebounce.h>

#define SW_PIN 2 //green wire
#define MIN_CLICK 100 //debounce ms
#define MAX_DCLICK 500 //double-click cutoff

ButtonDebounce button(SW_PIN, MIN_CLICK);// seems to be defining some values for the button debounce library

void setup() {
  randomSeed(analogRead(A5)); //Seed with bogus read
  Serial.begin(9600);
  update();
}


int Values[2][12]={
            {0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0}};

bool Latch[12]=
            {false,false,false,false,false,false,false,false,false,false,false,false};



void update(){
  delay(5000);
  for(int k=0; k<12; k++){
    if(Latch[k]){
      for(int i=0; i<10; i++){
        if(analogRead(k)!=0 && (Values[0][k]==0 || Values[0][k]<analogRead(k))){
          Values[0][k] = analogRead(k);
        }
        if(analogRead(k)!=0 && (Values[1][k]==0 || Values[1][k]>analogRead(k))){
          Values[1][k] = analogRead(k);
        }
      }
      Serial.println(k);
      Serial.println(Values[0][k]);
      Serial.println(Values[1][k]);
    }
  }
}

void loop(){
  delay(1);
}
