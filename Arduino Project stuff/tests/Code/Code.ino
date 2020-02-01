#include<Keypad.h>
#include<string.h>
#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>
#include<stdlib.h>
#include<PCD8544.h>



 byte Address = 0 ;
 char AddressString[2] = {"00"} ;
 bool addressSet = false ;

 
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


//Radio setup
RF24 radio(10 , 19) ;
//end Radio setup

//LCD setup
PCD8544 lcd(14 , 15 , 16 , 17 , 18);
//end LCD setup


const char failed[] = {"Failed"};
const char statChanger = 'A';
bool sender = false;
char msg[100] ;
int ptr= 0 ;

char screen[100] ;
int screenptr = 0 ;
int line = 0 ; 

unsigned long sendTime ;
bool undeliveredMSG = false ;
const int timeOut = 1000 ; //ms
const int failLimit = 3 ; //times
char deliveryText[] = "Delivery" ;

void setup() {
  Serial.begin(9600);

  lcd.begin(84 , 48);

  clearScreen() ;
  radio.begin() ;
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(115);
  radio.setDataRate(RF24_250KBPS);
}



void send(){  
 

  clearScreen();
  char givead[] = {"Address ?"};
  show(givead) ;

      char* key2 = (char*) malloc(1);
      *key2 = keypad.getKey() ;
      while (! *key2 || *key2 - '0' > 9 || *key2 - '0' < 0) *key2 = keypad.getKey() ;
      clearScreen() ;
      *( key2 + 1) = '\0' ;
      show(key2) ;
      char* key3 = (char*) malloc(1);
      *key3 = keypad.getKey() ;
      while (! *key3  || *key3 - '0' > 9 || *key3 - '0' < 0) *key3 = keypad.getKey() ;
      *((key3) + 1) = '\0' ;
      show(key3) ;
      byte add = ((*key2 - '0') * 10) + (*key3 - '0') ;
      if (add > 32) add = 32 ;
      if (add < 0) add = 0 ;
      
  sendTime = millis() ;
  undeliveredMSG = true ;
  
  char ad[2] = {(char)(add / 10 + '0') , (char)(add % 10 + '0')} ;
  
  Serial.println() ;
  Serial.println("destination address : ") ;
  Serial.println(ad);

  Serial.println("here is what you're supposed to send :") ;
  Serial.print(msg) ;

  
  for (int i = 0 ; i < 3 && undeliveredMSG ; i++){
    radio.stopListening() ;
    radio.openWritingPipe(add) ;
    radio.write(&msg , sizeof(msg)) ;
    //deliery expecting !
    radio.openReadingPipe(add) ;
    radio.startListening() ;
    unsigned long thisTime = millis(); 
    char package[20] ;
    
    while(!radio.available() && millis() - thisTime < 1000);
    if (radio.available()){
      while(radio.available()){
            radio.read(&package , sizeof(package));
            if (strcmp(package , deliveryText)){
              undeliveredMSG = false ;
            }
      }
    }

  }
  
}




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
  for (int i = 0 ; i < 100 ; i++){
    screen[i] = '\0';
  }
  screenptr = 0 ;
  lcd.clear() ;
}


void loop() {
  char key = keypad.getKey();
  if (key){
    if (key == statChanger && !undeliveredMSG) {   //make sender
      Serial.println();
      Serial.println("ok what's your message ? ");
      clearScreen() ;
      show("what's the message ?");
      lcd.setCursor(0 , 1) ;
      if (addressSet){
        sender = true ;
        radio.stopListening() ;
        return ;
      }else{
        char err[] ="set the Address first" ;
        show(err) ;
        
        return ;
      }
    }
    else if (key == '*'){     //set address
      clearScreen() ;
      show("setting address : \n");
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
      show("new address has been set : ");
      char thead[3] = {(char)(Address / 10 + '0') , (char)(Address % 10 + '0') , '\0'} ;
      show("      ");
      show(thead) ;
      show("          press A to write a message");
      AddressString[0] = thead[0] ;
      AddressString[1] = thead[1] ;
      return ;  
    }



    else if (sender){
      Serial.println("sender");
      if (key == 'C'){    //clear 1 character
        if (ptr > 0){
          ptr--;
          msg[ptr] = '\0';
          clearScreen() ;
          show(msg);
        }
     }
     else if (key == 'B'){  //clear everything
        while(ptr > 0){
          ptr-- ;
          msg[ptr] = '\0';
        }
        ptr = 0 ;
        clearScreen() ;
        show(msg);
     }
     else if (key == 'D'){  //send
      send();
      sender = false ;
      radio.openReadingPipe(0 , Address) ;
      while(ptr > 0){
        ptr--;
        msg[ptr] = '\0' ;
      }
      return ;
     }
      else if (sender && ptr < 100) { //type
        msg[ptr] = key ;
        ptr++ ;
        clearScreen() ;
        show(msg) ;
        
      }
    }
  }







   //receiver 


   if (!sender){
      if (radio.available()) {
          char package[100] = "";
          while(radio.available(){
            radio.read(&package , sizeof(package)) ;
          }
          Serial.println("package recieved : ");
          Serial.println(package);
          if (strcmp(package , "Delivery") ){
            Serial.println("strcmp says it's delivery !");
            undeliveredMSG = false ;
          }
          else if (strcmp(package , AddressString) ) {
            long unsigned time1 = millis(); 
            while(!radio.available() && millis() - time1 < 3000){
              Serial.println("receiver");
              radio.read(&package , sizeof(package)) ;
              clearScreen() ;
              strcpy(msg , package) ;
              show(msg) ;
              radio.write(&answer , sizeof(answer));
            }
          }   
      }
  }

}
