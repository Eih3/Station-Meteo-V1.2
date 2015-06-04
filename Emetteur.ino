/***********************************************************
* Nom Fichier: Emetteur.ino
*
* Programme: Emetteur Station Météo Arduino
*
* Fonctions: - Lecture valeurs capteurs °t et HR%           
*            - Envoi des valeurs par 433MHz          
*            - Affichage des valeurs sur le port série       
*
* Dernière MàJ: 04/06/2015
*
************************************************************
*    Michel Esc 2015, Open Source | @Eih3Prog +Eih3Prog
***********************************************************/

/**** Déclarations & Directives ****/
#include <dht.h>
#include <OneWire.h>
#include <VirtualWire.h>

#define DHT11     8
#define DS18S20   3
#define LM335     A0 

#define LED       2
#define TX        12
#define DELAY     5 // secondes avant chaque envoi

/**** Déclarations Variables ****/
float tempC0 = 0; // température DHT11
float tempC1 = 0; // température LM335
float tempC2 = 0; // température DS18S20
float humiC0 = 0; // humidité DHT11

OneWire ds(DS18S20);
dht DHT;

/**** Fonction configuration ****/
void setup()
{
  pinMode(LED, OUTPUT);
  
  Serial.begin(9600);

  Serial.println("Station Meteo Arduino Emetteur\n"); 
  
  vw_set_tx_pin(TX); 
  vw_setup(2000);  

  delay(1000);
}

void loop()
{
  lectureValeurs();

  // Envoi de la trame ex: C024.00C124.02C224.08H50.00
  envoiValeurs("C0" + (String)tempC0 
             + "C1" + (String)tempC1
             + "C2" + (String)tempC2
             + "H0" + (String)humiC0);

  delay(500);
 
  affichageValeurs();  

  delay(DELAY * 1000);
}

/**** Fonction envoi des valeurs des capteurs ****/
void envoiValeurs(String valeurs)
{  
  Serial.println("Valeurs envoyees : " + valeurs);

  const char* data = valeurs.c_str();  

  digitalWrite(LED, HIGH);
  delay(500);

  vw_send((uint8_t *)data, strlen(data));

  vw_wait_tx(); 

  digitalWrite(LED, LOW);
}

/**** Fonction lecture DS18S20 ****/
float DS18S20Temp()
{
  byte data[12];
  byte addr[8];

  if ( !ds.search(addr))
  {
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7])
  {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28)
  {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);
  
  for (int i = 0; i < 9; i++) { data[i] = ds.read(); }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB);
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;  
}

/**** Fonction lectures des valeurs des capteurs ****/
void lectureValeurs()
{
  int chk = DHT.read11(DHT11); // Lecture des données DHT11  
  delay(50);  
  
  tempC0 = DHT.temperature;   
  delay(50); 
  
  tempC1 = (analogRead(LM335) * 0.484) - 273.15;  
  delay(50); 
  
  tempC2 = DS18S20Temp();
  delay(50);

  humiC0 = DHT.humidity;   
  delay(50);
}

/**** Fonction affichage des valeurs sur le port série ****/
void affichageValeurs()
{
  Serial.println("DHT11 : " + (String)tempC0 + " oC");   
  
  Serial.println("LM335 : " + (String)tempC1 + " oC"); 
  
  Serial.println("DS18S20 : " + (String)tempC2 + " oC");
  
  Serial.println("DHT11 Hum : " + (String)humiC0 + " %");

  Serial.println("\n");
}
