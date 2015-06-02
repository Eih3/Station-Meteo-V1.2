#include <VirtualWire.h>

#define LED    13 
#define RX_PIN 12

String Temp; //Stockage de la température
String Humid; //Stockage de l'humidité

/**** Fonction configuration ****/
void setup()
{
  Serial.begin(9600);
  
  Serial.println("Station Meteo Arduino Emetteur"); 
  
  vw_set_tx_pin(RX_PIN);
  vw_setup(2000);      
  vw_rx_start();      
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  
  if (vw_get_message(buf, &buflen))
  {
    digitalWrite(LED, HIGH);
    
    char Char[buflen]; 
    
    for (int i = 0; i < buflen; i++)
    {
      Char[i]  = char(buf[i]); //Stockage du buffer dans un tableau
    }
    
    digitalWrite(LED, LOW);
 
    LectureDonnees(Char);   
  }
}

/**** Fonction lecture des données reçus ****/
void LectureDonnees(char* donnees)
{  
  String data = (String)donnees;
      
  Temp = data.substring(0, 2);
  Humid = data.substring(5, 7);
    
  Serial.println("Temperature : " + Temp + " oC");    
  Serial.println("Humidite : " + Humid + " %");    
  Serial.println("\n");  
}
