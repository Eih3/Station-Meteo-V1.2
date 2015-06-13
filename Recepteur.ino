/***********************************************************
* Nom Fichier: Recepteur.ino
*
* Programme: Recepteur Station Météo Arduino
*
* Fonctions: - Réception des valeurs par 433MHz          
*            - Affichage des valeurs sur le port série       
*
* Dernière MàJ: 04/06/2015
*
************************************************************
*    Michel Esc 2015, Open Source | @Eih3Prog +Eih3Prog
***********************************************************/

/**** Déclarations & Directives ****/
#include <VirtualWire.h>

#define LED    13 
#define RX     12

/**** Déclarations Variables ****/
String tempT0; // Stockage de la température DHT11
String tempT1; // Stockage température LM335
String tempT2; // Stockage température DS18S20
String humiH0; // Stockage de l'humidité DHT11

boolean cSharpApp;

/**** Fonction configuration ****/
void setup()
{
  pinMode(13, OUTPUT);
  
  Serial.begin(9600);
  
  vw_set_rx_pin(RX);
  vw_setup(4000);      
  vw_rx_start(); 
 
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
        Serial.println("Recepteur Station Meteo Arduino");     
        break;  
    } 
  }
  
  receptionValeurs(); 
}

/**** Fonction réception des valeurs ****/
void receptionValeurs()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  
  if (vw_get_message(buf, &buflen))
  {
    char Char[17];
    
    digitalWrite(LED, HIGH);    
    
    for (int i = 0; i < 17; i++) { Char[i]  = char(buf[i]); }
    
    digitalWrite(LED, LOW);   
  
    lectureValeurs(Char);
    
    affichageValeurs();
  }
 
}

/**** Fonction lecture des valeurs reçus ****/
void lectureValeurs(char* valeurs)
{  
  String data = (String)valeurs;
    
  tempT0 = data.substring(2, 4);
  tempT1 = data.substring(6, 8);
  tempT2 = data.substring(10, 12);
  humiH0 = data.substring(14, 16);
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
    
    Serial.println("DHT11 Hum : " + (String)humiH0 + " % HR");
  }    
}
