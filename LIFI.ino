#include <analogComp.h>

// auteurs : Pierre Gabin FODOP Gumete, Wilfried FONEN SIMO
//groupe:  4

// Inclusion des bibliotheque

#include <TimerOne.h>
#include <LiquidCrystal.h>

// declaration des diodes et bouton
const int photoresistance = 97;
const int ledRouge = 37;
const int ledVerte = 36;
const int bp1 = 18;

// les parametre du liquid crystal et son initialisation
//const int d4  =5;
//const int d5 = 4;
//const int d6 = 3;
//const int d7 = 2;
//const int enable  =11;
//const int rs = 12;
LiquidCrystal lcd( 12,11, 5,4,3,2);

// les parrametres de fonctionement
const double R9 = 10000.0;
const double ConvR8 = 5.0/1023.0; 
double  vo, R8, iled = 400;
unsigned long period = 25;
String message = "";
int dt =period;
const int tfin = period*6;
const int t0 = 4*period;
const int t1 = 2*period;

int countdemod = 0;
int tdemod = 0;
boolean premierDemod = true;
char cdemod = (char)0;
boolean td =true;

void setup() {
  // initialisation de la carte
  Serial.begin(115200);
  pinMode(ledRouge, OUTPUT);
  pinMode(ledVerte, OUTPUT);
  digitalWrite(ledVerte, HIGH);
  pinMode(bp1, INPUT);
  lcd.begin(16,2);
  lcd.clear ();
  attachInterrupt(digitalPinToInterrupt(bp1), initEcran, RISING);
  analogComparator.setOn(INTERNAL_REFERENCE, A0);
  analogComparator.enableInterrupt(demodulation, FALLING);
  tdemod = millis(); // init
  lcd.print("King of LIFI");  
  Serial.println("King of LIFI");
  /**/
  lcd.setCursor(0,1);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()){
    String mesgEntrant = Serial.readString();
    lcd.clear();
    lcd.print(mesgEntrant);
    lcd.setCursor(0,1);
    digitalWrite(ledVerte, LOW);
    
    for(int i = 0; i< mesgEntrant.length(); i++){
      char c = mesgEntrant[i];
      int b = 0xb00000001;
      for(int j = 0; j<8;j++){
        if(c&b){
          digitalWrite(ledRouge, HIGH);
          delay(dt);
          digitalWrite(ledRouge, LOW);
          delay(t1);
        }else {
          digitalWrite(ledRouge, HIGH);
          delay(dt);
          digitalWrite(ledRouge, LOW);
          delay(t0);
        }
        b = b << 1;
      }    
      // on envoi le signal de fin de transmission 
      digitalWrite(ledRouge, HIGH);
      delay(dt);
      digitalWrite(ledRouge, LOW);
      delay(tfin);
    }
    digitalWrite(ledRouge, HIGH);  
    delay(dt/2);      
    digitalWrite(ledRouge, LOW);      
  }
}

void fintransmission(){
      digitalWrite(ledVerte, HIGH);
      lcd.print(message);
      premierDemod = true;
}

void initEcran(){
  lcd.clear();
  lcd.setCursor(0,0);
}

void demodulation(){
  if(!premierDemod){
    if( abs(millis() -tdemod - dt - t1) <= period/2){
      cdemod = cdemod | (1 << countdemod);
      countdemod++;
      Serial.println(millis() -tdemod - dt);
    }else{
      if( abs(millis() -tdemod - dt - t0) <= period/2){
        cdemod = (cdemod & ~(1 << countdemod));
        countdemod++;
        Serial.println(millis() -tdemod - dt);
      }else{
        if( abs(millis() -tdemod - dt - tfin) <= period/2){
            Serial.println(cdemod);
            message = message +cdemod;
            countdemod = 0;
            cdemod =0;
        }
      }
    }
  }else{
    premierDemod = false;
  }
  tdemod = millis();
}
