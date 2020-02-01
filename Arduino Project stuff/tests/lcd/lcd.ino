#include<PCD8544.h>

PCD8544 lcd(14 , 15 , 16 , 17 , 18);
//uint_8t lcdpins[] = {14 , 15 , 16 , 17 , 18}


void setup() {
//
//  pinMode(14 , OUTPUT);
//  pinMode(15 , OUTPUT);
//  pinMode(16 , OUTPUT);
//  pinMode(17 , OUTPUT);
//  pinMode(18 , OUTPUT);
  
  lcd.begin(84 , 48);
  

}

void loop() {
//  for (int i = 20 ; i < 40 ; i++){
//    for (int j = 20 ; j < 40 ; j++){
//      
//    }
//  }
  char text[] = "salam shalgham";
  lcd.setCursor(0,0) ;
  lcd.print(text);

}
