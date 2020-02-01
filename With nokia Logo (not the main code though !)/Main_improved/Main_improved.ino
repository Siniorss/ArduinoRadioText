#include<Keypad.h>
#include<string.h>
#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>
#include<stdlib.h>
#include<PCD8544.h>


RF24 radio(10 , 19);
//LCD setup
PCD8544 lcd(14 , 15 , 16 , 17 , 18);
//end LCD setup


 byte Address = 0 ;
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

char keys1[4] = {'.' , '!' , '?' , '1'};
char keys2[4] = {'A' , 'B' , 'C' , '2'};
char keys3[4] = {'D' , 'E' , 'F' , '3'};
char keys4[4] = {'G' , 'H' , 'I'  , '4'};
char keys5[4] = {'J' , 'K' , 'L' , '5'};
char keys6[4] = {'M' , 'N' , 'O'  , '6' };
char keys7[5] = {'P' , 'Q' , 'R' , 'S' , '7'};
char keys8[4] = {'T' , 'U' , 'V' , '8'};
char keys9[5] = {'W' , 'X' , 'Y' , 'Z' , '9'};
char keys0[2] = {' ' , '0'};

byte colPins[COLS] = {5 , 4 , 3 ,2}; //connect to the row pinouts of the keypad
byte rowPins[ROWS] = {9 , 8 , 7 , 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//end of Keypad setup



const char failed[] = {"Failed"};
const char statChanger = 'A';
bool sender = false;
char msg[100] ;
int ptr= 0 ;
char package[100] ; 
char delivery[] = "Delivered\0";
unsigned long lastPress ;
int pressCount = 1 ;
char lastKey = '\0' ;
bool caps = false ;

char screen[100] ;
int screenptr = 0 ;
int line = 0 ; 

unsigned long sendTime ;
bool undeliveredMSG = false ;
const int timeOut = 1000 ; //ms
const int failLimit = 3 ; //times
char deliveryText[] = "Delivery" ;


char *nokia =(char[]) { 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfc, 0xfe, 0xfe, 0xfc, 0xfc, 0xfe, 0xfc, 0xfc, 0xfc, 0xf8, 0xfc, 0xfe, 0xfe, 0xfe, 0xff, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xee, 0xc6, 0xcf, 0x8e, 0x86, 0x8e, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x1f, 0x1f, 0x1f, 0x3f, 0x3f, 0x7f, 0x3f, 0x3f, 0x7f, 0x3f, 0x3f, 0x3f, 0x1f, 0x3f, 0x3f, 0x3f, 0x1f, 0x0f, 0x17, 0x07, 0x0f, 0x0f, 0x07, 0x0f, 0x06, 0x06, 0x02, 0x10, 0x18, 0x0a, 0x6f, 0x37, 0x5e, 0x7f, 0xbe, 0xb6, 0xce, 0xcc, 0xdc, 0xfc, 0x7c, 0x7c, 0x7c, 0x7c, 0x38, 0x38, 0x3c, 0x3c, 0x3c, 0x38, 0x7c, 0x78, 0x7c, 0x7c, 0x7c, 0x7c, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5c, 0xfe, 0xfe, 0x0e, 0x3c, 0x78, 0xf0, 0xec, 0xfe, 0xfe, 0x7c, 0xfe, 0xce, 0xc6, 0xc6, 0xc6, 0xde, 0xfe, 0x7c, 0xfe, 0xfe, 0xbc, 0x38, 0x7c, 0xee, 0xc6, 0xc6, 0x00, 0xfe, 0xfe, 0x80, 0xe0, 0xf8, 0x7c, 0x7e, 0x6e, 0x7c, 0xf8, 0xe0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};




void setup() {
  Serial.begin(9600);
  lcd.begin(84, 48);
  clearScreen() ;
  radio.begin() ;
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(115);
  radio.setDataRate(RF24_250KBPS);
  
  lcd.drawBitmap(nokia , 84 , 48);
  delay(5000);
  clearScreen();
  lcd.clear();
  
  showWelcomePage();
  lastPress = millis();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////functions//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
  lcd.clear();
}

void showWelcomePage(){
  clearScreen();
  lcd.setCursor(15 , 0);
  lcd.print("Welcome");
  lcd.setCursor(0,  1);
  lcd.print(" * to set Address");
  lcd.setCursor(0 , 2);
  lcd.print(" # for Advanced settings");
  lcd.setCursor(0 , 3);
  lcd.print(" A to send a message");
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

     for (int i = 0 ; i < 3  &&undeliveredMSG; i++){  //tries to send the msg 3times 
       radio.stopListening() ;
       radio.openWritingPipe(add);
       Serial.println(msg);
       radio.write(&msg, sizeof(msg));
       sender=false;
       
       unsigned long currentTime = millis() ;
       while (millis() - currentTime < 500){ //waits 1 second and waits for the delivery msg
        radio.openReadingPipe(1 , add);
        radio.startListening() ;
        if (radio.available()){
          radio.read(&package , sizeof(package));
          Serial.println("got this after sending the msg from the receiver : ");
          Serial.println(package); 
          if (package[0]==delivery[0]){
            undeliveredMSG = false ;
            char success[] = "Sent          Press any key to continue!" ;
            clearScreen() ;
            show(success);
            char key2 ;
            do key2 = keypad.getKey(); while(!key2);
            showWelcomePage();
            break ;
          }else {
            Serial.println("got something from the receiver but it wasn't delivery !");
            clearScreen();
            char idkstring[] = "IDK man ! it's weird ! but your msg was probably sent my bad !";
            show(idkstring);
          }
        }
       }
     }
     if (undeliveredMSG){
      char failed[] = "Failed to send the message           Press any key to continue!";
      clearScreen() ;
      show (failed) ;
      undeliveredMSG = false ;
      char key2 ;
      do key2 = keypad.getKey(); while(!key2);
      showWelcomePage();
     }
     deleteMessage() ;
}

void deleteMessage(){
  while(ptr > 0){
      ptr--;
      msg[ptr] = '\0' ;
   }
   ptr = 0 ;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////// LOOP //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void loop() {
  char key = keypad.getKey();
  if (key){

  
//////////////////////////////////////////////////////////////////////////////////stat changer//////////////////////////////////////////////////////////////////////////////////
    
    if (key == statChanger && !undeliveredMSG) {   //make sender
      clearScreen() ;
      char string5[] = "Start typing      \n";
      show(string5);
      lcd.setCursor(0, 1);
      if (addressSet){
        sender = true ;
        return ;
      }else{
        char err[] ="set the Address first" ;
        show(err) ;
        return ;
      }
    }


///////////////////////////////////////////////////////////////////////////////set address//////////////////////////////////////////////////////////////////////////////////

    
    else if (key == '*' && !sender){     
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
      char string4[] = "       press any key to continue" ;
      show(string4);
      do key2[0] = keypad.getKey(); while(!key2[0]);
      showWelcomePage();
      return ;  
    }

//////////////////////////////////////////////////////////////////////////////////channel & transfer rate///////////////////////////////////////////////////////////////////

    else if (key == '#' && !sender){
      
      uint8_t ch = 0 ;
      char chs[4] ;
      chs[3] = '\0';
      char key2[] = {'\0','\0'} ;
      char hashtag[] = "Set channel:        " ;
      while(true){
        clearScreen();
        show(hashtag);        
        for (int i = 0 ; i < 3 ; i++){
          key2[0] = keypad.getKey();
          while(!key2[0] || key2[0] - '0' < 0 || key2[0] - '0' > 9){
            key2[0] = keypad.getKey() ;
          }
          show(key2);
          ch = ch * 10 + (key2[0]-'0');
          chs[i] = key2[0] ;
        }
        if (ch < 126) break ;
        else ch = 0 ;
      }
      clearScreen();
      show("Channel is now : ");
      show(chs);
      radio.setChannel(chs);
      show("       press any key to continue");
      
      do key2[0] = keypad.getKey(); while(!key2[0]);
      
      clearScreen();
      lcd.print("Now Data rate : ");
      lcd.setCursor(0,2);
      lcd.print("1  :  1MBPS");
      lcd.setCursor(0,3);
      lcd.print("2  :  2MBPS");
      lcd.setCursor(0,4);
      lcd.print("3  :  250KBPS");
      key2[0] = '\0';
      while(!key2[0] || key2[0]-'0' < 1 || key2[0]-'0' > 3) {
        key2[0] = keypad.getKey() ;
      }
      if (key2[0] == '1') radio.setDataRate(RF24_1MBPS);
      else if (key2[0] == '2') radio.setDataRate(RF24_2MBPS);
      else if (key2[0] == '3') radio.setDataRate(RF24_250KBPS);
      clearScreen();
      show("Data transfer has been set !          press any key to continue");
      do key2[0] = keypad.getKey(); while(!key2[0]);
      showWelcomePage();
      
    }


//////////////////////////////////////////////////////////////////////////////////sender//////////////////////////////////////////////////////////////////////////////////

    else if (sender){
      
      if (key == 'C'){    //clear 1 character
        if (ptr > 0){
          ptr--;
          msg[ptr] = '\0';
          clearScreen() ;
          show(msg);
          pressCount =  0;
        }
     }
     else if (key == 'B'){  //clear everything
        deleteMessage() ;
        clearScreen() ;
        show(msg);
        pressCount =  0;
     }
     else if (key == 'D'){  //send
       send() ;
       pressCount =  0;
     }
     else if (key == '#') caps = 1 - caps ;


//////////////////////////////////////////////////////////////////////////////////TYPE//////////////////////////////////////////////////////////////////////////////////


     
       else if (ptr < 100) { //type
        lastPress = millis() ;
        while(millis() - lastPress <1000){
          if (key && key){
            if (key == '#') {
              caps = 1 - caps ;
              return  ;
            }
            if (lastKey == key){
              pressCount++ ;
              lastPress = millis() ;
              ptr-- ;
            }
            else {
              pressCount = 0 ;
              lastKey = key ; 
              lastPress = millis() ;
            }

            char ch ;
            switch (lastKey) {
              case '0' :
                ch = keys0[pressCount % sizeof(keys0)] ;
                break ;
              case '1' :
                ch = keys1[pressCount % sizeof(keys1)] ;
                break ;
              case '2' :
                ch = keys2[pressCount % sizeof(keys2)] ;
                break ;
              case '3' :
                ch = keys3[pressCount % sizeof(keys3)] ;
                break ;
              case '4' :
                ch = keys4[pressCount % sizeof(keys4)] ;
                break ;
              case '5' :
                ch = keys5[pressCount % sizeof(keys5)] ;
                break ;
              case '6' :
                ch = keys6[pressCount % sizeof(keys6)] ;
                break ;
              case '7' :
                ch = keys7[pressCount % sizeof(keys7)] ;
                break ;
              case '8' :
                ch = keys8[pressCount % sizeof(keys8)] ;
                break ;
              case '9' :
                ch = keys9[pressCount % sizeof(keys9)] ;
                break ;
              default : 
                Serial.println("debug from case");
                Serial.println(key);
            }
            if (!caps && lastKey != '0' && lastKey != '1' && lastKey != '#' && (ch - '0' > 9 || ch - '0' < 0) ) ch = ch - ('A' - 'a');
              
            msg[ptr] = ch ;
            ptr++ ;
            clearScreen();
            show(msg) ;

            
          }
          key = keypad.getKey() ;
        }
        
//        clearScreen() ;
//        show(msg) ;
        pressCount = 0 ;
        lastKey = '\0';
      }
      lastPress = millis();
    }
  }


/////////////////////////////////////////////////////////////////////////////////receiver//////////////////////////////////////////////////////////////////////////////////

    if (!sender){
        radio.openReadingPipe(1 , Address);
        radio.startListening() ;
        if (radio.available()){
          radio.read(&package , sizeof(package));
          char notif[] = "message received       \n    " ;
          Serial.println(notif);
          Serial.println(package);
          clearScreen() ;
          show(notif);
          show(package);
          show("           press any key to continue!");
          radio.stopListening() ;
          radio.openWritingPipe(Address);
          radio.write(&delivery, sizeof(delivery));
          //undeliveredMSG=false;
          char key2 ;
          do key2 = keypad.getKey(); while(!key2);
          showWelcomePage();
        }
    }
}   
