#include <DHT.h>
#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define led_red 11
#define led_green 12
#define button_1 13
#define button_2 2 
#define potentiometre A1
#define light_sensor A0 
#define buzzer 9
#define trig_pin 7
#define echo_pin 8
#define DHTTYPE DHT22
#define DHTPIN 4
#define GRB_red 3
#define GRB_green 5
#define GRB_blue 6
#define IRpin 10
LiquidCrystal_I2C lcd(0x27,16,2);
DHT dht(DHTPIN , DHTTYPE);
IRrecv IRrecv(IRpin);
bool shut_down = 0; //variable pour signaler l'arret du fonctionnement 
int q=1;// variable qui signale un defaut 
String error= "defaut"; //affiche un message derreur en cas de defaut 
 unsigned long compt_time=0;
 unsigned long total_compt_time=0;
 bool run;

float get_distance( ) {// cette fonction lis la distqnce grace au capteur ultra sonic 
  digitalWrite(trig_pin, LOW);
  digitalWrite(trig_pin, HIGH);
  delay(10);
  digitalWrite(trig_pin, LOW);
  long duration = pulseIn(echo_pin, HIGH);
  float distance = (duration * 0.034)/ 2;
  return distance;
  
}
void setColor( int red_value, int green_value, int blue_value){ //modifier les couleurs de la led RGB
  analogWrite(GRB_red, red_value);
  analogWrite(GRB_green, green_value);
  analogWrite(GRB_blue, blue_value);
  
}
bool mesDist(){  //affiche un message et alume la led RGB en fonction de la distance obtenue par le cqpteur ultra song 
  
    lcd.backlight();
    lcd.clear(); 
  float dist = get_distance();
  lcd.setCursor(0,0);
  if ( dist< 22){
    setColor(255, 0, 0);
    lcd.setCursor(0,0);
    lcd.print(F("OBSTACLE a:"));
    lcd.setCursor(0,11);
    lcd.print(dist);
    lcd.print(F("cm"));
    delay(500);
     }else if(dist>=22 && dist<32) {
      setColor(200, 37, 7);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("OBSTACLE a:"));
      lcd.setCursor(0,11);
      lcd.print(dist);
      lcd.print(F("cm"));
      delay(500);
     }else if( dist>=32 && dist<40){
      setColor(0, 255, 0);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("OBSTACLE a:"));
      lcd.setCursor(0,11);
      lcd.print(dist);
      lcd.print(F("cm"));
      delay(500);
      }else{
        lcd.clear();
        lcd.print(F("pret..."));
        setColor(0, 0, 0);
        } 
        return 1;
}

void button(){ //fonction pour gerer le fonctionnement du boutton 2 pour eteindre ou redemarrer le systhème
  int etat_button = digitalRead(button_2);
  unsigned long dure_appuis = 0;
   unsigned long  debut_appuis = 0;
  if (etat_button == HIGH ){
    debut_appuis = millis();
      while (digitalRead(button_2) == HIGH){
       }
       dure_appuis = millis() - debut_appuis;
       delay (50);
       
    if (dure_appuis < 800) {
      lcd.clear();
      Serial.println("appuis court");
      digitalWrite(led_green, LOW);
      digitalWrite(led_red, LOW);
      noTone(buzzer);

    setColor(0, 0, 0);
    run = 1;

    lcd.print("redemarrage");
    delay(1000);
    runing() ;
  
     } else if(dure_appuis >= 800) {
    Serial.println("appuis long");
    lcd.clear();
    setColor(0, 0, 0);
    digitalWrite(led_red, LOW);
    digitalWrite(led_green,LOW);
    digitalWrite(buzzer, LOW);
    shut_down = 1;
    run=0;
    loop();
    

    }
    }
  }


unsigned long  defaut_lumiere(){ //gère la detection de defaut lummineux
 int light_value= analogRead(light_sensor);
 //Serial.println(light_value);
  if(light_value < 100){
   compt_time=millis();
    
    q=1;
    lcd.clear();
   error = "ATTENTION ACCIDENT";
   //buzzer();
   while(q==1){
      defaut(compt_time);
    }
  }
  return compt_time;
}
unsigned long  defaut_humidite(){//gère la detrection de defaut par rapport a l'humidité
  
  float humidite = dht.readHumidity();
  float temperature = dht.readTemperature();
  while ( isnan(humidite) || isnan(temperature)){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("capteur error..."));
  }
  lcd.clear();
  if (humidite > 80){
   compt_time= millis();
    q=1;
    lcd.clear();
    error = "ATTENTION ELECTROCUTION";
    while(q==1){
      defaut(compt_time);
      
    }
    
  }
  return compt_time;
  
}

void defaut(unsigned long time_var ){ //gère le comportement de l'appareii en cas de detection d'un defaut 
if (digitalRead(button_1)== HIGH && q==1 ){
  setColor(0, 0, 0);
    lcd.clear();
    lcd.setCursor(0, 0);
    String ligne_1 = error.substring(0, 10);
    lcd.print(ligne_1);
    lcd.setCursor(0, 1);
    String ligne_2 = error.substring(10);
    lcd.print(ligne_2);
    digitalWrite(led_red, HIGH);
    digitalWrite(led_green, LOW);
    delay(100);
    if ((millis()-time_var) >= 180000){
      tone(buzzer, 330);
    }
    if((millis()- time_var)>= 240){
      if ( IRcaptor()!=0 ){
        while (digitalRead(button_1)== HIGH){
        digitalRead(button_1);
        setColor( 255, 0, 0);
        delay(100);
        setColor( 0,255, 0);
        delay(100);
        setColor( 0, 0,255);
        delay(100);
        }
      }
    }
  } 
    else{
    digitalWrite(led_red, LOW);
    digitalWrite(led_green, HIGH);
    setColor(0, 0, 0);
    noTone(buzzer);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("pret..."));
    q=0;
    }
  }
  uint16_t IRcaptor(){ // gère la reception des signaux par le capteur infra rouge 
    uint16_t received{0};
    if(IrReceiver.decode()){
      IrReceiver.printIRResultShort(&Serial);
      if( IrReceiver.decodedIRData.protocol == UNKNOWN){
        IrReceiver.printIRResultRawFormatted(&Serial, true );
      }
      if(IrReceiver.decodedIRData.protocol== NEC){
      received = IrReceiver.decodedIRData.command;
      Serial.println(received, HEX);
      }
      IrReceiver.resume();
    }
    return received;
  }
  void runing(){
    while(run){
        
digitalWrite(led_green, HIGH);
mesDist();
defaut_lumiere();
defaut_humidite();
button();
      }
  }
 
  

void setup() {  
pinMode(led_green , OUTPUT);
pinMode (led_red , OUTPUT);
Serial.begin(9600);
pinMode(button_1 , INPUT);
pinMode(button_2 , INPUT);
digitalWrite(led_green , LOW);
digitalWrite(led_red , LOW);
pinMode(buzzer, OUTPUT);
pinMode(trig_pin, OUTPUT);
pinMode( echo_pin, INPUT);
pinMode(GRB_red, OUTPUT);
pinMode(GRB_blue, OUTPUT);
pinMode(GRB_green, OUTPUT);
IRrecv.enableIRIn();
lcd.init ();
lcd.backlight();
lcd.setCursor(0, 0);
lcd.print("DHT 22 test!");
dht.begin();
delay(2000);
lcd.clear();
}
void loop() {
  // appelle des differents fonctions
  lcd.noBacklight();   
    if(digitalRead(button_1)== LOW){
      run=1;
      runing();
    }
  }
