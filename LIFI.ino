// auteurs : Pierre Gabin FODOP Gumete, Wilfried FONEN SIMO
//groupe:  4

// Inclusion des bibliotheque
#include <LiquidCrystal.h>
#include <TimerOne.h>

#include <analogComp.h>
//#include <LiquidCrystal.h>

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
int period = 50;
String message = "";
int dt =period;
const int tfin = period*4;
const int t0 = 3*period;
const int t1 = 2*period;

//parametre de demodulation
int countdemod = 0;
int tdemod = 0;
boolean premierDemod = false;
char cdemod = (char)0;
boolean td =true;

void setup() {
  // initialisation de la carte
  Serial.begin(115200);// vitesse de transmission
  
  // allumage des leds et preparation du bouton 
  pinMode(ledRouge, OUTPUT);
  pinMode(ledVerte, OUTPUT);
  digitalWrite(ledVerte, HIGH);
  pinMode(bp1, INPUT);
  
  // creation et initialisation de l'ecrant
  lcd.begin(16,2);
  lcd.clear ();
  
  // gestion des interuptions
  attachInterrupt(digitalPinToInterrupt(bp1), initEcran, RISING);
  analogComparator.setOn(INTERNAL_REFERENCE, A0);
  analogComparator.enableInterrupt(demodulation, FALLING);
  tdemod = millis(); // init le temps de decompte
  
  // tout a reussi alors bienvenu
  lcd.print("King of LIFI");  
  Serial.println("King of LIFI");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() >0){
   String mesgEntrant = Serial.readString();
    lcd.clear();
    lcd.print(mesgEntrant);
    // debut de transmission
    digitalWrite(ledVerte, LOW);
    for(int i = 0; i< mesgEntrant.length(); i++){
      char c = mesgEntrant[i];
      int b = 0xb00000001;
      for(int j = 0; j<8;j++){
        if(c&b){// si le bit du caractere en position i vaut 1
          digitalWrite(ledRouge, HIGH);
          delay(dt);
          digitalWrite(ledRouge, LOW);
          delay(t1);
        }else {// si le bit du caractere en position i vaut 0
          digitalWrite(ledRouge, HIGH);
          delay(dt);
          digitalWrite(ledRouge, LOW);
          delay(t0);
        }
        b = b << 1;
      }    
      // on envoi le signal de fin de transmission, pour un caracterre 
      digitalWrite(ledRouge, HIGH);
      delay(dt);
      digitalWrite(ledRouge, LOW);
      delay(tfin);
    }
    digitalWrite(ledRouge, HIGH);  
    delay(dt);      
    digitalWrite(ledRouge, LOW); 
    digitalWrite(ledVerte, HIGH);
    lcd.setCursor(0,1);
    Serial.print(message);
    lcd.print(message);
    message ="";
    premierDemod = true;
  }
  
}


void initEcran(){
  lcd.clear();
  lcd.setCursor(0,0);
}

void demodulation(){
  if(millis() -tdemod - dt >=period/2){// on evite les rebonds rapides
  if(premierDemod){
    if( millis() -tdemod - dt <= t1+period/2){// le bit vaut un
      cdemod = cdemod | (1 << countdemod);
      countdemod++;
    }else{
      if( millis() -tdemod - dt <= t0+period/2){// le bit vaut zero
        cdemod = (cdemod & ~(1 << countdemod));
        countdemod++;
      }else{
        if( millis() -tdemod - dt <= tfin+period/2){
            message = message + cdemod;
            Serial.print(cdemod);
            countdemod = 0;
            cdemod =0;
        }
      }
    }
  }else{
    premierDemod = true;
  }
  tdemod = millis();
  }
}
