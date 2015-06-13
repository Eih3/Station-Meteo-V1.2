/***********************************************************
* Nom Fichier: Emetteur.ino
*
* Programme : Emetteur Station Météo Arduino
*
* Fonctions : - Lecture valeurs capteurs °t et HR%           
*             - Envoi des valeurs par 433MHz          
*             - Affichage des valeurs sur le port série
*
* Capteurs : - DHT11 Humidité + Température
*            - LM335 
*            - DS18S20
*
* Dernière MàJ : 07/06/2015
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
int tempT0 = 0; // température DHT11
int tempT1 = 0; // température LM335
int tempT2 = 0; // température DS18S20
int humiH0 = 0; // humidité DHT11

boolean cSharpApp;

OneWire ds(DS18S20);
dht DHT;

/**** Fonction configuration ****/
void setup()
{
  pinMode(LED, OUTPUT);
  
  Serial.begin(9600);

  Serial.println("Emetteur Station Meteo Arduino"); 
  
  vw_set_tx_pin(TX); 
  vw_setup(4000);  

  delay(1000);
}

void loop()
{
  if (Serial.available() > 0)
  {
    char serialRx = Serial.read();
    
    switch (serialRx)
    {
      case '#':
        cSharpApp = true;
        break;
      case 'A':
        cSharpApp = false;
        Serial.println("\nCommunication Arduino");
        break;
    }  
  }
  
  lectureValeurs();

  // Envoi de la trame ex: T033T135T239H033
  envoiValeurs("T0" + (String)tempT0 
             + "T1" + (String)tempT1
             + "T2" + (String)tempT2
             + "H0" + (String)humiH0);

  delay(500);
 
  affichageValeurs();  

  delay(DELAY * 1000);
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
  
  tempT0 = DHT.temperature;   
  delay(50); 
  
  tempT1 = (analogRead(LM335) * 0.484) - 273.15;  
  delay(50); 
  
  tempT2 = DS18S20Temp();
  delay(50);

  humiH0 = DHT.humidity;   
  delay(50);
}

/**** Fonction envoi des valeurs des capteurs ****/
void envoiValeurs(String valeurs)
{
  const char* data = valeurs.c_str();  

  digitalWrite(LED, HIGH);
  delay(50);

  vw_send((uint8_t *)data, strlen(data));

  vw_wait_tx(); 

  digitalWrite(LED, LOW);
}

/**** Fonction affichage des valeurs sur le port série ****/
void affichageValeurs()
{
  if (cSharpApp == true)
  {
    Serial.println("T0" + (String)tempT0
                 + "T1" + (String)tempT1
                 + "T2" + (String)tempT2
                 + "H0" + (String)humiH0);
    
  } else
  {      
    Serial.println("\nDHT11 : " + (String)tempT0 + " oC");   
    
    Serial.println("LM335 : " + (String)tempT1 + " oC"); 
    
    Serial.println("DS18S20 : " + (String)tempT2 + " oC");
    
    Serial.println("DHT11 : humi " + (String)humiH0 + " % HR");
  }
}
