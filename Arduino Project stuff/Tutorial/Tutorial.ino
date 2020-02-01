  char incomingByte = 0 ;



  int in = 0 ;
  int out = 4 ;
  int outv = 0 ;
  
void setup() {
  // put your setup code here, to run once:
  // set up what the pins are supposed to do and stuff ...
  // what ever you write here is PUBLIC
  Serial.begin(9600);
  //  Serial.print("i'm gonna be printed just once at the beggining");

  pinMode(in , INPUT);
  pinMode(out , OUTPUT);

}

void loop() {
         // put your main code here, to run repeatedly:
        // whatever you write here is LOCAL
        
  //Serial.print("salam olaaghe aziz , halet chetore?"); 
//  if (Serial.available() > 0){ // it shows how many characters are left in the input ! it's string.length + 1
//    incomingByte = Serial.read() ;
//    Serial.println("incomingByte is : ");
//    Serial.println(incomingByte);
//  }
//  

  if (digitalRead(in) == 1){
      outv = 1 - outv ;
      digitalWrite(out , outv) ;
      Serial.print(digitalRead(in));
  }

}
