#include<SPI.h>
#include<RF24.h>
#include<PCD8544.h>
#include<Keypad.h>



//keyPad setup
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte colPins[COLS] = {5 , 4 , 3 ,2}; //connect to the row pinouts of the keypad
byte rowPins[ROWS] = {9 , 8 , 7 , 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//end of Keypad setup

//radio setup
RF24 radio(10 , 19);
//end of radio setup
byte Address ;
bool addressSet = false ; 


char text[30] ;
PCD8544 lcd(14 , 15 , 16 , 17 , 18);
char screen[100] ;
int screenptr = 0 ;

void show(char m[]) {
  
  int i = 0 ;
  while(m[i] != '\0' && m[i] != NULL){
    screen[screenptr] = m[i] ;
    i++ ;
    screenptr++ ;
  }
  screen[screenptr] = '\0' ;

  lcd.clear() ;
  lcd.print(screen);  
}

void clearScreen(){
  for (int i = 0 ; i < 100 ; i++)   screen[i] = '\0';
  screenptr = 0 ;
  lcd.clear() ;
}

void setup() {  
  Serial.begin(9600);
  
  lcd.begin(84 , 48);
  clearScreen() ;

  radio.begin() ;
  radio.setChannel(115) ;
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  delay(300) ;
}

void loop(){
    radio.openReadingPipe(1 , Address);
    radio.startListening();
    if (radio.available()){
      radio.read(&text , sizeof(text));
      Serial.println(text);
//      lcd.print(text);
      clearScreen();
      char string[] = "message received :      "; // adjust the amount of spaces acordingly 
      show(string) ;
      show(text);
    }

    char key = keypad.getKey();
    if (key){
      if (key == '*'){     //set address
        clearScreen() ;
        char string1[] = "setting address : \n" ;
        show(string1);
        char* key2 = (char*) malloc(1);
        *key2 = keypad.getKey() ;
        while (! *key2 || *key2 - '0' > 9 || *key2 - '0' < 0) *key2 = keypad.getKey() ;
        *( key2 + 1) = '\0' ;
        show(key2) ;
        char* key3 = (char*) malloc(1);
        *key3 = keypad.getKey() ;
        while (! *key3  || *key3 - '0' > 9 || *key3 - '0' < 0) *key3 = keypad.getKey() ;
        *((key3) + 1) = '\0' ;
        show(key3) ;
        byte ad = ((*key2 - '0') * 10) + (*key3 - '0') ;
        Address = ad ;
  
        if (Address > 32) {Address = 32 ; }
        if (Address < 0 ){ Address = 0 ; }
        addressSet = true ;
        clearScreen() ;
        char string2[] = "new address has been set : " ;
        show(string2);
        char thead[3] = {(char)(Address / 10 + '0') , (char)(Address % 10 + '0') , '\0'} ;
        char spaces[] = "      " ;
        show(spaces);
        show(thead) ;
        return ;  
      }
    }
  
}
