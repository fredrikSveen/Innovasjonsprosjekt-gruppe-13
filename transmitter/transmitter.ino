//Laget av gruppe 13 under innovasjonsprosjektet i TTT4255, høsten 2019 ved NTNU.
#include <CapacitiveSensor.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
RF24 radio(9, 8); // CE, CSN. Tallene forklarer hvilken pinneutgang de skal kobles til.

const byte address[6] = "00001"; //Setter hvilken adresse blåtann-enheten skal sende dataene sine til.
  
  
// bestemmer hvilke pinner vi bruker til akselerometer
#define LIS3DH_CLK 7
#define LIS3DH_MISO 6
#define LIS3DH_MOSI 5
#define LIS3DH_CS 4

//Software for SPI 
  Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS, LIS3DH_MOSI, LIS3DH_MISO, LIS3DH_CLK);

  CapacitiveSensor cap = CapacitiveSensor(3,2); //Bruk (3,2) i faktisk stein.

  const int threshold = 300;
  const int ledPin = 10;
  bool sensor_state = 0;
  bool test = 1;
  bool isHold = false; 

 //Definerer konstanter som skal brukes senere i koden
  long tid;

  int k = 0;
  bool j = 0;
  float v_0x = 0;
  float v_0y = 0;
  float v_0z = 0;
  float a_x = 0;
  float a_y = 0;
  float a_z = 0;
  float a_tot = 0;
  float v_x = 0;
  float v_y = 0;
  float v_z = 0;
  float v_tot = 0;
  float v_0 = 0;
  float avvik_a_tot = 0;
  float avvik_x_snitt = 0;
  float avvik_y_snitt = 0;
  float avvik_z_snitt = 0;
  float avvik_x_tot = 0;
  float avvik_z_tot = 0;
  float avvik_y_tot = 0;
  bool stopp_tid = false;



  //Lager en funksjon som sjekker om steinen holdes i 
bool checkHold(){
    long sensorValue = cap.capacitiveSensor(30);
    if((sensorValue > threshold) || (sensorValue == threshold))
      { return (true);}
    else if (sensorValue < threshold) { 
      return(false);
    }
}


void setup() {
  //Starter radiosending
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  #ifndef _AVR_
  while (!Serial);
  // will pause Zero, Leonardo, etc until serial console opens
  #endif
  
  //Starter seriell kommunikasjon.
  Serial.begin(9600);
  //Serial.println("LIS3DH test!");

  //Sjekker om man får verdier og stopper koden hvis man ikke får det
  if ( !lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    //Serial.println("Couldnt start");
    while (1);
  }
  //Serial.println("LIS3DH found!");

  //Bestemmer hvor stort spenn av akselerasjon som kan måles
  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  //Serial.print("Range = "); Serial.print(2 << lis.getRange());
  //Serial.println("G");
 
  pinMode(ledPin, OUTPUT);
  
  
  sensors_event_t event;
  //måler 500 verdier for akselerasjon mens steinen står i ro. Gjøres for å kalibrere akselerometeret.
     for(int k = 0; k<500; k++) {
      lis.read();
      lis.getEvent(&event);
      if(k != 0){
        avvik_x_tot = event.acceleration.x + avvik_x_tot;
        avvik_z_tot = event.acceleration.z + avvik_z_tot;
        avvik_y_tot = event.acceleration.y + avvik_y_tot;
        if(k%20 == 0){
          //Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
          //Serial.print(" \tY: "); Serial.print(event.acceleration.y);
          //Serial.print(" \tZ: "); Serial.print(event.acceleration.z);
          //Serial.println(" TESTVERDIER");
        }
      }
    }
    //Bruker verdiene vi fikk tidligere til å regne ut snitt avviket på 1 måling
        avvik_x_snitt = avvik_x_tot/499;
        avvik_y_snitt = avvik_y_tot/499;
        avvik_z_snitt = avvik_z_tot/499;
        
        /*Serial.print("\t\tX: "); Serial.print(avvik_x_snitt);
        Serial.print("\tY: "); Serial.print(avvik_y_snitt);
        Serial.print("\t\tZ: "); Serial.print(avvik_z_snitt);
        Serial.println(" SNITTVERDIER");
        */ 
}




void updateSpeed(){
    sensors_event_t event;
    lis.getEvent(&event);
    //Serial.println(tid);

    //Fjerner snitt avviket fra akselerasjon på hver av aksene
    a_x = event.acceleration.x - avvik_x_snitt;
    a_y = event.acceleration.y - avvik_y_snitt;
    a_z = event.acceleration.z - avvik_z_snitt;
    //Serial.print("\tX: "); Serial.print(a_x);
    //Serial.print("\tY: "); Serial.print(a_y);
    //Serial.print("\tZ: "); Serial.print(a_z);

   //Definerer fart for x aksen
   
    v_x = a_x*((millis()-tid))+v_0x*1000;
    v_x = v_x/1000;
    //Serial.print("\tV_x = ");
    //Serial.print(v_x);
    v_0x = v_x;
    
    //Definerer fart for y aksen
    v_y = a_y*((millis()-tid))+v_0y*1000;
    v_y = v_y/1000;
    //Serial.print("\tV_y = ");
    //Serial.print(v_y);
    v_0y = v_y;

    //Definerer fart for z aksen
    v_z = a_z*((millis()-tid))+v_0z*1000;
    v_z = v_z/1000;
    //Serial.print("\tV_z = ");
    //Serial.println(v_z);
    v_0z = v_z;


    //Setter opp en verdi for total fart som skal være 0 når steinen står stille
    v_tot = abs(v_x) + abs(v_y) + abs(v_z);
    //Serial.println(v_tot);
}

void loop() {
    lis.read(); 
    tid = millis();
    
    long sensorValue = cap.capacitiveSensor(30);
    Serial.println(sensorValue);
   updateSpeed();
   //Serial.print("\tFart = ");
   //Serial.println(v_tot);

//Sjekker om du holder i den, og tenner LED'en hvis du gjør det. 
  if(checkHold() && k == 0){
    //Variabelen k blir satt til 1 dersom du holder i den.
    k = 1;
    digitalWrite(ledPin, HIGH);
    Serial.println("Du holder");
  }

  //Hvis du ikke holder i den og k er 1, så har du sluppet den og da sender i en 0 til recieveren.
  else if(!checkHold() && k == 1){
    digitalWrite(ledPin, LOW);
    //stopp_tid = true;
    sensor_state = 0;
    for(int i = 0; i < 30; i++) {
      radio.write(&sensor_state, sizeof(sensor_state));
    }
    //En pause i sending så lenge steinen er i bevegelse.
    while(a_x > 0.3 && a_y > 0.3){
      tid =  millis();
      updateSpeed();
    }
    //Når steien stopper sendes det en 1.
    sensor_state = 1;
    for(int i = 0; i < 30; i++) {
      radio.write(&sensor_state, sizeof(sensor_state));
    }
    k = 0;
  }
  //Har valgt å sende radiosignalene 30 ganger for å være sikker på at recieveren får med seg signalet.
  delay(1000);
}
