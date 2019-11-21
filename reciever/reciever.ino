//Laget av gruppe 13 under innovasjonsprosjektet i TTT4255, høsten 2019 ved NTNU.
//Det ligger inn en del linjer med Serial.print() kommandoer, men disse er kun for testing av systemet, så man kan se hvilke verdier man har fått.
//Henter inn diverse biblioteker man trenger å få ulik hardware til å fungere som ønsket.
#include <CapacitiveSensor.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal.h>
//Setter innganger for LCD-displayet.
  LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

//Definerer ulike variabler som brukes for tidtakingssekvensen.
  int minutter1 = 73;
  int sekunder1 = 0;
  int s_teller1 = 0;
  int teller1 = 0;
  int minutter2 = 73;
  int sekunder2 = 0;
  int s_teller2 = 0;
  int teller2 = 0;
  int tur = 1;
  int spillets_gang = 1;
  bool holding = 0;


RF24 radio(7, 8); // CE, CSN. Tallene forklarer hvilken pinneutgang disse to fra bluetooth-enheten, skal kobles til på Arduinoen.
const byte address[6] = "00001"; //Hvilken adresse som blåtann-enheten skal kommuisere på.

//En boolsk variabel som brukes for å lagre verdien som sendes via bluetooth.
  bool sensor_state = 0;

void setup() {  
  //Starter seriell kommunikasjon.
  Serial.begin(9600);
  //Starter radiokommunikasjon
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();//Gjør at bluetooth-enheten kun lytter etter signal og ikke sender ut signal.
  //Starter LCD-displayet, og definerer ar vi har 16*2 tegn til disposisjon.
  lcd.begin(16,2);
}

void loop() {
  //Kjører loopen hvis radiosignaler er tilgjenglige.
  if (radio.available()) {
    radio.read(&sensor_state, sizeof(sensor_state));    //Leser dataen fra bluetooth-kommuikasjonen
    //Serial.println(sensor_state);
    //Serial.println("");
    //Serial.println(test);

    //En logisk løkke som skiller mellom hvem sin tur det er, og når den skal gå videre til neste tur.
    //Tur 1 og 3 skal være turene der steinene er i bevegelse for de ulike lagene,
    if (sensor_state == HIGH && spillets_gang == 1 && tur == 3) {
      tur = 0;
      spillets_gang = 0;
    } 
    else if (sensor_state == LOW && spillets_gang == 0 && tur == 0){
      tur = 1;
    }
    else if (sensor_state == HIGH && spillets_gang == 0 && tur == 1){
      spillets_gang = 1;
      tur = 2;
    }
    else if (sensor_state == LOW && spillets_gang == 1) {
      tur = 3;
    } 
    
      
      
 
  }

//Hvis det er lag 1 sin tur så skal denne if-setningen telle ned tiden for lag 1 på LCD-displayet.
    if(tur == 0) {
//LCD-skjermen fungerer slik ar du må tømme den for hver nye loop, også printer du direkte ved kommandoen lcd.print()
      lcd.clear();
      lcd.print("Lag 1: ");
      lcd.print(minutter1);
      lcd.print(":"); 
      if(sekunder1 < 10){
        lcd.print("0");
      }

      lcd.print(sekunder1);
      lcd.print(" <-");
      s_teller1++;
      if (s_teller1 > 10){
      sekunder1--;
      s_teller1 = 0;}
      if(sekunder1 < 0) {
        sekunder1 = 59;
        minutter1--;
      }
      
      //Bytter til linje 2 på lcd-skjermen.
      lcd.setCursor(0,1);
      lcd.print("Lag 2: ");
      lcd.print(minutter2);
      lcd.print(":"); 
      if(sekunder2 < 10){
        lcd.print("0");
      }
      lcd.print(sekunder2);


//Tilsvarende som over, men bare for lag 2.        
    } else if(tur == 2) {
      lcd.clear();
      lcd.print("Lag 1: ");
      lcd.print(minutter1);
      lcd.print(":"); 
      if(sekunder1 < 10){
        lcd.print("0");
      }
      lcd.print(sekunder1);
      
      lcd.setCursor(0,1);
      lcd.print("Lag 2: ");
      lcd.print(minutter2);
      lcd.print(":"); 
      if(sekunder2 < 10){
        lcd.print("0");
      }
      
      lcd.print(sekunder2);
      lcd.print(" <-");
      
      s_teller2++;
      if (s_teller2 > 10){
      sekunder2--;
      s_teller2 = 0;}
      
      if(sekunder2 < 0) {
        sekunder2 = 59;
        minutter2--;
      }
    }
    //lcd.print(liste);
  
  delay(100);
}
